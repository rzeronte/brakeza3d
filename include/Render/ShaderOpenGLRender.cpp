#define GL_GLEXT_PROTOTYPES

#include <SDL2/SDL_opengl.h>
#include <gtc/type_ptr.hpp>
#include "ShaderOpenGLRender.h"
#include "../ComponentsManager.h"

ShaderOpenGLRender::ShaderOpenGLRender(const std::string &vertexFilename, const std::string &fragmentFilename)
:
    VertexArrayID(0),
    ShaderOpenGL(vertexFilename, fragmentFilename)
{
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    MatrixUniform = glGetUniformLocation(programID, "MVP");
    ViewMatrixUniform = glGetUniformLocation(programID, "V");
    ModelMatrixUniform = glGetUniformLocation(programID, "M");
    TextureUniform = glGetUniformLocation(getProgramID(), "myTextureSampler");
    pointLightPositionsUniform = glGetUniformLocation(getProgramID(), "pointLights");
    numPointLightsUniform = glGetUniformLocation(getProgramID(), "numPointLights");
    viewPosUniform = glGetUniformLocation(getProgramID(), "viewPos");

     LightIDUniform = glGetUniformLocation(getProgramID(), "LightPosition_worldspace");

    pointLightPositions.push_back({glm::vec3(1.0f, 1.0, 11000), glm::vec3(1.0f), glm::vec3(1.0f)});
}

GLuint ShaderOpenGLRender::getVertexArrayID() const {
    return VertexArrayID;
}

void ShaderOpenGLRender::render(
    glm::mat4 ModelMatrix,
    GLint textureID,
    GLuint vertexbuffer,
    GLuint uvbuffer,
    GLuint normalbuffer,
    int size
)
{
    glUseProgram(getProgramID());
    glBindVertexArray(getVertexArrayID());

    glm::mat4 ViewMatrix = ComponentsManager::get()->getComponentCamera()->getCamera()->getViewMatrix();
    glm::mat4 MVP = Camera3D::getProjectionMatrix() * ViewMatrix * ModelMatrix;

    auto cameraPosition = ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition();
    glm::vec3 cameraPos = glm::vec3(cameraPosition.x, cameraPosition.y, cameraPosition.z);
    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(MatrixUniform, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(ModelMatrixUniform, 1, GL_FALSE, &ModelMatrix[0][0]);
    glUniformMatrix4fv(ViewMatrixUniform, 1, GL_FALSE, &ViewMatrix[0][0]);
    glUniform3f(LightIDUniform, cameraPos.x, cameraPos.y, cameraPos.z);
    /*glUniform1i(numPointLightsUniform, (int) pointLightPositions.size());
    glUniform3fv(viewPosUniform, 1, &cameraPos[0]);
    glUniform3fv(pointLightPositionsUniform, (int)pointLightPositions.size() * 4, glm::value_ptr(pointLightPositions[0].position));
    glUniform3fv(pointLightPositionsUniform + 1, (int)pointLightPositions.size() * 4, glm::value_ptr(pointLightPositions[0].ambient));
    glUniform3fv(pointLightPositionsUniform + 2, (int)pointLightPositions.size() * 4, glm::value_ptr(pointLightPositions[0].diffuse));
    glUniform3fv(pointLightPositionsUniform + 3, (int)pointLightPositions.size() * 4, glm::value_ptr(pointLightPositions[0].specular));
     */

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    // Set our "myTextureSampler" sampler to use Texture Unit 0
    glUniform1i(TextureUniform, 0);

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

    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(
            1,                                // attribute
            2,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            nullptr
    );

    // 3rd attribute buffer : normals
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(
            2,                                // attribute
            3,                                // size
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