
#include <glm/trigonometric.hpp>
#include "../../include/3D/LightSpot.h"
#include "../../include/3D/Cone3D.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.inl>
#include "../../include/Brakeza.h"
#include "../../include/GUI/Objects/LightSpotGUI.h"
#include "../../include/Components/Components.h"

LightSpot::LightSpot(
    const glm::vec4 &ambient,
    const glm::vec4 &diffuse,
    const glm::vec4 &specular,
    float constant,
    float linear,
    float quadratic,
    float cutOff,
    float outerCutOff
)
:
    LightPoint(ambient, diffuse, specular, constant, linear, quadratic),
    cutOff(cutOff),
    outerCutOff(outerCutOff)
{
}

void LightSpot::onUpdate()
{
    LightPoint::onUpdate();

    // VALIDACIÓN IMPORTANTE
    assert(cutOff > outerCutOff && "cutOff debe ser mayor que outerCutOff (coseno)");
    assert(cutOff >= 0.0f && cutOff <= 1.0f);
    assert(outerCutOff >= 0.0f && outerCutOff <= 1.0f);

    RenderDebugCone(std::acos(cutOff), Color::red());
}

void LightSpot::RenderDebugCone(float radians, const Color &c)
{
    float angulo_outer_grados = radians * 180.0f / static_cast<float>(M_PI);
    float angulo_cono = angulo_outer_grados * 2.0f; // Ángulo total del cono

    auto render = Components::get()->Render();

    if (showDebugCone && isGUISelected()) {
        cone.UpdateVertices(
            Config::get()->SHADOW_MAPPING_DEPTH_FAR_PLANE/10,
            angulo_cono,
            32
        );

        crearBuffersCono();

        if (Config::get()->TRIANGLE_MODE_TEXTURIZED) {
            glDisable(GL_CULL_FACE);
            glEnable(GL_BLEND);
            glBlendFunc(mode_src, mode_dst);
            render->getShaders()->shaderOGLColor->RenderColor(
                getModelMatrix(),
                vertexBuffer,
                uvBuffer,
                normalBuffer,
                cone.vertices.size(),
                c,
                false,
                Components::get()->Window()->getGBuffer().FBO
            );
            glEnable(GL_CULL_FACE);
        }

        if (Config::get()->TRIANGLE_MODE_WIREFRAME) {
            render->getShaders()->shaderOGLWireframe->render(
                getModelMatrix(),
                vertexBuffer,
                uvBuffer,
                normalBuffer,
                static_cast<int>(cone.vertices.size()),
                c,
                Components::get()->Window()->getSceneFramebuffer()
            );
        }
    }
}

glm::mat4 LightSpot::getLightSpaceMatrix() const
{
    float aspect = 1.0f; // Aspect ratio cuadrado para el shadow map
    float fov = std::acos(getOuterCutOff()) * 2.0f;

    glm::mat4 lightProjection = glm::perspective(
        fov,
        aspect,
        Config::get()->SHADOW_MAPPING_DEPTH_NEAR_PLANE,
        Config::get()->SHADOW_MAPPING_DEPTH_FAR_PLANE
    );

    Vertex3D forward = Vertex3D::fromGLM(getDirection());
    const auto p = position.toGLM();

    glm::mat4 lightView = glm::lookAt(
        p,
        p + forward.toGLM(),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    return lightProjection * lightView;
}

void LightSpot::crearBuffersCono()
{
    clearConeBuffers();

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(cone.vertices.size() * sizeof(glm::vec4)), &cone.vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(cone.uvs.size() * sizeof(glm::vec2)), &cone.uvs[0], GL_STATIC_DRAW);

    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(cone.normals.size() * sizeof(glm::vec3)), &cone.normals[0], GL_STATIC_DRAW);
}

void LightSpot::clearConeBuffers()
{
    if (vertexBuffer != 0) {
        glDeleteBuffers(1, &vertexBuffer);
        vertexBuffer = 0;
    }
    if (normalBuffer != 0) {
        glDeleteBuffers(1, &normalBuffer);
        normalBuffer = 0;
    }
    if (uvBuffer != 0) {
        glDeleteBuffers(1, &uvBuffer);
        uvBuffer = 0;
    }
}

void LightSpot::setCutOff(float value)
{
    cutOff = value;
}

void LightSpot::setOuterCutOff(float value)
{
    outerCutOff = value;
}

ObjectType LightSpot::getTypeObject() const
{
    return ObjectType::LightSpot;
}

float LightSpot::getCutOff() const
{
    return cutOff;
}

float LightSpot::getOuterCutOff() const
{
    return outerCutOff;
}

glm::vec4 LightSpot::getDirection() const
{
    return ( getRotation().getTranspose() * Vertex3D(0, 0, 1)).toGLM4();
}

GUIType::Sheet LightSpot::getIcon()
{
    return IconObject::LIGHT_SPOT;
}

void LightSpot::DrawPropertiesGUI()
{
    LightPoint::DrawPropertiesGUI();
    LightSpotGUI::DrawPropertiesGUI(this);
}
