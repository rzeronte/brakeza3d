//
// Created by edu on 22/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLPARTICLES_H
#define BRAKEZA3D_SHADEROPENGLPARTICLES_H


#include "ShaderOpenGL.h"

struct Particle{
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

#define MaxParticles 100000

class ShaderOpenGLParticles : ShaderOpenGL {
    GLuint VertexArrayID;

    GLuint CameraRight_worldspace_ID;
    GLuint CameraUp_worldspace_ID;
    GLuint ViewProjMatrixID;
    GLuint TextureID;


public:
    ShaderOpenGLParticles(const std::string &vertexFilename, const std::string &fragmentFilename);
    void render(
        GLuint billboard_vertex_buffer,
        GLuint particles_position_buffer,
        GLuint particles_color_buffer,
        GLint textureID,
        int particlesCount
    );

    int FindUnusedParticle();

    void SortParticles();
};


#endif //BRAKEZA3D_SHADEROPENGLPARTICLES_H
