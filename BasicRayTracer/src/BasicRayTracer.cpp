#include "Camera.h"
#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include "Walnut/Timer.h"

#include "Renderer.h"

#include <ranges>
#include <glm/gtc/type_ptr.hpp>

#include "shape/Plane.h"
#include "shape/Sphere.h"
#include "shape/ui/PlaneUIController.h"
#include "shape/ui/SphereUIController.h"

using namespace Walnut;

class ExampleLayer final : public Walnut::Layer
{
public:
    ExampleLayer()
        : m_Camera{ 45.0f, 0.1f, 100.0f }
    {
        m_Scene.SkyColor = glm::vec3(0.6f, 0.7f, 0.9f);

        Material& blueSphereMat =  m_Scene.Materials.emplace_back();
        blueSphereMat.Albedo = { 1.0f, 1.0f, 1.0f };
        blueSphereMat.Roughness = 0.7f;

        Material& groundMat = m_Scene.Materials.emplace_back();
        groundMat.Albedo = { 0.5f, 0.3f, 0.2f };
        groundMat.Roughness = 1.0f;

        Material& lightBulbMat = m_Scene.Materials.emplace_back();
        lightBulbMat.Albedo = { 0.8f, 0.5f, 0.2f };
        lightBulbMat.Roughness = 0.1f;
        lightBulbMat.EmissionColor = lightBulbMat.Albedo;
        lightBulbMat.EmissionPower = 2.0f;

        Material& lightBulbMat2 = m_Scene.Materials.emplace_back();
        lightBulbMat2.Albedo = { 0.0f, 1.0f, 1.0f };
        lightBulbMat2.Roughness = 0.1f;
        lightBulbMat2.EmissionColor = lightBulbMat2.Albedo;
        lightBulbMat2.EmissionPower = 2.0f;

        auto blueSphere = std::make_unique<Sphere>(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, 0);
        auto blueSphereUi = std::make_unique<SphereUIController>(*blueSphere);
        m_Scene.AddShape(std::move(blueSphere), std::move(blueSphereUi));

        auto ground = std::make_unique<Plane>(glm::vec3(0.0f, 1.0f, 0.0f), -1.0f, 1);
        auto groundUi = std::make_unique<PlaneUIController>(*ground);
        m_Scene.AddShape(std::move(ground), std::move(groundUi));

        auto lightBulb = std::make_unique<Sphere>(glm::vec3(2.0f, 0.0f, 0.0f), 1.0f, 2);
        auto lightBulbUi = std::make_unique<SphereUIController>(*lightBulb);
        m_Scene.AddShape(std::move(lightBulb), std::move(lightBulbUi));

        auto lightBulb2 = std::make_unique<Sphere>(glm::vec3(-2.0f, 0.0f, 0.0f), 1.0f, 3);
        auto lightBulbUi2 = std::make_unique<SphereUIController>(*lightBulb2);
        m_Scene.AddShape(std::move(lightBulb2), std::move(lightBulbUi2));
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
        ImGui::Separator();

        for (auto&& [i, sphere] : m_Scene.Shapes | std::views::enumerate)
        {
            ImGui::PushID(static_cast<int>(i));

            m_Scene.UIControllers[i]->RenderUI();

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