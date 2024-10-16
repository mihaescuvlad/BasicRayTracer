#ifndef ISHAPE_H
#define ISHAPE_H

#include <string>
#include <glm/glm.hpp>

#include "..\Ray.h"

struct HitPayload
{
    float HitDistance;
    glm::vec3 WorldPosition;
    glm::vec3 WorldNormal;

    uint32_t MaterialIndex;
};

class IShape {
public:
    virtual ~IShape() = default;

    virtual float Trace(const Ray& ray) const = 0;
    virtual HitPayload ClosestHit(const Ray& ray, float hitDistance) const = 0;

    virtual int& GetMaterialIndex() = 0;
    virtual glm::vec3& GetPosition() = 0;
};

#endif
