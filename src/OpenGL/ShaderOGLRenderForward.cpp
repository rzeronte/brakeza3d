#define GL_GLEXT_PROTOTYPES

#include <glm/gtc/type_ptr.hpp>
#include "../../include/OpenGL/ShaderOGLRenderForward.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"

ShaderOGLRenderForward::ShaderOGLRenderForward()
:
    ShaderBaseOpenGL(
        EngineSetup::get()->SHADERS_FOLDER + "Render.vs",
        EngineSetup::get()->SHADERS_FOLDER + "Render.fs",
        false
    )
{
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    matrixProjectionUniform = glGetUniformLocation(programID, "projection");
    matrixViewUniform = glGetUniformLocation(programID, "view");
    matrixModelUniform = glGetUniformLocation(programID, "model");

    viewPositionUniform = glGetUniformLocation(programID, "viewPos");

    numLightPointsUniform = glGetUniformLocation(programID, "numLights");
    numSpotLightsUniform = glGetUniformLocation(programID, "numSpotLights");

    directionalLightDirectionUniform = glGetUniformLocation(programID, "dirLight.direction");
    directionalLightAmbientUniform = glGetUniformLocation(programID, "dirLight.ambient");
    directionalLightDiffuseUniform = glGetUniformLocation(programID, "dirLight.diffuse");
    directionalLightSpecularUniform = glGetUniformLocation(programID, "dirLight.specular");

    materialTextureDiffuseUniform = glGetUniformLocation(programID, "material.diffuse");
    materialTextureSpecularUniform = glGetUniformLocation(programID, "material.specular");
    materialShininessUniform = glGetUniformLocation(programID, "material.shininess");

    alphaUniform = glGetUniformLocation(programID, "alpha");

    initializeLightBuffers();
}

void ShaderOGLRenderForward::render(
    Object3D *o,
    GLint textureID,
    GLint textureSpecularID,
    GLuint vertexbuffer,
    GLuint uvbuffer,
    GLuint normalbuffer,
    int size,
    float alpha,
    GLuint framebuffer
) const
{
    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(framebuffer);

    ComponentsManager::get()->getComponentRender()->changeOpenGLProgram(programID);
    glBindVertexArray(VertexArrayID);

    auto camera = ComponentsManager::get()->getComponentCamera();
    auto cameraPosition = camera->getCamera()->getPosition().toGLM();

    setMat4Uniform(matrixProjectionUniform, camera->getGLMMat4ProjectionMatrix());
    setMat4Uniform(matrixViewUniform, camera->getGLMMat4ViewMatrix());
    setMat4Uniform(matrixModelUniform, o->getModelMatrix());

    setVec3Uniform(viewPositionUniform, cameraPosition);

    // textures
    setIntUniform(materialTextureDiffuseUniform, 0);
    setIntUniform(materialTextureSpecularUniform, 1);
    setFloatUniform(materialShininessUniform, 32.0f);

    setVec3Uniform(directionalLightDirectionUniform, directionalLight.direction);
    setVec3Uniform(directionalLightAmbientUniform, directionalLight.ambient);
    setVec3Uniform(directionalLightDiffuseUniform, directionalLight.diffuse);
    setVec3Uniform(directionalLightSpecularUniform, directionalLight.specular);

    setIntUniform(numLightPointsUniform, static_cast<int>(pointsLights.size()));
    setIntUniform(numSpotLightsUniform, static_cast<int>(spotLights.size()));
    setFloatUniform(alphaUniform, alpha);

    setVec3("drawOffset", o->getDrawOffset().toGLM());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureSpecularID);

    setVAOAttributes(vertexbuffer, uvbuffer, normalbuffer);
    glUniformBlockBinding(programID, glGetUniformBlockIndex(programID, "PointLightsBlock"), 0);
    glUniformBlockBinding(programID, glGetUniformBlockIndex(programID, "SpotLightsBlock"), 1);

    glDrawArrays(GL_TRIANGLES, 0, size);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(0);
}

