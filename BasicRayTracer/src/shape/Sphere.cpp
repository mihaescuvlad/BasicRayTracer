#include "Sphere.h"

Sphere::Sphere(const glm::vec3& position, float radius, int materialIndex)
    : m_Position{ position }, m_Radius{ radius }, m_MaterialIndex{ materialIndex } {}

float Sphere::Trace(const Ray& ray) const
{
    const glm::vec3 origin = ray.Origin - m_Position;

    const float a = glm::dot(ray.Direction, ray.Direction);
    const float b = 2.0f * glm::dot(origin, ray.Direction);
    const float c = glm::dot(origin, origin) - m_Radius * m_Radius;

    const float discriminant = (b * b) - 4.0f * a * c;
    if (discriminant < 0.0f) return -1.0f;

    // float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
    return (-b - glm::sqrt(discriminant)) / (2.0f * a);
}

HitPayload Sphere::ClosestHit(const Ray& ray, float hitDistance) const
{
    HitPayload payload{};
    payload.HitDistance = hitDistance;
    payload.MaterialIndex = m_MaterialIndex;

    const glm::vec3 origin = ray.Origin - m_Position;
    payload.WorldPosition = origin + ray.Direction * hitDistance;
    payload.WorldNormal = glm::normalize(payload.WorldPosition);
    payload.WorldPosition += m_Position;

    return payload;
}