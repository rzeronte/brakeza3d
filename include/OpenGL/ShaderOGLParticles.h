//
// Created by edu on 22/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLPARTICLES_H
#define BRAKEZA3D_SHADEROPENGLPARTICLES_H

#include "Base/ShaderBaseOpenGL.h"

struct Particle {
    glm::vec3 pos, speed;
    unsigned char r,g,b,a; // Color
    float size, angle, weight;
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
public:
    ShaderOGLParticles();
    void PrepareMainThread() override;
    void LoadUniforms() override;
    void render(
        GLuint billboard_vertex_buffer,
        GLuint particles_position_buffer,
        GLuint particles_color_buffer,
        GLuint textureID,
        int particlesCount
    );
    void Destroy() override;

private:
};


#endif //BRAKEZA3D_SHADEROPENGLPARTICLES_H
