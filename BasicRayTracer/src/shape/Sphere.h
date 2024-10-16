#ifndef SPHERE_H
#define SPHERE_H

#include "IShape.h"

class Sphere : public IShape
{
public:
    Sphere(const glm::vec3& position, float radius, int materialIndex);

    float Trace(const Ray& ray) const override;
    HitPayload ClosestHit(const Ray& ray, float hitDistance) const override;

    int& GetMaterialIndex() override { return m_MaterialIndex; }
    glm::vec3& GetPosition() override { return m_Position; }

    float& GetRadius() { return m_Radius; }
private:
    glm::vec3 m_Position;
    float m_Radius;
    int m_MaterialIndex;
};

#endif