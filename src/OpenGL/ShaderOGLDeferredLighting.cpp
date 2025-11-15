
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
    numLightsUniform = glGetUniformLocation(programID, "numLights");
    numSpotLightsUniform = glGetUniformLocation(programID, "numSpotLights");
    
    directionalLightDirectionUniform = glGetUniformLocation(programID, "dirLight.direction");
    directionalLightAmbientUniform = glGetUniformLocation(programID, "dirLight.ambient");
    directionalLightDiffuseUniform = glGetUniformLocation(programID, "dirLight.diffuse");
    directionalLightSpecularUniform = glGetUniformLocation(programID, "dirLight.specular");

    materialTextureDiffuseUniform = glGetUniformLocation(programID, "material.diffuse");
    materialTextureSpecularUniform = glGetUniformLocation(programID, "material.specular");
    materialShininessUniform = glGetUniformLocation(programID, "material.shininess");

    numShadowMapsUniform = glGetUniformLocation(programID, "numShadowMaps");
    debugShadowMappingUniform = glGetUniformLocation(programID, "debugShadowMapping");
    shadowMappingIntensityUniform = glGetUniformLocation(programID, "shadowIntensity");
}

void ShaderOGLDeferredLighting::setSpotLightInCameraUniforms(glm::vec3 cameraPosition, Vertex3D forward) {

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
    int numLights,
    int numSpotLights,
    GLuint shadowMapArrayTex,
    int numShadowMaps,
    GLuint outputFramebuffer
) {
    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(outputFramebuffer);
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

    setIntUniform(numShadowMapsUniform, numShadowMaps);
    setBoolUniform(debugShadowMappingUniform, EngineSetup::get()->SHADOW_MAPPING_DEBUG);
    setFloatUniform(shadowMappingIntensityUniform, EngineSetup::get()->SHADOW_MAPPING_INTENSITY);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D_ARRAY, shadowMapArrayTex);

    setFloatUniform(materialShininessUniform, 32.0f);

    setVec3Uniform(directionalLightDirectionUniform, directionalLight.direction);
    setVec3Uniform(directionalLightAmbientUniform, directionalLight.ambient);
    setVec3Uniform(directionalLightDiffuseUniform, directionalLight.diffuse);
    setVec3Uniform(directionalLightSpecularUniform, directionalLight.specular);
    
    setIntUniform(numLightsUniform, numLights);
    setIntUniform(numSpotLightsUniform, numSpotLights);

    Vertex3D forward = camera->getCamera()->getRotation().getTranspose() * Vertex3D(0, 0, -1);

    setSpotLightInCameraUniforms(cameraPosition, forward);

    glUniformBlockBinding(programID, glGetUniformBlockIndex(programID, "PointLightsBlock"), 0);
    glUniformBlockBinding(programID, glGetUniformBlockIndex(programID, "SpotLightsBlock"), 1);
    glUniformBlockBinding(programID, glGetUniformBlockIndex(programID, "ShadowMapLightsBlock"), 2);

    glUniform1i(glGetUniformLocation(programID, "shadowMapArray"), 4);

    drawQuad();

    glBindVertexArray(0);
}

void ShaderOGLDeferredLighting::destroy()
{
    resetQuadMatrix();
}

void ShaderOGLDeferredLighting::fillUBOLightsMatrix()
{
    glDeleteBuffers(1, &bufferUBOLightsMatrix);

    std::vector<glm::mat4> shadowMapLightMatrix;

    auto render = ComponentsManager::get()->getComponentRender();
    auto shaderRender = render->getShaderOGLRender();

    auto lightPoints = shaderRender->getShadowMappingLightPoints();
    auto numLights = (int) lightPoints.size();

    for (int i = 0; i < numLights; i++) {
        shadowMapLightMatrix.push_back(lightPoints[i]->getLightSpaceMatrix());
    }

    //Logging::Message("UBO Lights Matrix Number: %d", shadowMapLightMatrix.size());

    glGenBuffers(1, &bufferUBOLightsMatrix);
    glBindBuffer(GL_UNIFORM_BUFFER, bufferUBOLightsMatrix);
    glBufferData(GL_UNIFORM_BUFFER, static_cast<int>(shadowMapLightMatrix.size() * sizeof(glm::mat4)), shadowMapLightMatrix.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, bufferUBOLightsMatrix);

    shadowMapLightMatrix.clear();
}