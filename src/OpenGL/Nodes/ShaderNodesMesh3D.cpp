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

#include "../../../include/OpenGL/Nodes/ShaderNodesMesh3D.h"
#include "../../../include/GUI/ShaderNodeEditorManager.h"
#include "../../../include/Components/Components.h"

ShaderNodesMesh3D::ShaderNodesMesh3D(
    const std::string& label,
    const std::string& typesFile,
    ShaderCustomType type,
    ShaderNodeEditorManager* nodeManager,
    Mesh3D* mesh
)
:
    ShaderBaseNodes(label, typesFile, type, nodeManager),
    currentMesh(mesh)
{
}

void ShaderNodesMesh3D::LoadUniforms()
{
    // Los uniforms se cargan automáticamente por el shader generado
}

void ShaderNodesMesh3D::Destroy()
{
}

void ShaderNodesMesh3D::Reload()
{
}

void ShaderNodesMesh3D::PrepareMainThread()
{
    if (getNodeManager()) return;   // ya creado (carga síncrona desde disco)

    auto manager = new ShaderNodeEditorManager(getType());
    manager->LoadFromFile(getTypesFile().c_str());
    setNodeManager(manager);
}

void ShaderNodesMesh3D::DrawImGuiProperties(const Image* diffuse, Image* specular)
{
    if (!GetNodeManager()) return;
    GetNodeManager()->RenderShaderDebugPanel();
}