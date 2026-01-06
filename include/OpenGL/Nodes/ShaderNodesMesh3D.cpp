//
// Created by Eduardo on 04/01/2026.
//

#include "ShaderNodesMesh3D.h"

ShaderNodesMesh3D::ShaderNodesMesh3D(const std::string &label, const std::string &typesFile, ShaderCustomType type, NodeEditorManager *nodeManager, Mesh3D *mesh)
:
    ShaderBaseNodes(label, typesFile, type, nodeManager),
    mesh(mesh)
{
}

void ShaderNodesMesh3D::LoadUniforms() {
}

void ShaderNodesMesh3D::Destroy() {
}

void ShaderNodesMesh3D::Reload() {
}

void ShaderNodesMesh3D::Render(GLuint fbo, GLuint texture) {
}

void ShaderNodesMesh3D::DrawImGuiProperties(const Image *diffuse, Image *specular) {
}
