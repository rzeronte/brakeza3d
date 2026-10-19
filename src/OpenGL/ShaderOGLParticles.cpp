
#include "../../include/OpenGL/ShaderOGLParticles.h"
#include "../../include/Brakeza.h"
#include "../../include/Components/Components.h"
#include "../../include/Render/Profiler.h"

ShaderOGLParticles::ShaderOGLParticles()
:
    ShaderBaseOpenGL(
        Config::get()->SHADERS_FOLDER + "Particle.vs",
        Config::get()->SHADERS_FOLDER + "Particle.fs",
        false
    )
{
}

void ShaderOGLParticles::PrepareMainThread()
{
    ShaderBaseOpenGL::PrepareMainThread();
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    LoadUniforms();
}

void ShaderOGLParticles::LoadUniforms()
{
    CameraRight_worldspace_ID  = glGetUniformLocation(programID, "CameraRight_worldspace");
    CameraUp_worldspace_ID  = glGetUniformLocation(programID, "CameraUp_worldspace");
    ViewProjMatrixID = glGetUniformLocation(programID, "VP");

    textureIDuniform = glGetUniformLocation(programID, "myTextureSampler");
}

void ShaderOGLParticles::render(
    GLuint billboard_vertex_buffer,
    GLuint particles_position_buffer,
    GLuint particles_color_buffer,
    GLuint particles_rotation_buffer,
    GLuint textureID,
    int particlesCount,
    bool depthTest
)
{
    // depthTest: el sceneFBO comparte el depth del G-Buffer, así la geometría opaca oculta las partículas.
    // El foregroundFBO no tiene profundidad, por eso allí siempre quedan por encima de todo.
    auto* window = Components::get()->Window();
    Components::get()->Render()->ChangeOpenGLFramebuffer(
        depthTest ? window->getSceneFramebuffer() : window->getForegroundFramebuffer()
    );

    Components::get()->Render()->ChangeOpenGLProgram(programID);
    glBindVertexArray(VertexArrayID);

    glm::mat4 ProjectionMatrix = Components::get()->Camera()->getGLMMat4ProjectionMatrix();
    glm::mat4 ViewMatrix = Components::get()->Camera()->getGLMMat4ViewMatrix();
    glm::mat4 ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (depthTest) {
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);      // prueba contra la escena pero no escribe: las partículas no se tapan entre sí
    }

    Components::get()->Render()->ChangeOpenGLProgram(programID);

    setTextureUniform(textureIDuniform, textureID, 0);


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
            nullptr            // array buffer offset
    );

    // 2nd attribute buffer: positions of particles' centers
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glVertexAttribPointer(
            1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
            4,                                // size : x + y + z + size => 4
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            nullptr                          // array buffer offset
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
            nullptr                          // array buffer offset
    );

    // 4th attribute buffer : ángulo de cada partícula (radianes, giro en el plano de la pantalla)
    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, particles_rotation_buffer);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, nullptr);

    glVertexAttribDivisor(0, 0); // particles vertices: always reuse the same 4 vertices -> 0
    glVertexAttribDivisor(1, 1); // positions: one per quad (its center)                 -> 1
    glVertexAttribDivisor(2, 1); // color: one per quad                                  -> 1
    glVertexAttribDivisor(3, 1); // rotation: one per quad                               -> 1

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particlesCount);
    Profiler::get()->incrementDrawCall(GL_TRIANGLE_STRIP, 4, particlesCount, "Particles");

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);

    glDisable(GL_BLEND);
    if (depthTest) glDepthMask(GL_TRUE);

    Components::get()->Render()->ChangeOpenGLFramebuffer(0);
}

void ShaderOGLParticles::Destroy() {

}