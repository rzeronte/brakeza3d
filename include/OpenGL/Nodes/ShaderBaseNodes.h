//
// Created by Eduardo on 04/01/2026.
//

#ifndef BRAKEZA3D_SHADERBASENODES_H
#define BRAKEZA3D_SHADERBASENODES_H

#include "../Base/ShaderBaseCustom.h"

class ShaderBaseNodes : public ShaderBaseCustom
{
    NodeEditorManager *nodeManager;
public:
    ShaderBaseNodes(const std::string &label, const std::string &fileTypes, ShaderCustomType type, NodeEditorManager *nodeManager)
    :
        ShaderBaseCustom(label, fileTypes, type),
        nodeManager(nodeManager)
    {
    }

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