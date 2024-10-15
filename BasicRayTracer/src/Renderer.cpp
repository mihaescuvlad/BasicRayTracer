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

void Renderer::Render(const Camera& camera) const
{
    Ray ray;
    ray.Origin = camera.GetPosition();

    for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
    {
        for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
        {
            ray.Direction = camera.GetRayDirections()[x + y * m_FinalImage->GetWidth()];

            glm::vec4 color = TraceRay(ray);

            color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
            m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color);
        }
    }

    m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::TraceRay(const Ray& ray)
{
    constexpr float radius = 0.5f;

    const float a = glm::dot(ray.Direction, ray.Direction);
    const float b = 2.0f * glm::dot(ray.Origin, ray.Direction);
    const float c = glm::dot(ray.Origin, ray.Origin) - (radius * radius);

    const float discriminant = (b * b) - 4.0f * a * c;

    if (discriminant < 0.0f)
        return {0, 0, 0, 1};

    float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
    const float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);

    const glm::vec3 hitPoint = ray.Origin + ray.Direction * closestT;
    const glm::vec3 normal = glm::normalize(hitPoint);

    const glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));

    float d = glm::max(glm::dot(normal, -lightDir), 0.0f); // == cos(angle)

    glm::vec3 sphereColor(0, 1, 1);
    sphereColor *= d;

    return { sphereColor, 1.0f };
}