glm::mat4 ShaderOGLRenderForward::getDirectionalLightMatrix(const DirLightOpenGL& light)
{
    const float size = EngineSetup::get()->SHADOW_MAPPING_FRUSTUM_SIZE;

    glm::mat4 lightProjection = glm::ortho(
        -size,
        size,
        -size,
        size,
        EngineSetup::get()->SHADOW_MAPPING_DEPTH_FRUSTUM_NEAR_PLANE,
        EngineSetup::get()->SHADOW_MAPPING_DEPTH_FRUSTUM_FAR_PLANE
    );

    // Normalizar la dirección de la luz
    glm::vec3 forward = glm::normalize(light.direction);

    // Para una luz direccional, usamos una posición arbitraria en la dirección opuesta a donde apunta la luz
    glm::vec3 p = -forward * EngineSetup::get()->SHADOW_MAPPING_DEPTH_FRUSTUM_FAR_PLANE * 0.5f;

    glm::mat4 lightView = glm::lookAt(
        p,
        p + forward,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    return lightProjection * lightView;
}

void ShaderOGLRenderForward::setVAOAttributes(GLuint vertexbuffer, GLuint uvbuffer, GLuint normalbuffer)
{
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
}

void ShaderOGLRenderForward::createUBOFromLights()
{
    pointsLights.resize(0);
    spotLights.resize(0);
    shadowMappingLights.resize(0);

    if (EngineSetup::get()->ENABLE_LIGHTS) {
        for (auto o : Brakeza3D::get()->getSceneObjects()) {
            if (!o->isEnabled()) continue;
            extractLights(o);
            for (auto a: o->getAttached()) {
                if (!a->isEnabled()) continue;
                extractLights(a);
            }
        }
    }

    fillUBOLights();
}

DirLightOpenGL &ShaderOGLRenderForward::getDirectionalLight()
{
    return directionalLight;
}

void ShaderOGLRenderForward::destroy()
{
}

void ShaderOGLRenderForward::renderMesh(Mesh3D *o, GLuint framebuffer) const
{
    for (const auto& m: o->meshes) {
        render(
            o,
            o->getModelTextures()[m.materialIndex]->getOGLTextureID(),
            o->getModelTextures()[m.materialIndex]->getOGLTextureID(),
            m.vertexbuffer,
            m.uvbuffer,
            m.normalbuffer,
            static_cast<int>(m.vertices.size()),
            o->getAlpha(),
            framebuffer
        );
    }
}

void ShaderOGLRenderForward::fillUBOLights()
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
    Vertex3D forward = o->getRotation().Z().getNormalize();

    auto s = dynamic_cast<SpotLight3D*>(o);
    if (s != nullptr) {
        shadowMappingLights.push_back(s);
        spotLights.push_back(SpotLightOpenGL{
            glm::vec4(o->getPosition().toGLM(), 1),
            glm::vec4(forward.toGLM(), 0),
            s->ambient,
            s->diffuse,
            s->specular,
            s->constant,
            s->linear,
            s->quadratic,
            s->cutOff,
            s->outerCutOff
        });
        return;
    }

    auto l = dynamic_cast<LightPoint3D*>(o);
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

void ShaderOGLRenderForward::renderAnimatedMesh(Mesh3D *o, GLuint framebuffer) const
{
    for (const auto& m: o->meshes) {
        render(
            o,
            o->getModelTextures()[m.materialIndex]->getOGLTextureID(),
            o->getModelTextures()[m.materialIndex]->getOGLTextureID(),
            m.feedbackBuffer,
            m.uvbuffer,
            m.normalbuffer,
            static_cast<int>(m.vertices.size()),
            o->getAlpha(),
            framebuffer
        );
    }
}

int ShaderOGLRenderForward::getNumPointLights() const {
    return static_cast<int>(pointsLights.size());
}

int ShaderOGLRenderForward::getNumSpotLights() const {
    return static_cast<int>(spotLights.size());
}

[[nodiscard]] std::vector<SpotLight3D *> &ShaderOGLRenderForward::getShadowMappingSpotLights() {
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
    if (getNumPointLights() != lastPointLightsSize) {
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

    // Buffer de Point Lights
    glGenBuffers(1, &bufferUBOLightPoints);
    glBindBuffer(GL_UNIFORM_BUFFER, bufferUBOLightPoints);
    glBufferData(
        GL_UNIFORM_BUFFER,
        MAX_POINT_LIGHTS * sizeof(PointLightOpenGL),  // Tamaño MÁXIMO
        nullptr,                                       // Sin datos iniciales
        GL_DYNAMIC_DRAW                               // Datos que cambian frecuentemente
    );
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, bufferUBOLightPoints);  // Binding point 0

    // Buffer de Spot Lights
    glGenBuffers(1, &bufferUBOSpotLights);
    glBindBuffer(GL_UNIFORM_BUFFER, bufferUBOSpotLights);
    glBufferData(
        GL_UNIFORM_BUFFER,
        MAX_SPOT_LIGHTS * sizeof(SpotLightOpenGL),    // Tamaño MÁXIMO
        nullptr,                                       // Sin datos iniciales
        GL_DYNAMIC_DRAW
    );
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, bufferUBOSpotLights);   // Binding point 1

    Logging::Message("Light buffers initialized!");
    buffersInitialized = true;
}