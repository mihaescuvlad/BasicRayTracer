#ifndef PLANE_H
#define PLANE_H

#include "IShape.h"

class Plane : public IShape {
public:
    Plane(const glm::vec3& normal, float distance, int materialIndex);

    float Trace(const Ray& ray) const override;
    HitPayload ClosestHit(const Ray& ray, float hitDistance) const override;
    int& GetMaterialIndex() override { return m_MaterialIndex; }
    glm::vec3& GetPosition() override { return m_Normal; }

    glm::vec3& GetNormal() { return m_Normal; }
    float& GetDistance() { return m_Distance; }
private:
    glm::vec3 m_Normal;
    float m_Distance;
    int m_MaterialIndex;
};

#endif