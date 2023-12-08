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

GLuint ShaderOpenGLRender::getVertexArrayID() const {
    return VertexArrayID;
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
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    glUseProgram(programID);
    glBindVertexArray(getVertexArrayID());

    glm::mat4 ViewMatrix = ComponentsManager::get()->getComponentCamera()->getCamera()->getViewMatrix();
    glm::mat4 ProjectionMatrix = Camera3D::getProjectionMatrix();

    setMat4("projection", ProjectionMatrix);
    setMat4("view", ViewMatrix);
    setMat4("model", ModelMatrix);

    auto cameraPosition = ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition();
    glm::vec3 cameraPos = glm::vec3(cameraPosition.x, cameraPosition.y, cameraPosition.z);

    setInt("material.diffuse", 0);
    setInt("material.specular", 1);

    setVec3("viewPos", cameraPos);
    setFloat("material.shininess", 32.0f);

    glm::vec3 pointLightPositions[] = {
            glm::vec3( 0.7f,  0.2f,  2.0f),
            glm::vec3( 2.3f, -3.3f, -4.0f),
            glm::vec3(-4.0f,  2.0f, -12.0f),
            glm::vec3( 0.0f,  0.0f, -3.0f)
    };

    // directional light
    setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
    // point light 1
    setVec3("pointLights[0].position", pointLightPositions[0]);
    setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
    setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
    setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
    setFloat("pointLights[0].constant", 1.0f);
    setFloat("pointLights[0].linear", 0.09f);
    setFloat("pointLights[0].quadratic", 0.032f);

    // point light 2
    setVec3("pointLights[1].position", pointLightPositions[1]);
    setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
    setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
    setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
    setFloat("pointLights[1].constant", 1.0f);
    setFloat("pointLights[1].linear", 0.09f);
    setFloat("pointLights[1].quadratic", 0.032f);
    // point light 3
    setVec3("pointLights[2].position", pointLightPositions[2]);
    setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
    setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
    setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
    setFloat("pointLights[2].constant", 1.0f);
    setFloat("pointLights[2].linear", 0.09f);
    setFloat("pointLights[2].quadratic", 0.032f);
    // point light 4
    setVec3("pointLights[3].position", pointLightPositions[3]);
    setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
    setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
    setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
    setFloat("pointLights[3].constant", 1.0f);
    setFloat("pointLights[3].linear", 0.09f);
    setFloat("pointLights[3].quadratic", 0.032f);


    Vertex3D forward = ComponentsManager::get()->getComponentCamera()->getCamera()->getRotation().getTranspose() * Vertex3D(0, 0, 1);

    const auto f = glm::vec3(forward.x, forward.y, forward.z);

    // spotLight
    setVec3("spotLight.position", cameraPos);
    setVec3("spotLight.direction", f);
    setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
    setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
    setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
    setFloat("spotLight.constant", 1.0f);
    setFloat("spotLight.linear", 0.09f);
    setFloat("spotLight.quadratic", 0.032f);
    setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));


    // bind diffuse map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // bind specular map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureSpecularID);

    // 1rst attribute buffer : vertices
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


    // Draw the triangles !
    glDrawArrays(GL_TRIANGLES, 0, (GLint) size );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void ShaderOpenGLRender::initDEMO() {


}