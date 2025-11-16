
#include "../../include/OpenGL/ShaderOGLDeferredLighting.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"
#include <glm/gtc/type_ptr.hpp>

#define GL_GLEXT_PROTOTYPES

ShaderOGLDeferredLighting::ShaderOGLDeferredLighting()
:
    ShaderOpenGL(
        EngineSetup::get()->SHADERS_FOLDER + "LightingPass.vs",
        EngineSetup::get()->SHADERS_FOLDER + "LightingPass.fs",
        false
    )
{
    setupQuadUniforms(programID);

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

    enableDirectionalLightShadowMapUniform = glGetUniformLocation(programID, "enableDirectionalLightShadowMapping");
}

void ShaderOGLDeferredLighting::setSpotLightInCameraUniforms(glm::vec3 cameraPosition, const Vertex3D &forward) const {

    // spotLight
    setVec4("spotLight.position", glm::vec4(cameraPosition, 0));
    setVec4("spotLight.direction", glm::vec4(forward.toGLM(), 0));
    setVec4("spotLight.ambient", 0.0f, 0.0f, 0.0f, 0);
    setVec4("spotLight.diffuse", 1.0f, 1.0f, 1.0f, 0);
    setVec4("spotLight.specular", 1.0f, 1.0f, 1.0f, 0);
    setFloat("spotLight.constant", 1.0f);
    setFloat("spotLight.linear", 0.09f);
    setFloat("spotLight.quadratic", 0.032f);
    setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
}

void ShaderOGLDeferredLighting::render(
    GLuint gPosition,
    GLuint gNormal,
    GLuint gAlbedoSpec,
    const DirLightOpenGL &directionalLight,
    GLuint dirLightShadowMapTexture,
    int numPointLights,
    int numSpotLights,
    GLuint spotLightsShadowMapTexturesArray,
    int numSpotLightsShadowMaps,
    GLuint framebuffer
) {
    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(framebuffer);
    ComponentsManager::get()->getComponentRender()->changeOpenGLProgram(programID);

    loadQuadMatrixUniforms();

    auto camera = ComponentsManager::get()->getComponentCamera();
    auto cameraPosition = camera->getCamera()->getPosition().toGLM();

    setVec3Uniform(viewPosUniform, cameraPosition);
    
    // Bind G-Buffer textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glUniform1i(gPositionUniform, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glUniform1i(gNormalUniform, 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    setIntUniform(materialTextureDiffuseUniform, 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    setIntUniform(materialTextureSpecularUniform, 3);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D_ARRAY, spotLightsShadowMapTexturesArray);
    glUniform1i(glGetUniformLocation(programID, "shadowMapArray"), 4);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, dirLightShadowMapTexture);
    setIntUniform(dirLightShadowMapTextureUniform, 5);

    setIntUniform(numSpotLightShadowMapsUniform, numSpotLightsShadowMaps);
    setBoolUniform(debugShadowMappingUniform, EngineSetup::get()->SHADOW_MAPPING_DEBUG);
    setFloatUniform(shadowMappingIntensityUniform, EngineSetup::get()->SHADOW_MAPPING_INTENSITY);

    setFloatUniform(materialShininessUniform, 32.0f);

    setVec3Uniform(directionalLightDirectionUniform, directionalLight.direction);
    setVec3Uniform(directionalLightAmbientUniform, directionalLight.ambient);
    setVec3Uniform(directionalLightDiffuseUniform, directionalLight.diffuse);
    setVec3Uniform(directionalLightSpecularUniform, directionalLight.specular);

    auto shaderRender = ComponentsManager::get()->getComponentRender()->getShaderOGLRender();
    setMat4Uniform(directionalLightMatrixUniform, shaderRender->getDirectionalLightMatrix(directionalLight));

    setIntUniform(numPointLightsUniform, numPointLights);
    setIntUniform(numSpotLightsUniform, numSpotLights);

    Vertex3D forward = camera->getCamera()->getRotation().getTranspose() * Vertex3D(0, 0, -1);
    setSpotLightInCameraUniforms(cameraPosition, forward);

    setBoolUniform(enableDirectionalLightShadowMapUniform, EngineSetup::get()->SHADOW_MAPPING_ENABLE_DIRECTIONAL_LIGHT);

    glUniformBlockBinding(programID, glGetUniformBlockIndex(programID, "PointLightsBlock"), 0);
    glUniformBlockBinding(programID, glGetUniformBlockIndex(programID, "SpotLightsBlock"), 1);
    glUniformBlockBinding(programID, glGetUniformBlockIndex(programID, "SpotLightsShadowMapDepthTexturesBlock"), 2);

    drawQuad();

    glBindVertexArray(0);
}

void ShaderOGLDeferredLighting::destroy()
{
    resetQuadMatrix();
}

void ShaderOGLDeferredLighting::fillSpotLightsMatricesUBO()
{
    glDeleteBuffers(1, &bufferSpotLightsMatricesUBO);

    std::vector<glm::mat4> spotLightsShadowMapLightMatrices;

    auto spotLights = ComponentsManager::get()->getComponentRender()->getShaderOGLRender()->getShadowMappingSpotLights();

    for (int i = 0; i < spotLights.size(); i++) {
        spotLightsShadowMapLightMatrices.push_back(spotLights[i]->getLightSpaceMatrix());
    }

    glGenBuffers(1, &bufferSpotLightsMatricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, bufferSpotLightsMatricesUBO);
    glBufferData(GL_UNIFORM_BUFFER, static_cast<int>(spotLightsShadowMapLightMatrices.size() * sizeof(glm::mat4)), spotLightsShadowMapLightMatrices.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, bufferSpotLightsMatricesUBO);

    spotLightsShadowMapLightMatrices.clear();
}
