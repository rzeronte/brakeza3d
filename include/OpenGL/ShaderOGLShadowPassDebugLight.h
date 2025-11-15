//
// Created by Eduardo on 14/11/2025.
//

#ifndef BRAKEZA3D_SHADERSHADOWPASSDEBUGLIGHT_H
#define BRAKEZA3D_SHADERSHADOWPASSDEBUGLIGHT_H

#include "ShaderOpenGL.h"
#include "ShaderOpenGLRender.h"

class ShaderOGLShadowPassDebugLight : public ShaderOpenGL, public ShaderQuadOpenGL  {
    GLuint internalFramebuffer;
    GLuint sceneTexture;
    std::vector<GLuint> internalTextures;

    public:
    ShaderOGLShadowPassDebugLight();

    void render(GLuint shadowMapArrayTex, int layer, GLuint framebuffer);

    void renderInternal(GLuint shadowMapArrayTex, int layer);

    void destroy() override;

    void createFramebuffer();

    [[nodiscard]] GLuint getSceneFramebuffer() const;
    [[nodiscard]] GLuint getTexture() const;

    void createTextures(int numLayers);

    [[nodiscard]] GLuint getInternalTexture(int layer) const;

    void updateDebugTextures(int numLights);
    void clearInternalTextures();

};


#endif //BRAKEZA3D_SHADERSHADOWPASSDEBUGLIGHT_H