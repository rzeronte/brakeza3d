#define GL_GLEXT_PROTOTYPES

#include <glm/gtc/type_ptr.hpp>
#include "../../include/OpenGL/ShaderOGLRenderForward.h"
#include "../../include/Components/Components.h"
#include "../../include/Brakeza.h"

ShaderOGLRenderForward::ShaderOGLRenderForward()
:
    ShaderBaseOpenGL(
        Config::get()->SHADERS_FOLDER + "Render.vs",
        Config::get()->SHADERS_FOLDER + "Render.fs",
        false
    )
{
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    matrixProjectionUniform = glGetUniformLocation(programID, "projection");
    matrixViewUniform = glGetUniformLocation(programID, "view");
    matrixModelUniform = glGetUniformLocation(programID, "model");

    materialTextureDiffuseUniform = glGetUniformLocation(programID, "material.diffuse");
    materialTextureSpecularUniform = glGetUniformLocation(programID, "material.specular");
    materialShininessUniform = glGetUniformLocation(programID, "material.shininess");

    alphaUniform = glGetUniformLocation(programID, "alpha");

    initializeLightBuffers();
}

void ShaderOGLRenderForward::render(
    Object3D *o,
    GLuint textureID,
    GLuint textureSpecularID,
    GLuint vertexbuffer,
    GLuint uvbuffer,
    GLuint normalbuffer,
    int size,
    float alpha,
    GLuint fbo
) const
{
    Components::get()->Render()->changeOpenGLFramebuffer(fbo);

    Components::get()->Render()->changeOpenGLProgram(programID);
    glBindVertexArray(VertexArrayID);

    auto camera = Components::get()->Camera();

    setFloatUniform(alphaUniform, alpha);

    setMat4Uniform(matrixProjectionUniform, camera->getGLMMat4ProjectionMatrix());
    setMat4Uniform(matrixViewUniform, camera->getGLMMat4ViewMatrix());
    setMat4Uniform(matrixModelUniform, o->getModelMatrix());

    // textures
    setIntUniform(materialTextureDiffuseUniform, 0);
    setIntUniform(materialTextureSpecularUniform, 1);
    setFloatUniform(materialShininessUniform, 32.0f);

    setVec3("drawOffset", o->getDrawOffset().toGLM());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureSpecularID);

    setVAOAttributes(vertexbuffer, uvbuffer, normalbuffer);

    glDrawArrays(GL_TRIANGLES, 0, size);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    Components::get()->Render()->changeOpenGLFramebuffer(0);
}

glm::mat4 ShaderOGLRenderForward::getDirectionalLightMatrix(const DirLightOpenGL& light)
{
    const float size = Config::get()->SHADOW_MAPPING_FRUSTUM_SIZE;

    glm::mat4 lightProjection = glm::ortho(
        -size,
        size,
        -size,
        size,
        Config::get()->SHADOW_MAPPING_DEPTH_NEAR_PLANE,
        Config::get()->SHADOW_MAPPING_DEPTH_FAR_PLANE
    );

    // Normalizar la dirección de la luz
    glm::vec3 forward = glm::normalize(light.direction);

    // Para una luz direccional, usamos una posición arbitraria en la dirección opuesta a donde apunta la luz
    glm::vec3 p = -forward * Config::get()->SHADOW_MAPPING_DEPTH_FAR_PLANE * 0.5f;

    glm::mat4 lightView = glm::lookAt(
        p,
        p + forward,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    return lightProjection * lightView;
}

void ShaderOGLRenderForward::createUBOFromLights()
{
    pointsLights.resize(0);
    spotLights.resize(0);
    shadowMappingLights.resize(0);

    if (Config::get()->ENABLE_LIGHTS) {
        for (auto o : Brakeza::get()->getSceneObjects()) {
            if (!o->isEnabled()) continue;
            extractLights(o);
            for (auto a: o->getAttached()) {
                if (!a->isEnabled()) continue;
                extractLights(a);
            }
        }
    }

    FillUBOLights();
}

DirLightOpenGL &ShaderOGLRenderForward::getDirectionalLight()
{
    return directionalLight;
}

void ShaderOGLRenderForward::destroy()
{
}

void ShaderOGLRenderForward::renderMesh(Mesh3D *o, bool useFeedbackBuffer, GLuint fbo) const
{
    for (const auto& m: o->getMeshData()) {
        render(
            o,
            o->getModelTextures()[m.materialIndex]->getOGLTextureID(),
            o->getModelTextures()[m.materialIndex]->getOGLTextureID(),
            useFeedbackBuffer ? m.feedbackBuffer : m.vertexBuffer,
            m.uvBuffer,
            m.normalBuffer,
            static_cast<int>(m.vertices.size()),
            o->getAlpha(),
            fbo
        );
    }
}

void ShaderOGLRenderForward::FillUBOLights()
{
    if (!buffersInitialized) {
        initializeLightBuffers();
    }

    size_t numPointLights = std::min(pointsLights.size(), MAX_POINT_LIGHTS);
    if (numPointLights > 0) {
        glBindBuffer(GL_UNIFORM_BUFFER, bufferUBOLightPoints);
        glBufferSubData(GL_UNIFORM_BUFFER, 0,numPointLights * sizeof(PointLightOpenGL), pointsLights.data());
    }
    if (pointsLights.size() > MAX_POINT_LIGHTS) {
        Logging::Message("Point lights exceed max: %zu > %zu", pointsLights.size(), MAX_POINT_LIGHTS);
    }

    size_t numSpotLights = std::min(spotLights.size(), MAX_SPOT_LIGHTS);
    if (numSpotLights > 0) {
        glBindBuffer(GL_UNIFORM_BUFFER, bufferUBOSpotLights);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, numSpotLights * sizeof(SpotLightOpenGL),spotLights.data());
    }

    if (spotLights.size() > MAX_POINT_LIGHTS) {
        Logging::Message("Spot lights exceed max: %zu > %zu", spotLights.size(), MAX_POINT_LIGHTS);
    }
}

