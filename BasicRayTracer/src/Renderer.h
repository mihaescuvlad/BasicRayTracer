#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include "Camera.h"
#include "Ray.h"
#include "Scene.h"
#include "Walnut/Image.h"

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
}

class Renderer
{
public:
    Renderer() = default;

    void OnResize(uint32_t width, uint32_t height);
    void Render(const Scene& scene, const Camera& camera);

    std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }
private:
    struct HitPayload
    {
        float HitDistance;
        glm::vec3 WorldPosition;
        glm::vec3 WorldNormal;

        uint32_t ObjectIndex;
    };

    glm::vec4 PerPixel(uint32_t x, uint32_t y); // Ray Gen

    HitPayload TraceRay(const Ray& ray) const;
    HitPayload ClosestHit(const Ray& ray, float hitDistance, uint32_t objectIndex) const;
    static HitPayload Miss(const Ray& ray);
private:
    std::shared_ptr<Walnut::Image> m_FinalImage;

    const Scene* m_ActiveScene = nullptr;
    const Camera* m_ActiveCamera = nullptr;

    uint32_t* m_ImageData = nullptr;
};

#endif