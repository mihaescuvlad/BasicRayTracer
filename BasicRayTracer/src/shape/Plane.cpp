#include "Plane.h"

Plane::Plane(const glm::vec3& normal, float distance, int materialIndex)
    : m_Normal(normal), m_Distance(distance), m_MaterialIndex(materialIndex) {}

float Plane::Trace(const Ray& ray) const
{
    const float denom = glm::dot(m_Normal, ray.Direction);

    if (glm::abs(denom) > 0.0001f) {
        const float t = (m_Distance - glm::dot(m_Normal, ray.Origin)) / denom;
        if (t >= 0.0f)
            return t;
    }

    return -1.0f;
}


HitPayload Plane::ClosestHit(const Ray& ray, float hitDistance) const
{
    HitPayload payload{};
    payload.HitDistance = hitDistance;
    payload.MaterialIndex = m_MaterialIndex;

    payload.WorldPosition = ray.Origin + ray.Direction * hitDistance;
    payload.WorldNormal = m_Normal; // Normal is constant for planes

    return payload;
}
