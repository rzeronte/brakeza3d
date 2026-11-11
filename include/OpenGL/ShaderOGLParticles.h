//
// Created by edu on 22/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLPARTICLES_H
#define BRAKEZA3D_SHADEROPENGLPARTICLES_H

#include "Base/ShaderBaseOpenGL.h"

struct Particle {
    glm::vec3 pos, speed;
    unsigned char r,g,b,a; // Color
    float size, angle, weight;     // angle: rotación en el plano de la pantalla (radianes)
    float spin;                    // velocidad de giro (radianes/s)
    float life; // Remaining life of the particle. if <0 : dead and unused.
    float cameradistance; // *Squared* distance to the camera. if dead : -1.0f

    bool operator<(const Particle& that) const {
        // Sort in reverse order : far particles drawn first.
        return this->cameradistance > that.cameradistance;
    }
};

#define MaxParticles 512

class ShaderOGLParticles : public ShaderBaseOpenGL
{
    GLuint VertexArrayID = 0;

    GLuint CameraRight_worldspace_ID = 0;
    GLuint CameraUp_worldspace_ID = 0;
    GLuint ViewProjMatrixID = 0;
    GLuint textureIDuniform = 0;
    GLuint orientationID = 0;
    GLuint flatRightID = 0;
    GLuint flatUpID = 0;
    GLuint cameraPosID = 0;
    GLuint flutterAngleID = 0;
    GLuint flutterSpeedID = 0;
public:
    ShaderOGLParticles();
    void PrepareMainThread() override;
    void LoadUniforms() override;
    void render(
        GLuint billboard_vertex_buffer,
        GLuint particles_position_buffer,
        GLuint particles_color_buffer,
        GLuint particles_rotation_buffer,
        GLuint textureID,
        int particlesCount,
        bool depthTest = false,
        int orientation = 0,                                 // ParticleOrientation (0 = billboard, original)
        const glm::vec3 &flatRight = glm::vec3(1, 0, 0),     // ejes del quad cuando no es billboard
        const glm::vec3 &flatUp = glm::vec3(0, 0, -1),
        float flutterAngle = 0.0f,                           // balanceo en grados (0 = sin balanceo, original)
        float flutterSpeed = 0.5f
    );
    void Destroy() override;

private:
};


#endif //BRAKEZA3D_SHADEROPENGLPARTICLES_H
