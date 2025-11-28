
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

    // ✅ INVERTIDO: Apex alejado, base en origen
    glm::vec4 apex(0.0f, 0.0f, 0.0f, 1.0f);
    float z_base = altura;

    // ========== SUPERFICIE LATERAL: TRIÁNGULOS DIRECTOS ==========
    for(int i = 0; i < segmentos; ++i) {
        float angle1 = i * sectorStep;
        float angle2 = (i + 1) * sectorStep;

        // Puntos en la base
        glm::vec4 base1(std::cos(angle1) * baseRadius, std::sin(angle1) * baseRadius, z_base, 1.0f);
        glm::vec4 base2(std::cos(angle2) * baseRadius, std::sin(angle2) * baseRadius, z_base, 1.0f);

        // Calcular normal para el triángulo (orden invertido)
        glm::vec3 edge1 = glm::vec3(base1) - glm::vec3(apex);
        glm::vec3 edge2 = glm::vec3(base2) - glm::vec3(apex);
        glm::vec3 normal = glm::normalize(glm::cross(edge2, edge1));

        // Triángulo lateral: apex -> base1 -> base2
        vertices.push_back(apex);
        normals.push_back(normal);
        uvs.push_back(glm::vec2(0.5f, 0.0f));

        vertices.push_back(base1);
        normals.push_back(normal);
        uvs.push_back(glm::vec2((float)i / segmentos, 1.0f));

        vertices.push_back(base2);
        normals.push_back(normal);
        uvs.push_back(glm::vec2((float)(i+1) / segmentos, 1.0f));
    }

    // ========== BASE CIRCULAR: RELLENO CENTRAL ==========
    glm::vec4 baseCenter(0.0f, 0.0f, z_base, 1.0f);
    glm::vec3 baseNormal(0.0f, 0.0f, 1.0f);  // ✅ Normal hacia +Z
    glm::vec2 baseUVCenter(0.5f, 0.5f);

    for(int i = 0; i < segmentos; ++i)
    {
        float angle1 = i * sectorStep;
        float angle2 = (i + 1) * sectorStep;

        float x1 = std::cos(angle1) * baseRadius;
        float y1 = std::sin(angle1) * baseRadius;
        float x2 = std::cos(angle2) * baseRadius;
        float y2 = std::sin(angle2) * baseRadius;

        float u1 = std::cos(angle1) * 0.5f + 0.5f;
        float v1 = std::sin(angle1) * 0.5f + 0.5f;
        float u2 = std::cos(angle2) * 0.5f + 0.5f;
        float v2 = std::sin(angle2) * 0.5f + 0.5f;

        // Triángulo del relleno (orden invertido)
        vertices.push_back(baseCenter);
        normals.push_back(baseNormal);
        uvs.push_back(baseUVCenter);

        vertices.push_back(glm::vec4(x2, y2, z_base, 1.0f));
        normals.push_back(baseNormal);
        uvs.push_back(glm::vec2(u2, v2));

        vertices.push_back(glm::vec4(x1, y1, z_base, 1.0f));
        normals.push_back(baseNormal);
        uvs.push_back(glm::vec2(u1, v1));
    }

    // ========== CÍRCULO PERIMETRAL DE LA BASE (PARA WIREFRAME) ==========
    for(int i = 0; i < segmentos; ++i)
    {
        float angle1 = i * sectorStep;
        float angle2 = (i + 1) * sectorStep;

        glm::vec4 p1(std::cos(angle1) * baseRadius, std::sin(angle1) * baseRadius, z_base, 1.0f);
        glm::vec4 p2(std::cos(angle2) * baseRadius, std::sin(angle2) * baseRadius, z_base, 1.0f);

        float innerRadius = baseRadius * 0.98f;
        glm::vec4 p1_inner(std::cos(angle1) * innerRadius, std::sin(angle1) * innerRadius, z_base, 1.0f);
        glm::vec4 p2_inner(std::cos(angle2) * innerRadius, std::sin(angle2) * innerRadius, z_base, 1.0f);

        // Primer triángulo del borde
        vertices.push_back(p1);
        normals.push_back(baseNormal);
        uvs.push_back(glm::vec2(0, 0));

        vertices.push_back(p2);
        normals.push_back(baseNormal);
        uvs.push_back(glm::vec2(0, 0));

        vertices.push_back(p1_inner);
        normals.push_back(baseNormal);
        uvs.push_back(glm::vec2(0, 0));

        // Segundo triángulo del borde
        vertices.push_back(p2);
        normals.push_back(baseNormal);
        uvs.push_back(glm::vec2(0, 0));

        vertices.push_back(p2_inner);
        normals.push_back(baseNormal);
        uvs.push_back(glm::vec2(0, 0));

        vertices.push_back(p1_inner);
        normals.push_back(baseNormal);
        uvs.push_back(glm::vec2(0, 0));
    }
}
