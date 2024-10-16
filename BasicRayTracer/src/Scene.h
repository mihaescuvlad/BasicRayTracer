#ifndef SCENE_H
#define SCENE_H

#include <vector>

#include <glm/vec3.hpp>

#include "shape/IShape.h"
#include "shape/ui/IShapeUIController.h"

struct Material
{
    glm::vec3 Albedo{ 1.0f };
    float Roughness = 1.0f;
    float Metallic = 0.0f;
    glm::vec3 EmissionColor{ 0.0f };
    float EmissionPower = 0.0f;

    glm::vec3 GetEmission() const { return EmissionColor * EmissionPower; }
};

struct Scene
{
    std::vector<std::unique_ptr<IShape>> Shapes;
    std::vector<std::unique_ptr<IShapeUIController>> UIControllers;

    std::vector<Material> Materials;
    glm::vec3 SkyColor;

    void AddShape(std::unique_ptr<IShape> shape, std::unique_ptr<IShapeUIController> uiController)
    {
        Shapes.push_back(std::move(shape));
        UIControllers.push_back(std::move(uiController));
    }
};

#endif