void ShaderOGLRenderForward::extractLights(Object3D *o)
{
    auto s = dynamic_cast<LightSpot*>(o);
    if (s != nullptr) {
        shadowMappingLights.push_back(s);
        spotLights.push_back(SpotLightOpenGL{
            o->getPosition().toGLM4(),
            s->getDirection(),
            s->ambient,
            s->diffuse,
            s->specular,
            s->constant,
            s->linear,
            s->quadratic,
            s->getCutOff(),
            s->getOuterCutOff()
        });
        return;
    }

    auto l = dynamic_cast<LightPoint*>(o);
    if (l != nullptr) {
        pointsLights.push_back({
            glm::vec4(o->getPosition().toGLM(), 1),
            l->ambient,
            l->diffuse,
            l->specular,
            l->constant,
            l->linear,
            l->quadratic
        });
    }
}

void ShaderOGLRenderForward::setGlobalIlluminationDirection(const Vertex3D &d)
{
    this->directionalLight.direction = d.toGLM();
}

void ShaderOGLRenderForward::setGlobalIlluminationAmbient(const Vertex3D &a)
{
    this->directionalLight.ambient = a.toGLM();
}

void ShaderOGLRenderForward::setGlobalIlluminationDiffuse(const Vertex3D &d)
{
    this->directionalLight.diffuse = d.toGLM();

}

void ShaderOGLRenderForward::setGlobalIlluminationSpecular(const Vertex3D &s)
{
    this->directionalLight.specular = s.toGLM();
}

int ShaderOGLRenderForward::getNumPointLights() const
{
    return static_cast<int>(pointsLights.size());
}

int ShaderOGLRenderForward::getNumSpotLights() const
{
    return static_cast<int>(spotLights.size());
}

[[nodiscard]] std::vector<LightSpot *> &ShaderOGLRenderForward::getShadowMappingSpotLights() {
    return shadowMappingLights;
}

bool ShaderOGLRenderForward::hasSpotLightsChanged() const
{
    if (getNumSpotLights() != lastSpotLightsSize) {
        return true;
    }

    return false;
}

bool ShaderOGLRenderForward::HasPointLightsChanged() const
{
    if (spotLights.size() != lastPointLightsSize) {
        return true;
    }

    return false;
}

void ShaderOGLRenderForward::setLastSpotLightsSize(int v)
{
    lastSpotLightsSize = v;
}

void ShaderOGLRenderForward::setLastPointLightsSize(int v)
{
    lastPointLightsSize = v;
}


void ShaderOGLRenderForward::initializeLightBuffers()
{
    if (buffersInitialized) {
        return;
    }

    // Buffer de LightPoints
    glGenBuffers(1, &bufferUBOLightPoints);
    glBindBuffer(GL_UNIFORM_BUFFER, bufferUBOLightPoints);
    glBufferData(GL_UNIFORM_BUFFER, MAX_POINT_LIGHTS * sizeof(PointLightOpenGL), nullptr,GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, bufferUBOLightPoints);  // Binding point 0

    // Buffer de LightSpots
    glGenBuffers(1, &bufferUBOSpotLights);
    glBindBuffer(GL_UNIFORM_BUFFER, bufferUBOSpotLights);
    glBufferData(GL_UNIFORM_BUFFER, MAX_SPOT_LIGHTS * sizeof(SpotLightOpenGL), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, bufferUBOSpotLights);   // Binding point 1

    Logging::Message("[ShaderOGLRenderForward] Light Buffers created");
    buffersInitialized = true;
}