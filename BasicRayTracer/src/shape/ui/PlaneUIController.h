#ifndef PLANEUICONTROLLER_H
#define PLANEUICONTROLLER_H

#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "..\Plane.h"
#include "IShapeUIController.h"

struct PlaneUIController : public IShapeUIController
{
    Plane& m_Plane;

    PlaneUIController(Plane& plane) : m_Plane(plane) {}

    void RenderUI() override
    {
        ImGui::Text("Plane");
        ImGui::DragFloat3("Normal", glm::value_ptr(m_Plane.GetNormal()), 0.1f);
        ImGui::DragFloat("Distance", &m_Plane.GetDistance(), 0.1f);
        ImGui::DragInt("Material", &m_Plane.GetMaterialIndex(), 1.0f, 0, 10);
    }
};

#endif