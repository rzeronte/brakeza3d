/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
#ifndef SHADERNODESMESH3D_H
#define SHADERNODESMESH3D_H

#include "ShaderBaseNodes.h"
#include "../../GUI/ShaderNodeEditorManager.h"
#include "../../3D/Mesh3D.h"

class ShaderNodesMesh3D : public ShaderBaseNodes {
private:
    Mesh3D* currentMesh;

public:
    ShaderNodesMesh3D(
        const std::string& label,
        const std::string& typesFile,
        ShaderCustomType type,
        ShaderNodeEditorManager* nodeManager,
        Mesh3D* mesh
    );

    void LoadUniforms() override;
    void Destroy() override;
    void Reload() override;

    // El gestor de nodos usa OpenGL y el editor de nodos (globales de ImGui): se crea y carga aquí,
    // en el hilo principal, nunca en el hilo de carga del mesh.
    void PrepareMainThread() override;

    ShaderNodeEditorManager* GetNodeManager() const             { return dynamic_cast<ShaderNodeEditorManager*>(getNodeManager()); }

    void DrawImGuiProperties(const Image* diffuse, Image* specular) override;
};

#endif // SHADERNODESMESH3D_H