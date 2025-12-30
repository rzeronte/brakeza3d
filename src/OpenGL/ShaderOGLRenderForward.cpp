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
}

void ShaderOGLRenderForward::LoadUniforms()
{
    matrixProjectionUniform = glGetUniformLocation(programID, "projection");
    matrixViewUniform = glGetUniformLocation(programID, "view");
    matrixModelUniform = glGetUniformLocation(programID, "model");

    materialTextureDiffuseUniform = glGetUniformLocation(programID, "material.diffuse");
    materialTextureSpecularUniform = glGetUniformLocation(programID, "material.specular");
    materialShininessUniform = glGetUniformLocation(programID, "material.shininess");

    alphaUniform = glGetUniformLocation(programID, "alpha");

    //---
    viewPositionUniform = glGetUniformLocation(programID, "viewPos");
    numLightPointsUniform = glGetUniformLocation(programID, "numLights");
    numSpotLightsUniform = glGetUniformLocation(programID, "numSpotLights");
    enableLightsUniform = glGetUniformLocation(programID, "enableLights");

    directionalLightDirectionUniform = glGetUniformLocation(programID, "dirLight.direction");
    directionalLightAmbientUniform = glGetUniformLocation(programID, "dirLight.ambient");
    directionalLightDiffuseUniform = glGetUniformLocation(programID, "dirLight.diffuse");
    directionalLightSpecularUniform = glGetUniformLocation(programID, "dirLight.specular");

    //--

    directionalLightMatrixUniform = glGetUniformLocation(programID, "dirLightMatrix");

    dirLightShadowMapTextureUniform = glGetUniformLocation(programID, "dirLightShadowMap");

    numSpotLightShadowMapsUniform = glGetUniformLocation(programID, "numShadowMaps");
    debugShadowMappingUniform = glGetUniformLocation(programID, "debugShadowMapping");
    shadowMappingIntensityUniform = glGetUniformLocation(programID, "shadowIntensity");

    shadowMapArrayUniform = glGetUniformLocation(programID, "shadowMapArray");

    enableDirectionalLightShadowMapUniform = glGetUniformLocation(programID, "enableDirectionalLightShadowMapping");
}

void ShaderOGLRenderForward::PrepareMainThread()
{
    ShaderBaseOpenGL::PrepareMainThread();
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    LoadUniforms();
    InitLightBuffers();
}

void ShaderOGLRenderForward::render(
    Object3D *o,
    GLuint textureID,
    GLuint textureSpecularID,
    GLuint vertexbuffer,
    GLuint uvbuffer,
    GLuint normalbuffer,
    int size,
    GLuint fbo
) const
{
    Components::get()->Render()->ChangeOpenGLFramebuffer(fbo);
    Components::get()->Render()->ChangeOpenGLProgram(programID);

    glBindVertexArray(VertexArrayID);

    auto camera = Components::get()->Camera();

    setFloatUniform(alphaUniform, o->getAlpha());

    setMat4Uniform(matrixProjectionUniform, camera->getGLMMat4ProjectionMatrix());
    setMat4Uniform(matrixViewUniform, camera->getGLMMat4ViewMatrix());
    setMat4Uniform(matrixModelUniform, o->getModelMatrix());

    setVec3Uniform(viewPositionUniform, camera->getCamera()->getPosition().toGLM());

    auto shaders = Components::get()->Render()->getShaders();
    //_-
    setMat4Uniform(directionalLightMatrixUniform, getDirectionalLightMatrix(directionalLight));
    setBoolUniform(enableDirectionalLightShadowMapUniform, Config::get()->SHADOW_MAPPING_ENABLE_DIRECTIONAL_LIGHT);
    setIntUniform(numSpotLightShadowMapsUniform, (int) shaders->shaderOGLRender->getShadowMappingSpotLights().size());
    setBoolUniform(debugShadowMappingUniform, (Config::get()->SHADOW_MAPPING_DEBUG && Config::get()->ENABLE_SHADOW_MAPPING));
    setFloatUniform(shadowMappingIntensityUniform, Config::get()->SHADOW_MAPPING_INTENSITY);
    setBoolUniform(enableLightsUniform, o->isEnableLights()),

        // textures
    setIntUniform(materialTextureDiffuseUniform, 0);
    setIntUniform(materialTextureSpecularUniform, 1);
    setFloatUniform(materialShininessUniform, 32.0f);

    setVec3Uniform(directionalLightDirectionUniform, directionalLight.direction);
    setVec3Uniform(directionalLightAmbientUniform, directionalLight.ambient);
    setVec3Uniform(directionalLightDiffuseUniform, directionalLight.diffuse);
    setVec3Uniform(directionalLightSpecularUniform, directionalLight.specular);

    setIntUniform(numLightPointsUniform, (int) pointsLights.size());
    setIntUniform(numSpotLightsUniform, (int) spotLights.size());

    setVec3("drawOffset", o->getDrawOffset().toGLM());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureSpecularID);

    setTextureArrayUniform(shadowMapArrayUniform, shaders->shaderShadowPass->getSpotLightsShadowMapArrayTextures(), 4);
    setTextureUniform(dirLightShadowMapTextureUniform, shaders->shaderShadowPass->getDirectionalLightDepthTexture(), 5);

    setVAOAttributes(vertexbuffer, uvbuffer, normalbuffer);
    glUniformBlockBinding(programID, glGetUniformBlockIndex(programID, "PointLightsBlock"), 0);
    glUniformBlockBinding(programID, glGetUniformBlockIndex(programID, "SpotLightsBlock"), 1);
    glUniformBlockBinding(programID, glGetUniformBlockIndex(programID, "SpotLightsShadowMapDepthTexturesBlock"), 2);

    auto settings = o->getRenderSettings();

    settings.blend ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
    glBlendFunc(o->getRenderSettings().mode_src, o->getRenderSettings().mode_dst);
    settings.depthTest ?  glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    settings.writeDepth ?  glDepthMask(GL_TRUE) : glDepthMask(GL_FALSE);
    settings.culling ?  glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);;

    glDrawArrays(GL_TRIANGLES, 0, size);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);


    Components::get()->Render()->ChangeOpenGLFramebuffer(0);
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

