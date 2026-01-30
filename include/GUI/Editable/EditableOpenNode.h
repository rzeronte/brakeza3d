//
// Created by Eduardo on 04/01/2026.
//

#ifndef BRAKEZA3D_EDITABLEOPENNODE_H
#define BRAKEZA3D_EDITABLEOPENNODE_H

#include "EditableOpenBaseResource.h"
#include "../../OpenGL/Nodes/ShaderBaseNodes.h"

class EditableOpenNode : public EditableOpenBaseResource
{
    ShaderBaseNodes *shaderNodes;
    bool isLiveShader = false;
public:
    EditableOpenNode(const std::string &uniqueId, const std::string &tabLabel, const std::string &path, ShaderBaseNodes *shaderNodes, bool isLiveShader = false);
    void DrawTabEdition(int i) override;

    void DrawEditableOpenNodeConfig();

    void DrawNodeEditorTab() const;

    void DrawCodeEditActionButtons();

    ~EditableOpenNode() override;
};


#endif //BRAKEZA3D_EDITABLEOPENNODE_H