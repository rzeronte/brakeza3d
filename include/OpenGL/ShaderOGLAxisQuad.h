#ifndef BRAKEZA3D_SHADEROGL_AXISQUAD_H
#define BRAKEZA3D_SHADEROGL_AXISQUAD_H

#include "Base/ShaderBaseOpenGL.h"
#include "../Render/Color.h"
#include "../3D/Vertex3D.h"

class Object3D;

class ShaderOGLAxisQuad : public ShaderBaseOpenGL
{
public:
    enum Axis {
        AXIS_X,  // quad normal faces +X (uses Y,Z basis)
        AXIS_Y,  // quad normal faces +Y (uses X,Z basis) — default
        AXIS_Z   // quad normal faces +Z (uses X,Y basis)
    };

private:
    GLuint vao = 0;
    GLuint vbo = 0;

    GLuint modelUniform       = 0;
    GLuint viewUniform        = 0;
    GLuint projectionUniform  = 0;
    GLuint colorUniform       = 0;

    void drawImpl(const glm::vec3& pos, const glm::mat4& rotationMatrix,
                  const Color& color, float halfSize, GLuint fbo, Axis axis);

public:
    ShaderOGLAxisQuad();

    void PrepareMainThread() override;
    void LoadUniforms() override;
    void Destroy() override;

    void draw(Object3D* obj, const Color& color, float halfSize, GLuint fbo, Axis axis = AXIS_Y);
    void drawAt(const Vertex3D& pos, const Color& color, float halfSize, GLuint fbo, Axis axis = AXIS_Y);
};

#endif