void ShaderOGLRenderForward::CreateUBOFromLights()
{
    pointsLights.resize(0);
    spotLights.resize(0);
    shadowMappingLights.resize(0);

    if (Config::get()->ENABLE_LIGHTS) {
        for (auto &o : Brakeza::get()->getSceneObjects()) {
            if (!o->isEnabled()) continue;
            ExtractLights(o);
            for (auto &a: o->getAttached()) {
                if (!a->isEnabled()) continue;
                ExtractLights(a);
            }
        }
    }

    FillUBOLights();
}

DirLightOpenGL &ShaderOGLRenderForward::getDirectionalLight()
{
    return directionalLight;
}

void ShaderOGLRenderForward::Destroy()
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
            fbo
        );
    }
}

void ShaderOGLRenderForward::FillUBOLights()
{
    if (!buffersInitialized) {
        InitLightBuffers();
    }

    size_t numPointLights = std::min(pointsLights.size(), MAX_POINT_LIGHTS);
    if (numPointLights > 0) {
        glBindBuffer(GL_UNIFORM_BUFFER, bufferUBOLightPoints);
        glBufferSubData(GL_UNIFORM_BUFFER, 0,numPointLights * sizeof(PointLightOpenGL), pointsLights.data());
    }
    if (pointsLights.size() > MAX_POINT_LIGHTS) {
        LOG_MESSAGE("Point lights exceed max: %zu > %zu", pointsLights.size(), MAX_POINT_LIGHTS);
    }

    size_t numSpotLights = std::min(spotLights.size(), MAX_SPOT_LIGHTS);
    if (numSpotLights > 0) {
        glBindBuffer(GL_UNIFORM_BUFFER, bufferUBOSpotLights);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, numSpotLights * sizeof(SpotLightOpenGL),spotLights.data());
    }

    if (spotLights.size() > MAX_POINT_LIGHTS) {
        LOG_MESSAGE("Spot lights exceed max: %zu > %zu", spotLights.size(), MAX_POINT_LIGHTS);
    }
}

void ShaderOGLRenderForward::ExtractLights(Object3D *o)
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

[[nodiscard]] std::vector<LightSpot *> &ShaderOGLRenderForward::getShadowMappingSpotLights()
{
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


void ShaderOGLRenderForward::InitLightBuffers()
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

    LOG_MESSAGE("[ShaderOGLRenderForward] Light Buffers created");
    buffersInitialized = true;
}