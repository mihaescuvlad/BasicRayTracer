#ifndef SCENE_H
#define SCENE_H

#include <vector>

#include <glm/vec3.hpp>

struct Sphere
{
    glm::vec3 Position{ 0.0f };
    float Radius = 0.5f;

    glm::vec3 Albedo{ 1.0f };
};

struct Scene
{
    std::vector<Sphere> Spheres;
};

#endif