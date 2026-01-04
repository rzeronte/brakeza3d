//
// Created by Eduardo on 04/01/2026.
//

#ifndef BRAKEZA3D_SHADERNODESMESH3D_H
#define BRAKEZA3D_SHADERNODESMESH3D_H

#include "../../3D/Mesh3D.h"
#include "../Base/ShaderBaseCustom.h"


class ShaderNodesMesh3D : public ShaderBaseCustom
{
    Mesh3D* mesh;
public:
    ShaderNodesMesh3D(const std::string &label, Mesh3D* mesh);

    void LoadUniforms() override;

    void Destroy() override;

    void Reload() override;

    void Render(GLuint fbo, GLuint texture) override;

    void DrawImGuiProperties(const Image *diffuse, Image *specular) override;
};


#endif //BRAKEZA3D_SHADERNODESMESH3D_H