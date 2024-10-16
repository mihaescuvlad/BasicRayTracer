#include "Renderer.h"

#include "Walnut/Random.h"
#include <ranges>
#include <execution>
#include <algorithm>

namespace Utils
{
    static uint32_t ConvertToRGBA(const glm::vec4& color)
    {
        const auto r = static_cast<uint8_t>(color.r * 255.0f);
        const auto g = static_cast<uint8_t>(color.g * 255.0f);
        const auto b = static_cast<uint8_t>(color.b * 255.0f);
        const auto a = static_cast<uint8_t>(color.a * 255.0f);

        const uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
        return result;
    }

    static uint32_t PCG_Hash(const uint32_t& input)
    {
        const uint32_t state = input * 747796405u + 2891336453u;
        const uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
        return (word >> 22u) ^ word;
    }

    static float RandomFloat(uint32_t& seed)
    {
        seed = PCG_Hash(seed);
        return static_cast<float>(seed) / static_cast<float>(std::numeric_limits<uint32_t>::max());
    }

    static glm::vec3 InUnitSphere(uint32_t& seed)
    {
        return glm::normalize(glm::vec3(
            RandomFloat(seed) * 2.0f - 1.0f,
            RandomFloat(seed) * 2.0f - 1.0f,
            RandomFloat(seed) * 2.0f - 1.0f)
        );
    }
}

void Renderer::OnResize(uint32_t width, uint32_t height)
{
    if (m_FinalImage)
    {
        // No resize necessary
        if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
            return;

        m_FinalImage->Resize(width, height);
    }
    else
    {
        m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
    }

    delete[] m_ImageData;
    m_ImageData = new uint32_t[width * height];

    delete[] m_AccumulationData;
    m_AccumulationData = new glm::vec4[width * height];
}

void Renderer::Render(const Scene& scene, const Camera& camera)
{
    m_ActiveScene = &scene;
    m_ActiveCamera = &camera;

    if (m_FrameIndex == 1)
        memset(m_AccumulationData, 0, m_FinalImage->GetWidth() * m_FinalImage->GetHeight() * sizeof(glm::vec4));

    const uint32_t width = m_FinalImage->GetWidth();
    const uint32_t height = m_FinalImage->GetHeight();
    const uint32_t totalPixels = width * height;

    auto indices = std::ranges::iota_view(0u, totalPixels);

    std::for_each(std::execution::par, indices.begin(), indices.end(), [this, width](uint32_t i) {
        const uint32_t x = i % width;
        const uint32_t y = i / width;

        const glm::vec4 color = PerPixel(x, y);
        m_AccumulationData[x + y * m_FinalImage->GetWidth()] += color;

        glm::vec4 accumulatedColor = m_AccumulationData[x + y * m_FinalImage->GetWidth()];
        accumulatedColor /= static_cast<float>(m_FrameIndex);
        accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));

        m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(accumulatedColor);
    });

    m_FinalImage->SetData(m_ImageData);

    if (m_Settings.Accumulate)
        m_FrameIndex++;
    else
        ResetFrameIndex();
}

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y) const
{
    Ray ray{};
    ray.Origin = m_ActiveCamera->GetPosition();
    ray.Direction = m_ActiveCamera->GetRayDirections()[x + y * m_FinalImage->GetWidth()];

    glm::vec3 light(0.0f);
    glm::vec3 contribution(1.0f);

    uint32_t seed = x + y * m_FinalImage->GetWidth();
    seed *= m_FrameIndex;

    constexpr int bounces = 5;
    for (int i = 0; i < bounces; i++) {
        seed += i;

        const HitPayload payload = TraceRay(ray);
        if (payload.HitDistance < 0.0f) {
            light += m_ActiveScene->SkyColor * contribution;
            break;
        }

        const Sphere& sphere = m_ActiveScene->Spheres[payload.ObjectIndex];
        const Material& material = m_ActiveScene->Materials[sphere.MaterialIndex];

        light += material.GetEmission() * contribution;

        glm::vec3 normal = payload.WorldNormal;
        glm::vec3 viewDir = -ray.Direction;

        glm::vec3 reflectDir = glm::reflect(ray.Direction, normal);
        glm::vec3 reflectedColor = glm::mix(material.Albedo, glm::vec3(1.0f), material.Metallic);
        glm::vec3 scatterDir = glm::normalize(normal + Utils::InUnitSphere(seed));

        contribution *= reflectedColor;

        float reflectance = material.Metallic + (1.0f - material.Metallic) * pow(1.0f - glm::dot(viewDir, normal), 5.0f);

        ray.Direction = glm::mix(scatterDir, reflectDir, reflectance);
        ray.Origin = payload.WorldPosition + normal * 0.0001f;
    }

    return { light, 1.0f };
}

Renderer::HitPayload Renderer::TraceRay(const Ray& ray) const
{
    uint32_t closestSphere = std::numeric_limits<uint32_t>::max();
    float hitDistance = std::numeric_limits<float>::max();

    for (auto&& [i, sphere] : m_ActiveScene->Spheres | std::views::enumerate)
    {
        const glm::vec3 origin = ray.Origin - sphere.Position;

        const float a = glm::dot(ray.Direction, ray.Direction);
        const float b = 2.0f * glm::dot(origin, ray.Direction);
        const float c = glm::dot(origin, origin) - (sphere.Radius * sphere.Radius);

        const float discriminant = (b * b) - 4.0f * a * c;

        if (discriminant < 0.0f)
            continue;

        // float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
        const float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);

        if(closestT > 0.0f && closestT < hitDistance)
        {
            hitDistance = closestT;
            closestSphere = i;
        }
    }

    if (closestSphere == std::numeric_limits<uint32_t>::max())
        return Miss(ray);

    return ClosestHit(ray, hitDistance, closestSphere);
}

Renderer::HitPayload Renderer::ClosestHit(const Ray& ray, float hitDistance, uint32_t objectIndex) const
{
    HitPayload payload;
    payload.HitDistance = hitDistance;
    payload.ObjectIndex = objectIndex;

    const Sphere& closestSphere = m_ActiveScene->Spheres[objectIndex];

    const glm::vec3 origin = ray.Origin - closestSphere.Position;
    payload.WorldPosition = origin + ray.Direction * hitDistance;
    payload.WorldNormal = glm::normalize(payload.WorldPosition);

    payload.WorldPosition += closestSphere.Position;

    return payload;
}

Renderer::HitPayload Renderer::Miss(const Ray& ray)
{
    HitPayload payload{};
    payload.HitDistance = -1.0f;

    return payload;
}