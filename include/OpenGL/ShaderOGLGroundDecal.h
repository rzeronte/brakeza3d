#ifndef BRAKEZA3D_SHADEROGL_GROUNDDECAL_H
#define BRAKEZA3D_SHADEROGL_GROUNDDECAL_H

#include "Base/ShaderBaseOpenGL.h"
#include "../Render/Color.h"
#include <map>
#include <string>

class Object3D;

class ShaderOGLGroundDecal : public ShaderBaseOpenGL
{
    GLuint vao = 0;

    GLuint viewUniform       = 0;
    GLuint projectionUniform = 0;
    GLuint tintUniform       = 0;
    GLuint textureUniform    = 0;

    std::map<std::string, GLuint> textureCache;

    GLuint loadTexture(const std::string& path);

public:
    ShaderOGLGroundDecal();
    void PrepareMainThread() override;
    void LoadUniforms() override;
    void Destroy() override;

    void draw(Object3D* obj, const std::string& texturePath, const Color& tint, float radius, GLuint fbo);
};

#endif //BRAKEZA3D_SHADEROGL_GROUNDDECAL_H
