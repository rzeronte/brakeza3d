//
// Created by edu on 8/12/23.
//

#ifndef BRAKEZA3D_OPENGLSHADERTYPES_H
#define BRAKEZA3D_OPENGLSHADERTYPES_H


#include <vec3.hpp>
#include <SDL2/SDL_opengl.h>

struct Material {
    GLint diffuse;
    GLint specular;
    float shininess;
};

struct DirLight {
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct PointLight {
    glm::vec4 position;
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
    float constant;
    float linear;
    float quadratic;
    float padding;
};

struct SpotLight {
    glm::vec3 position;
    glm::vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

#endif //BRAKEZA3D_OPENGLSHADERTYPES_H
