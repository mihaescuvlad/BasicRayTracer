#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include "Camera.h"
#include "Ray.h"
#include "Scene.h"
#include "shape/IShape.h"
#include "Walnut/Image.h"

class Renderer
{
public:
    struct Settings
    {
        bool Accumulate = true;
    };
public:
    Renderer() = default;

    void OnResize(uint32_t width, uint32_t height);
    void Render(const Scene& scene, const Camera& camera);

    std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }

    void ResetFrameIndex() { m_FrameIndex = 1; }
    Settings& GetSettings() { return m_Settings; }
private:
    glm::vec4 PerPixel(uint32_t x, uint32_t y) const; // Ray Gen

    HitPayload TraceRay(const Ray& ray) const;
    //HitPayload ClosestHit(const Ray& ray, float hitDistance, uint32_t objectIndex) const;
    static HitPayload Miss(const Ray& ray);
private:
    std::shared_ptr<Walnut::Image> m_FinalImage;
    Settings m_Settings;

    const Scene* m_ActiveScene = nullptr;
    const Camera* m_ActiveCamera = nullptr;

    uint32_t* m_ImageData = nullptr;
    glm::vec4* m_AccumulationData = nullptr;

    uint32_t m_FrameIndex = 1;
};

#endif