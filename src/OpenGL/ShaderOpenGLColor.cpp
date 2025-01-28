
//
// Created by edu on 17/12/23.
//

#include "../../include/OpenGL/ShaderOpenGLColor.h"
#include "../../include/ComponentsManager.h"

ShaderOpenGLColor::ShaderOpenGLColor()
:
    VertexArrayID(0),
    framebuffer(0),
    textureColorbuffer(0),
    ShaderOpenGL(
        EngineSetup::get()->SHADERS_FOLDER + "Color.vs",
        EngineSetup::get()->SHADERS_FOLDER + "Color.fs"
    )
{
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    glGenFramebuffers(1, &framebuffer);
}

void ShaderOpenGLColor::render(
    Object3D *o,
    GLuint vertexbuffer,
    GLuint uvbuffer,
    GLuint normalbuffer,
    int size,
    bool toFramebuffer,
    Color color
)
{
    if (toFramebuffer) {
        int w = ComponentsManager::get()->getComponentWindow()->getWidth();
        int h = ComponentsManager::get()->getComponentWindow()->getHeight();

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glGenTextures(1, &textureColorbuffer);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    glUseProgram(programID);
    glBindVertexArray(VertexArrayID);

    glm::mat4 ViewMatrix = ComponentsManager::get()->getComponentCamera()->getCamera()->getGLMMat4ViewMatrix();
    glm::mat4 ProjectionMatrix = Camera3D::getGLMMat4ProjectionMatrix();

    setMat4("projection", ProjectionMatrix);
    setMat4("view", ViewMatrix);
    setMat4("model", o->getModelMatrix());

    setVec3("color", color.toGLM());

    setVAOAttributes(vertexbuffer, uvbuffer, normalbuffer);

    glDrawArrays(GL_TRIANGLES, 0, (GLint) size );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShaderOpenGLColor::setVAOAttributes(GLuint vertexbuffer, GLuint uvbuffer, GLuint normalbuffer)
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

void ShaderOpenGLColor::destroy() {

}
