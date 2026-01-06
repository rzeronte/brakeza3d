//
// Created by Eduardo on 04/01/2026.
//

#ifndef BRAKEZA3D_SHADERNODESMESH3D_H
#define BRAKEZA3D_SHADERNODESMESH3D_H

#include "ShaderBaseNodes.h"
#include "../../3D/Mesh3D.h"

class ShaderNodesMesh3D : public ShaderBaseNodes
{
    Mesh3D* mesh;
public:
    ShaderNodesMesh3D(const std::string &label, const std::string &typesFile, ShaderCustomType type, NodeEditorManager *nodeManager, Mesh3D *mesh);

    void LoadUniforms() override;

    void Destroy() override;

    void Reload() override;

    void Render(GLuint fbo, GLuint texture) override;

    void DrawImGuiProperties(const Image *diffuse, Image *specular) override;
};


#endif //BRAKEZA3D_SHADERNODESMESH3D_H