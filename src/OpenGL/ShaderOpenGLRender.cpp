#define GL_GLEXT_PROTOTYPES

#include <SDL2/SDL_opengl.h>
#include <gtc/type_ptr.hpp>
#include "../../include/OpenGL/ShaderOpenGLRender.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"

ShaderOpenGLRender::ShaderOpenGLRender()
:
    bufferUBOSpotLights(0),
    bufferUBOLightPoints(0),
    directionalLight(DirLightOpenGL{
    glm::vec3(0, 0, 1),
    glm::vec3(0.3f, 0.3f, 0.3f),
    glm::vec3(0.4f, 0.4f, 0.4f),
    glm::vec3(0.5f, 0.5f, 0.5f)
    }),
        VertexArrayID(0),
        ShaderOpenGL(
        EngineSetup::get()->SHADERS_FOLDER + "Render.vs",
        EngineSetup::get()->SHADERS_FOLDER + "Render.fs"
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
}

void ShaderOpenGLRender::renderMesh(Mesh3D *o, GLuint framebuffer)
{
    render(
        o,
        o->getModelTextures()[0]->getOGLTextureID(),
        o->getModelTextures()[0]->getOGLTextureID(),
        o->vertexbuffer,
        o->uvbuffer,
        o->normalbuffer,
        o->vertices.size(),
        framebuffer
    );
}

void ShaderOpenGLRender::render(
    Object3D *o,
    GLint textureID,
    GLint textureSpecularID,
    GLuint vertexbuffer,
    GLuint uvbuffer,
    GLuint normalbuffer,
    int size,
    GLuint framebuffer
)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glUseProgram(programID);
    glBindVertexArray(VertexArrayID);

    auto camera = ComponentsManager::get()->getComponentCamera()->getCamera();
    auto cameraPosition = ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition().toGLM();

    setMat4Uniform(matrixProjectionUniform,  Camera3D::getProjectionMatrix());
    setMat4Uniform(matrixViewUniform, camera->getViewMatrix());
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

    setIntUniform(numLightPointsUniform, (int) pointsLights.size());
    setIntUniform(numSpotLightsUniform, (int) spotLights.size());

    Vertex3D forward = ComponentsManager::get()->getComponentCamera()->getCamera()->getRotation().getTranspose() * Vertex3D(0, 0, 1);

    // spotLight
    setVec4("spotLight.position", glm::vec4(cameraPosition, 0));
    setVec4("spotLight.direction", forward.x, forward.y, forward.z, 0);
    setVec4("spotLight.ambient", 0.0f, 0.0f, 0.0f, 0);
    setVec4("spotLight.diffuse", 1.0f, 1.0f, 1.0f, 0);
    setVec4("spotLight.specular", 1.0f, 1.0f, 1.0f, 0);
    setFloat("spotLight.constant", 1.0f);
    setFloat("spotLight.linear", 0.09f);
    setFloat("spotLight.quadratic", 0.032f);
    setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureSpecularID);

    setVAOAttributes(vertexbuffer, uvbuffer, normalbuffer);
    glUniformBlockBinding(programID, glGetUniformBlockIndex(programID, "PointLightsBlock"), 0);
    glUniformBlockBinding(programID, glGetUniformBlockIndex(programID, "SpotLightsBlock"), 1);


    glDrawArrays(GL_TRIANGLES, 0, size );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);



    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShaderOpenGLRender::setVAOAttributes(GLuint vertexbuffer, GLuint uvbuffer, GLuint normalbuffer)
{
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,nullptr);

    // 3rd attribute buffer : normals
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,nullptr);

    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,0,nullptr);
}

void ShaderOpenGLRender::createUBOFromLights()
{
    pointsLights.resize(0);
    spotLights.resize(0);

    auto &sceneObjects = Brakeza3D::get()->getSceneObjects();
    for (auto o : sceneObjects) {
        if (!o->isEnabled()) continue;
        Vertex3D forward = o->getRotation().getTranspose() * Vertex3D(0, 0, 1);

        auto p = o->getPosition();

        auto s = dynamic_cast<SpotLight3D*>(o);
        if (s != nullptr) {
            spotLights.push_back(SpotLightOpenGL{
                glm::vec4(p.x, p.y, p.z, 0),
                glm::vec4(forward.x, forward.y, forward.z, 0),
                s->ambient,
                s->diffuse,
                s->specular,
                s->constant,
                s->linear,
                s->quadratic,
                s->cutOff,
                s->outerCutOff
            });
        }

        auto l = dynamic_cast<LightPoint3D*>(o);
        if (l != nullptr) {
            pointsLights.push_back({
               glm::vec4(p.x, p.y, p.z, 0),
               l->ambient,
               l->diffuse,
               l->specular,
               l->constant,
               l->linear,
               l->quadratic
           });
        }
    }

    foo();

}

void ShaderOpenGLRender::foo()
{
    glDeleteBuffers(1, &bufferUBOLightPoints);
    glDeleteBuffers(1, &bufferUBOSpotLights);

    glGenBuffers(1, &bufferUBOLightPoints);
    glBindBuffer(GL_UNIFORM_BUFFER, bufferUBOLightPoints);
    glBufferData(GL_UNIFORM_BUFFER, (int) (pointsLights.size() * sizeof(PointLightOpenGL)), pointsLights.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, bufferUBOLightPoints);

    glGenBuffers(1, &bufferUBOSpotLights);
    glBindBuffer(GL_UNIFORM_BUFFER, bufferUBOSpotLights);
    glBufferData(GL_UNIFORM_BUFFER, (int) (spotLights.size() * sizeof(SpotLightOpenGL)), spotLights.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, bufferUBOSpotLights);
}

DirLightOpenGL *ShaderOpenGLRender::getDirectionalLight() {
    return &directionalLight;
}

void ShaderOpenGLRender::destroy() {

}

