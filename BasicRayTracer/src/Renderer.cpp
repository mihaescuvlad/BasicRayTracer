#include "Renderer.h"

#include "Walnut/Random.h"

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

void Renderer::Render(const Scene& scene, const Camera& camera) const
{
    Ray ray;
    ray.Origin = camera.GetPosition();

    for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
    {
        for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
        {
            ray.Direction = camera.GetRayDirections()[x + y * m_FinalImage->GetWidth()];

            glm::vec4 color = TraceRay(scene, ray);

            color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
            m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color);
        }
    }

    m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::TraceRay(const Scene& scene, const Ray& ray)
{
    if (scene.Spheres.empty())
        return {0, 0, 0, 1};

    const Sphere* closestSphere = nullptr;
    float hitDistance = std::numeric_limits<float>::max();

    for(const Sphere& sphere : scene.Spheres)
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

        if(closestT < hitDistance)
        {
            hitDistance = closestT;
            closestSphere = &sphere;
        }
    }

    if (closestSphere == nullptr)
        return {0.0f, 0.0f, 0.0f, 1.0f};

    const glm::vec3 origin = ray.Origin - closestSphere->Position;
    const glm::vec3 hitPoint = origin + ray.Direction * hitDistance;
    const glm::vec3 normal = glm::normalize(hitPoint);

    const glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));

    const float lightIntensity = glm::max(glm::dot(normal, -lightDir), 0.0f); // == cos(angle)

    glm::vec3 sphereColor = closestSphere->Albedo;
    sphereColor *= lightIntensity;

    return { sphereColor, 1.0f };
}
