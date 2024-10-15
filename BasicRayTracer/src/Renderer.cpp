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

void Renderer::Render() const
{
    for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
    {
        for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
        {
            glm::vec2 coord = {
                static_cast<float>(x) / static_cast<float>(m_FinalImage->GetWidth()),
                static_cast<float>(y) / static_cast<float>(m_FinalImage->GetHeight())
            };
            coord = coord * 2.0f - 1.0f; // -1 -> 1

            glm::vec4 color = PerPixel(coord);
            color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
            m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color);
        }
    }

    m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::PerPixel(const glm::vec2& coord)
{
    const auto r = static_cast<uint8_t>(coord.x * 255.0f);
    const auto g = static_cast<uint8_t>(coord.y * 255.0f);

    const glm::vec3 rayOrigin(0.0f, 0.0f, 1.0f);
    const glm::vec3 rayDirection(coord.x, coord.y, -1.0f);
    constexpr float radius = 0.5f;

    const float a = glm::dot(rayDirection, rayDirection);
    const float b = 2.0f * glm::dot(rayOrigin, rayDirection);
    const float c = glm::dot(rayOrigin, rayOrigin) - (radius * radius);

    const float discriminant = (b * b) - 4.0f * a * c;

    if (discriminant < 0.0f)
        return {0, 0, 0, 1};

    float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
    const float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);

    const glm::vec3 hitPoint = rayOrigin + rayDirection * closestT;
    const glm::vec3 normal = glm::normalize(hitPoint);

    const glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));

    float d = glm::max(glm::dot(normal, -lightDir), 0.0f); // == cos(angle)

    glm::vec3 sphereColor(0, 1, 1);
    sphereColor *= d;

    return { sphereColor, 1.0f };
}
