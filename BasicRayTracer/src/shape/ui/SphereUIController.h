#ifndef SPHEREUICONTROLLER_H
#define SPHEREUICONTROLLER_H

#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "..\Sphere.h"
#include "IShapeUIController.h"

struct SphereUIController : public IShapeUIController
{
    Sphere& m_Sphere;

    SphereUIController(Sphere& sphere) : m_Sphere(sphere) {}

    void RenderUI() override
    {
        ImGui::Text("Sphere");
        ImGui::DragFloat3("Position", glm::value_ptr(m_Sphere.GetPosition()), 0.1f);
        ImGui::DragFloat("Radius", &m_Sphere.GetRadius(), 0.1f);
        ImGui::DragInt("Material", &m_Sphere.GetMaterialIndex(), 1.0f, 0, 10);
    }
};

#endif