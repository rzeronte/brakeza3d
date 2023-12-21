#ifndef BRAKEZA3D_SHADERCUSTOMOPENGLPARTICLES_H
#define BRAKEZA3D_SHADERCUSTOMOPENGLPARTICLES_H

#include <vector>
#include "ShaderOpenGLCustom.h"
#include "../Objects/OpenCLTypes.h"

#define SHADERGL_NUM_PARTICLES 1024

class ShaderCustomOpenGLParticles : public ShaderOpenGLCustom {
public:
    explicit ShaderCustomOpenGLParticles(const std::string &shaderFilename);

    void render(GLint particlesBuffer, int numberParticles, Vertex3D origin, Vertex3D direction, OCParticlesContext context, Color cf, Color ct, float executionTime);
};


#endif //BRAKEZA3D_SHADERCUSTOMOPENGLPARTICLES_H
