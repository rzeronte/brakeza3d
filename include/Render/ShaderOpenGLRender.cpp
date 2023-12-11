#define GL_GLEXT_PROTOTYPES

#include <SDL2/SDL_opengl.h>
#include <gtc/type_ptr.hpp>
#include "ShaderOpenGLRender.h"
#include "../ComponentsManager.h"
#include "../Brakeza3D.h"

ShaderOpenGLRender::ShaderOpenGLRender(const std::string &vertexFilename, const std::string &fragmentFilename)
:
    VertexArrayID(0),
    ShaderOpenGL(vertexFilename, fragmentFilename)
{
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
}

void ShaderOpenGLRender::render(
    glm::mat4 ModelMatrix,
    GLint textureID,
    GLint textureSpecularID,
    GLuint vertexbuffer,
    GLuint uvbuffer,
    GLuint normalbuffer,
    int size
)
{
    glUseProgram(programID);
    glBindVertexArray(VertexArrayID);

    glm::mat4 ViewMatrix = ComponentsManager::get()->getComponentCamera()->getCamera()->getViewMatrix();
    glm::mat4 ProjectionMatrix = Camera3D::getProjectionMatrix();

    auto cameraPosition = ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition();
    glm::vec3 cameraPos = glm::vec3(cameraPosition.x, cameraPosition.y, cameraPosition.z);

    setMat4("projection", ProjectionMatrix);
    setMat4("view", ViewMatrix);
    setMat4("model", ModelMatrix);
    setVec3("viewPos", cameraPos);

    setInt("numLights", (int) pointsLights.size());

    setInt("material.diffuse", 0);
    setInt("material.specular", 1);
    setFloat("material.shininess", 32.0f);

    // directional light
    setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

    //fillPointLightsUniform();

    getPointLightFromSceneObjects();
    fillPointLightsUBO();

    Vertex3D forward = ComponentsManager::get()->getComponentCamera()->getCamera()->getRotation().getTranspose() * Vertex3D(0, 0, 1);

    // spotLight
    setVec3("spotLight.position", cameraPos);
    setVec3("spotLight.direction", forward.x, forward.y, forward.z);
    setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
    setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
    setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
    setFloat("spotLight.constant", 1.0f);
    setFloat("spotLight.linear", 0.09f);
    setFloat("spotLight.quadratic", 0.032f);
    setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

    selectActiveTextures(textureID, textureSpecularID);
    setVAOAttributes(vertexbuffer, uvbuffer, normalbuffer);

    glDrawArrays(GL_TRIANGLES, 0, (GLint) size );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    glDeleteBuffers(1, &bufferUBO);
}

void ShaderOpenGLRender::fillPointLightsUBO()
{
    glGenBuffers(1, &bufferUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, bufferUBO);
    glBufferData(GL_UNIFORM_BUFFER, (int) (pointsLights.size() * sizeof(PointLight)), pointsLights.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, bufferUBO);
    glUniformBlockBinding(programID, glGetUniformBlockIndex(programID, "PointLightsBlock"), 0);
}

void ShaderOpenGLRender::fillPointLightsUniform() const {
    for (int i = 0; i < (int) pointsLights.size(); ++i) {
        const PointLight& light = pointsLights[i];
        std::string prefix = "pointLights[" + std::to_string(i) + "].";

        setVec4(prefix + "position", light.position);
        setVec4(prefix + "ambient", light.ambient);
        setVec4(prefix + "diffuse", light.diffuse);
        setVec4(prefix + "specular", light.specular);
        setFloat(prefix + "constant", light.constant);
        setFloat(prefix + "linear", light.linear);
        setFloat(prefix + "quadratic", light.quadratic);
    }
}

void ShaderOpenGLRender::setVAOAttributes(GLuint vertexbuffer, GLuint uvbuffer, GLuint normalbuffer)
{
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
            0,                  // attribute
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            nullptr
    );
    // 3rd attribute buffer : normals
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(
            1,                                // attribute
            3,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            nullptr
    );

    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(
            2,                                // attribute
            2,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            nullptr
    );
}

void ShaderOpenGLRender::selectActiveTextures(GLint textureID, GLint textureSpecularID)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureSpecularID);
}

void ShaderOpenGLRender::getPointLightFromSceneObjects()
{
    pointsLights.clear();

    auto &sceneObjects = Brakeza3D::get()->getSceneObjects();
    for (auto o : sceneObjects) {
        auto l = dynamic_cast<LightPoint3D*>(o);
        auto p = o->getPosition();

        if (l == nullptr) continue;

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
