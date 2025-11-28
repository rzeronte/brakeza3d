#include "../../include/3D/Cone3D.h"

#include <cmath>
#include <glm/geometric.hpp>
#include <glm/detail/func_trigonometric.inl>

Cone3D::Cone3D(float altura, float angulo_grados, int segmentos)
{
    UpdateVertices(altura, angulo_grados, segmentos);
}

void Cone3D::UpdateVertices(float altura, float angulo_grados, int segmentos)
{
    // Limpiar vectores anteriores
    vertices.clear();
    normals.clear();
    uvs.clear();

    // Parámetros del cono
    float baseRadius = altura * std::tan(glm::radians(angulo_grados / 2.0f));

    const float PI = std::acos(-1.0f);
    float sectorStep = 2.0f * PI / segmentos;

    // Apex alejado, base en origen
    glm::vec4 apex(0.0f, 0.0f, 0.0f, 1.0f);
    float z_base = altura;

    // Calcular ángulo de inclinación para normales
    float slopeAngle = std::atan2(baseRadius, altura);

    // ========== PARÁMETRO DE REPETICIÓN UV ==========
    float uvRepeatU = 4.0f;  // Repeticiones horizontales (alrededor del cono)
    float uvRepeatV = 4.0f;  // Repeticiones verticales (desde apex hasta base)

    // ========== SUPERFICIE LATERAL ==========
    for(int i = 0; i < segmentos; ++i) {
        float angle1 = i * sectorStep;
        float angle2 = (i + 1) * sectorStep;

        // Puntos en la base
        glm::vec4 base1(std::cos(angle1) * baseRadius, std::sin(angle1) * baseRadius, z_base, 1.0f);
        glm::vec4 base2(std::cos(angle2) * baseRadius, std::sin(angle2) * baseRadius, z_base, 1.0f);

        // Normales en cada vértice
        glm::vec3 n_apex(0.0f, 0.0f, std::sin(slopeAngle));
        glm::vec3 n1(std::cos(angle1) * std::cos(slopeAngle),
                     std::sin(angle1) * std::cos(slopeAngle),
                     std::sin(slopeAngle));
        glm::vec3 n2(std::cos(angle2) * std::cos(slopeAngle),
                     std::sin(angle2) * std::cos(slopeAngle),
                     std::sin(slopeAngle));

        n_apex = glm::normalize(n_apex);
        n1 = glm::normalize(n1);
        n2 = glm::normalize(n2);

        // UVs con repetición
        float u1 = ((float)i / segmentos) * uvRepeatU;
        float u2 = ((float)(i + 1) / segmentos) * uvRepeatU;

        // Triángulo lateral: apex -> base2 -> base1
        vertices.push_back(apex);
        normals.push_back(n_apex);
        uvs.push_back(glm::vec2((u1 + u2) * 0.5f, 0.0f));

        vertices.push_back(base2);
        normals.push_back(n2);
        uvs.push_back(glm::vec2(u2, 1.0f * uvRepeatV));

        vertices.push_back(base1);
        normals.push_back(n1);
        uvs.push_back(glm::vec2(u1, 1.0f * uvRepeatV));
    }
}