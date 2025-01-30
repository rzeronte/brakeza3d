
#include "../../include/OpenGL/ShaderOpenGLParticles.h"
#include "../../include/Brakeza3D.h"

ShaderOpenGLParticles::ShaderOpenGLParticles()
:
    VertexArrayID(0),
    ShaderOpenGL(
        EngineSetup::get()->SHADERS_FOLDER + "Particle.vs",
        EngineSetup::get()->SHADERS_FOLDER + "Particle.fs"
    )
{
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    CameraRight_worldspace_ID  = glGetUniformLocation(programID, "CameraRight_worldspace");
    CameraUp_worldspace_ID  = glGetUniformLocation(programID, "CameraUp_worldspace");
    ViewProjMatrixID = glGetUniformLocation(programID, "VP");

    TextureID = glGetUniformLocation(programID, "myTextureSampler");
}

void ShaderOpenGLParticles::render(
    GLuint billboard_vertex_buffer,
    GLuint particles_position_buffer,
    GLuint particles_color_buffer,
    GLint textureID,
    int particlesCount
)
{
    glBindFramebuffer(GL_FRAMEBUFFER, ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer());

    glUseProgram(programID);
    glBindVertexArray(VertexArrayID);

    glm::mat4 ProjectionMatrix = Camera3D::getGLMMat4ProjectionMatrix();
    glm::mat4 ViewMatrix = ComponentsManager::get()->getComponentCamera()->getGLMMat4ViewMatrix();
    glm::mat4 ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(programID);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(TextureID, 0);

    glUniform3f(CameraRight_worldspace_ID, ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
    glUniform3f(CameraUp_worldspace_ID   , ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
    glUniformMatrix4fv(ViewProjMatrixID, 1, GL_FALSE, &ViewProjectionMatrix[0][0]);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glVertexAttribPointer(
            0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
    );

    // 2nd attribute buffer : positions of particles' centers
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glVertexAttribPointer(
            1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
            4,                                // size : x + y + z + size => 4
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
    );

    // 3rd attribute buffer : particles' colors
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    glVertexAttribPointer(
            2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
            4,                                // size : r + g + b + a => 4
            GL_UNSIGNED_BYTE,                 // type
            GL_TRUE,                          // normalized?    *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
            0,                                // stride
            (void*)0                          // array buffer offset
    );

    glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
    glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
    glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particlesCount);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShaderOpenGLParticles::destroy() {

}