/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
//
// Created by Eduardo on 27/11/2025.
//

#ifndef BRAKEZA3D_CONE3D_H
#define BRAKEZA3D_CONE3D_H
#include <vector>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

class Cone3D
{
public:
    std::vector<glm::vec4> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;

    Cone3D() = default;
    Cone3D(float altura, float angulo_grados, int segmentos);
    void UpdateVertices(float altura, float angulo_grados, int segmentos);
};


#endif //BRAKEZA3D_CONE3D_H