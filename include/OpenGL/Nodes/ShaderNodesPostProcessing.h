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
// Created by Eduardo on 04/01/2026.
//

#ifndef BRAKEZA3D_SHADERNODESPOSTPROCESSING_H
#define BRAKEZA3D_SHADERNODESPOSTPROCESSING_H

#include "ShaderBaseNodes.h"
#include "../../GUI/ShaderNodeEditorManager.h"

class ShaderNodesPostProcessing  : public ShaderBaseNodes
{
public:
    ShaderNodesPostProcessing(const std::string &label, const std::string &typesFile, ShaderCustomType type, ShaderNodeEditorManager *nodeManager);

    void LoadUniforms() override;
    void Destroy() override;
    void Reload() override;
    void Render(GLuint fbo, GLuint scenePostProcessedTexture) override;
    void DrawImGuiProperties(const Image *diffuse, Image *specular) override;
    void UpdateInternalTextures(GLuint colorTexture, GLuint depthTexture) const;
    ShaderNodeEditorManager* GetNodeManager() const { return dynamic_cast<ShaderNodeEditorManager*>(getNodeManager()); }
};


#endif //BRAKEZA3D_SHADERNODESPOSTPROCESSING_H