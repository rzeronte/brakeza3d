/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
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