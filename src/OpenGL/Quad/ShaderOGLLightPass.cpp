
#include "../../../include/OpenGL/Quad/ShaderOGLLightPass.h"
#include "../../../include/Components/Components.h"
#include "../../../include/Brakeza.h"
#include <glm/gtc/type_ptr.hpp>

#define GL_GLEXT_PROTOTYPES

ShaderOGLLightPass::ShaderOGLLightPass()
:
    ShaderBaseOpenGL(
        Config::get()->SHADERS_FOLDER + "LightingPass.vs",
        Config::get()->SHADERS_FOLDER + "LightingPass.fs",
        false
    )
{
}

void ShaderOGLLightPass::LoadUniforms()
{
    gPositionUniform = glGetUniformLocation(programID, "gPosition");
    gNormalUniform = glGetUniformLocation(programID, "gNormal");
    viewPosUniform = glGetUniformLocation(programID, "viewPos");

    numPointLightsUniform = glGetUniformLocation(programID, "numPointLights");
    numSpotLightsUniform = glGetUniformLocation(programID, "numSpotLights");

    directionalLightDirectionUniform = glGetUniformLocation(programID, "dirLight.direction");
    directionalLightAmbientUniform = glGetUniformLocation(programID, "dirLight.ambient");
    directionalLightDiffuseUniform = glGetUniformLocation(programID, "dirLight.diffuse");
    directionalLightSpecularUniform = glGetUniformLocation(programID, "dirLight.specular");
    directionalLightMatrixUniform = glGetUniformLocation(programID, "dirLightMatrix");

    dirLightShadowMapTextureUniform = glGetUniformLocation(programID, "dirLightShadowMap");

    materialTextureDiffuseUniform = glGetUniformLocation(programID, "material.diffuse");
    materialTextureSpecularUniform = glGetUniformLocation(programID, "material.specular");
    materialShininessUniform = glGetUniformLocation(programID, "material.shininess");

    numSpotLightShadowMapsUniform = glGetUniformLocation(programID, "numShadowMaps");
    debugShadowMappingUniform = glGetUniformLocation(programID, "debugShadowMapping");
    shadowMappingIntensityUniform = glGetUniformLocation(programID, "shadowIntensity");

    shadowMapArrayUniform = glGetUniformLocation(programID, "shadowMapArray");

    enableDirectionalLightShadowMapUniform = glGetUniformLocation(programID, "enableDirectionalLightShadowMapping");
}

void ShaderOGLLightPass::PrepareMainThread()
{
    ShaderBaseOpenGL::PrepareMainThread();
    LoadUniforms();
    CreateQuadVBO();
    SetupQuadUniforms(programID);
}

void ShaderOGLLightPass::render(
    GLuint gPosition,
    GLuint gNormal,
    GLuint gAlbedoSpec,
    const DirLightOpenGL &directionalLight,
    GLuint dirLightShadowMapTexture,
    int numPointLights,
    int numSpotLights,
    GLuint spotLightsShadowMapTexturesArray,
    int numSpotLightsShadowMaps,
    GLuint fbo
) {
    Components::get()->Render()->ChangeOpenGLFramebuffer(fbo);
    Components::get()->Render()->ChangeOpenGLProgram(programID);

    LoadQuadMatrixUniforms();

    auto camera = Components::get()->Camera();
    auto cameraPosition = camera->getCamera()->getPosition().toGLM();

    setVec3Uniform(viewPosUniform, cameraPosition);

    setTextureUniform(gPositionUniform, gPosition, 0);
    setTextureUniform(gNormalUniform, gNormal, 1);
    setTextureUniform(materialTextureDiffuseUniform, gAlbedoSpec, 2);
    setTextureUniform(materialTextureSpecularUniform, gAlbedoSpec, 3);
    setTextureArrayUniform(shadowMapArrayUniform, spotLightsShadowMapTexturesArray, 4);
    setTextureUniform(dirLightShadowMapTextureUniform, dirLightShadowMapTexture, 5);

    setIntUniform(numSpotLightShadowMapsUniform, numSpotLightsShadowMaps);
    setBoolUniform(debugShadowMappingUniform, (Config::get()->SHADOW_MAPPING_DEBUG && Config::get()->ENABLE_SHADOW_MAPPING));
    setFloatUniform(shadowMappingIntensityUniform, Config::get()->SHADOW_MAPPING_INTENSITY);

    setFloatUniform(materialShininessUniform, 32.0f);

    setVec3Uniform(directionalLightDirectionUniform, directionalLight.direction);
    setVec3Uniform(directionalLightAmbientUniform, directionalLight.ambient);
    setVec3Uniform(directionalLightDiffuseUniform, directionalLight.diffuse);
    setVec3Uniform(directionalLightSpecularUniform, directionalLight.specular);

    setMat4Uniform(directionalLightMatrixUniform, ShaderOGLRenderForward::getDirectionalLightMatrix(directionalLight));

    setIntUniform(numPointLightsUniform, numPointLights);
    setIntUniform(numSpotLightsUniform, numSpotLights);

    setBoolUniform(enableDirectionalLightShadowMapUniform, Config::get()->SHADOW_MAPPING_ENABLE_DIRECTIONAL_LIGHT);

    glUniformBlockBinding(programID, glGetUniformBlockIndex(programID, "PointLightsBlock"), 0);
    glUniformBlockBinding(programID, glGetUniformBlockIndex(programID, "SpotLightsBlock"), 1);
    glUniformBlockBinding(programID, glGetUniformBlockIndex(programID, "SpotLightsShadowMapDepthTexturesBlock"), 2);

    glDisable(GL_DEPTH_TEST);
    DrawQuad();

    glBindVertexArray(0);
}

void ShaderOGLLightPass::Destroy()
{
    ResetQuadMatrix();
}

void ShaderOGLLightPass::FillSpotLightsMatricesUBO()
{
    glDeleteBuffers(1, &bufferSpotLightsMatricesUBO);

    std::vector<glm::mat4> spotLightsShadowMapLightMatrices;

    auto spotLights = Components::get()->Render()->getShaders()->shaderOGLRender->getShadowMappingSpotLights();

    for (auto & spotLight : spotLights) {
        spotLightsShadowMapLightMatrices.push_back(spotLight->getLightSpaceMatrix());
    }

    glGenBuffers(1, &bufferSpotLightsMatricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, bufferSpotLightsMatricesUBO);
    glBufferData(GL_UNIFORM_BUFFER, static_cast<int>(spotLightsShadowMapLightMatrices.size() * sizeof(glm::mat4)), spotLightsShadowMapLightMatrices.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, bufferSpotLightsMatricesUBO);

    spotLightsShadowMapLightMatrices.clear();
}