#include "Renderer.h"

#include "Walnut/Random.h"
#include <ranges>

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
}

void Renderer::Render(const Scene& scene, const Camera& camera)
{
    m_ActiveScene = &scene;
    m_ActiveCamera = &camera;

    for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
    {
        for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
        {
            glm::vec4 color = PerPixel(x, y);
            color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
            m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color);
        }
    }

    m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
{
    Ray ray{};
    ray.Origin = m_ActiveCamera->GetPosition();
    ray.Direction = m_ActiveCamera->GetRayDirections()[x + y * m_FinalImage->GetWidth()];

    glm::vec3 color(0.0f);
    float multiplier = 1.0f;

    constexpr int bounces = 2;
    for (int i = 0; i < bounces; i++) {
        const Renderer::HitPayload payload = TraceRay(ray);
        if (payload.HitDistance < 0.0f) {
            glm::vec3 skyColor = glm::vec3(0.0f, 0.0f, 0.0f);
            color += skyColor * multiplier;
            break;
        }

        const glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));

        const float lightIntensity = glm::max(glm::dot(payload.WorldNormal, -lightDir), 0.0f); // == cos(angle)

        const Sphere& sphere = m_ActiveScene->Spheres[payload.ObjectIndex];
        glm::vec3 sphereColor = sphere.Albedo;
        sphereColor *= lightIntensity;
        color += sphereColor * multiplier;

        multiplier *= 0.7f;

        ray.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f;
        ray.Direction = glm::reflect(ray.Direction, payload.WorldNormal);
    }

    return { color, 1.0f };
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
    Renderer::HitPayload payload{};
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