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

    // Renderizar mesh específico con sus texturas
    void RenderMesh(
        Mesh3D* mesh,
        int materialIndex,
        GLuint vertexbuffer,
        GLuint uvbuffer,
        GLuint normalbuffer,
        int vertexCount,
        GLuint fbo
    );
    ShaderNodeEditorManager* GetNodeManager() const             { return dynamic_cast<ShaderNodeEditorManager*>(getNodeManager()); }

    void DrawImGuiProperties(const Image* diffuse, Image* specular) override;
};

#endif // SHADERNODESMESH3D_H