//
// Created by edu on 8/12/23.
//

#ifndef BRAKEZA3D_OPENGLSHADERTYPES_H
#define BRAKEZA3D_OPENGLSHADERTYPES_H


#include <vec3.hpp>

struct Material {
    GLint diffuse;
    GLint specular;
    float shininess;
};

struct DirLightOpenGL {
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct PointLightOpenGL {
    glm::vec4 position;
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
    float constant;
    float linear;
    float quadratic;
    float padding;
};

struct SpotLightOpenGL {
    glm::vec4 position;
    glm::vec4 direction;
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
    float constant;
    float linear;
    float quadratic;
    float cutOff;
    float outerCutOff;
    float padding;
    float padding1;
    float padding2;
};

#endif //BRAKEZA3D_OPENGLSHADERTYPES_H
