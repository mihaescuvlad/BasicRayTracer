#include "Camera.h"
#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include "Walnut/Timer.h"

#include "Renderer.h"

#include <ranges>
#include <glm/gtc/type_ptr.hpp>

using namespace Walnut;

class ExampleLayer final : public Walnut::Layer
{
public:
    ExampleLayer()
        : m_Camera{ 45.0f, 0.1f, 100.0f }
    {
        m_Scene.SkyColor = glm::vec3(0.6f, 0.7f, 0.9f);

        Material& blueSphere =  m_Scene.Materials.emplace_back();
        blueSphere.Albedo = { 0.0f, 1.0f, 1.0f };
        blueSphere.Roughness = 0.0f;

        Material& ground = m_Scene.Materials.emplace_back();
        ground.Albedo = { 0.5f, 0.3f, 0.2f };
        ground.Roughness = 0.1f;

        Material& lightBulb = m_Scene.Materials.emplace_back();
        lightBulb.Albedo = { 0.8f, 0.5f, 0.2f };
        lightBulb.Roughness = 0.1f;
        lightBulb.EmissionColor = lightBulb.Albedo;
        lightBulb.EmissionPower = 2.0f;

        {
            Sphere sphere;
            sphere.Position = { 0.0f, 0.0f, 0.0f };
            sphere.Radius = 1.0f;
            sphere.MaterialIndex = 0;

            m_Scene.Spheres.push_back(sphere);
        }

        {
            Sphere sphere;
            sphere.Position = { 0.0f, -101.0f, 0.0f };
            sphere.Radius = 100.0f;
            sphere.MaterialIndex = 1;

            m_Scene.Spheres.push_back(sphere);
        }

        {
            Sphere sphere;
            sphere.Position = { 2.0f, 0.0f, 0.0f };
            sphere.Radius = 1.0f;
            sphere.MaterialIndex = 2;

            m_Scene.Spheres.push_back(sphere);
        }
    }

    virtual void OnUpdate(const float ts) override
    {
        if (m_Camera.OnUpdate(ts))
            m_Renderer.ResetFrameIndex();
    }

    virtual void OnUIRender() override
    {
        ImGui::Begin("Settings");
        ImGui::Text("Last render: %.3fms", static_cast<double>(m_LastRenderTime));
        if (ImGui::Button("Render"))
            Render();

        ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accumulate);

        if (ImGui::Button("Reset"))
            m_Renderer.ResetFrameIndex();
        
        ImGui::End();

        ImGui::Begin("Scene");

        ImGui::ColorEdit3("Sky Color", glm::value_ptr(m_Scene.SkyColor));

        for (auto&& [i, sphere] : m_Scene.Spheres | std::views::enumerate)
        {
            ImGui::PushID(static_cast<int>(i));

            ImGui::DragFloat3("Position", glm::value_ptr(sphere.Position), 0.1f);
            ImGui::DragFloat("Radius", &sphere.Radius, 0.1f);
            ImGui::DragInt("Material", &sphere.MaterialIndex, 1.0f, 0, static_cast<int>(m_Scene.Materials.size()) - 1);

            ImGui::Separator();

            ImGui::PopID();
        }
        ImGui::End();

        ImGui::Begin("Materials");
        for (auto&& [i, material] : m_Scene.Materials | std::views::enumerate)
        {
            ImGui::PushID(static_cast<int>(i));

            ImGui::ColorEdit3("Albedo", glm::value_ptr(material.Albedo));
            ImGui::DragFloat("Roughness", &material.Roughness, 0.05f, 0.0f, 1.0f);
            ImGui::DragFloat("Metallic", &material.Metallic, 0.05f, 0.0f, 1.0f);
            ImGui::ColorEdit3("Emission Color", glm::value_ptr(material.EmissionColor));
            ImGui::DragFloat("Emission Power", &material.EmissionPower, 0.05f, 0.0f, std::numeric_limits<float>::max());

            ImGui::Separator();

            ImGui::PopID();
        }
        ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("Viewport");

        m_ViewportWidth = static_cast<uint32_t>(ImGui::GetContentRegionAvail().x);
        m_ViewportHeight = static_cast<uint32_t>(ImGui::GetContentRegionAvail().y);

        const auto image = m_Renderer.GetFinalImage();
        if (image)
            ImGui::Image(image->GetDescriptorSet(), { static_cast<float>(image->GetWidth()), static_cast<float>(image->GetHeight()) },
                ImVec2(0, 1), ImVec2(1, 0));

        ImGui::End();
        ImGui::PopStyleVar();

        Render();
    }

    void Render()
    {
        Timer timer;

        m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
        m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
        m_Renderer.Render(m_Scene, m_Camera);

        m_LastRenderTime = timer.ElapsedMillis();
    }
private:
    Camera m_Camera;
    Renderer m_Renderer;
    Scene m_Scene;
    uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

    float m_LastRenderTime = 0.0f;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
    Walnut::ApplicationSpecification spec;
    spec.Name = "Basic Ray Tracer";

    auto app = new Walnut::Application(spec);
    app->PushLayer<ExampleLayer>();
    app->SetMenubarCallback([app]()
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Exit"))
                {
                    app->Close();
                }
                ImGui::EndMenu();
            }
        });
    return app;
}