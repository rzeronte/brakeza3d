//
// Created by Eduardo on 14/11/2025.
//

#ifndef BRAKEZA3D_SHADERSHADOWPASSDEBUGLIGHT_H
#define BRAKEZA3D_SHADERSHADOWPASSDEBUGLIGHT_H

#include "../Base/ShaderBaseOpenGL.h"
#include "../ShaderOGLRenderForward.h"

class ShaderOGLShadowPassDebugLight : public ShaderBaseOpenGL, public ShaderBaseOpenGLQuad  {
    GLuint internalFramebuffer;
    GLuint sceneTexture;
    std::vector<GLuint> internalTextures;

    public:
    ShaderOGLShadowPassDebugLight();
    void LoadUniforms() override;

    void PrepareMainThread() override;

    void renderInternalToTexture();
    void renderInternalFromArrayTextures(GLuint depthTexture, int layer);
    void Destroy() override;
    void CreateFramebuffer();
    void createArrayTextures(int nLayers);
    void updateDebugTextures(int numLights);
    void clearInternalTextures();
    [[nodiscard]] GLuint getSceneFramebuffer() const;
    [[nodiscard]] GLuint getSceneTexture() const;
    [[nodiscard]] GLuint getInternalTexture(int layer) const;
    static GLuint extractLayerFromArray(GLuint arrayTexture, int layer);
};


#endif //BRAKEZA3D_SHADERSHADOWPASSDEBUGLIGHT_H