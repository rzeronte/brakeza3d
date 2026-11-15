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

#ifndef BRAKEZA3D_SHADERBASENODES_H
#define BRAKEZA3D_SHADERBASENODES_H

#include "../Base/ShaderBaseCustom.h"

class ShaderBaseNodes : public ShaderBaseCustom
{
    NodeEditorManager *nodeManager;
protected:
    // Para las subclases que crean el gestor más tarde (en el hilo principal, ver ShaderNodesMesh3D)
    void setNodeManager(NodeEditorManager *manager)     { nodeManager = manager; }
public:
    ShaderBaseNodes(const std::string &label, const std::string &fileTypes, ShaderCustomType type, NodeEditorManager *nodeManager)
    :
        ShaderBaseCustom(label, fileTypes, type),
        nodeManager(nodeManager)
    {
    }

    // El shader es dueño de su gestor de nodos (programa GL, VAO/VBO, contexto del editor, texturas)
    ~ShaderBaseNodes() override;

    // Los shaders de nodos no leen ficheros GLSL: el programa lo genera y compila el gestor
    void PrepareBackground() override {}
    void PrepareMainThread() override {}

    cJSON* getTypesJSON() const override;
    void LoadUniforms() override;
    void Destroy() override;
    void Reload() override;
    void Render(GLuint fbo, GLuint texture) override;
    void DrawImGuiProperties(const Image *diffuse, Image *specular) override;

    [[nodiscard]] NodeEditorManager* getNodeManager() const                    { return nodeManager; }

    void SaveToFile() const;

    static void WriteEmptyCustomShaderToDisk(const std::string& name, const std::string& folder, ShaderCustomType type, NodeEditorManager *nodeManager);

};


#endif //BRAKEZA3D_SHADERBASENODES_H