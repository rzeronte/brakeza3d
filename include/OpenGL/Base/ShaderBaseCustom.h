//
// Created by Eduardo on 03/01/2026.
//

#ifndef BRAKEZA3D_SHADERBASECUSTOM_H
#define BRAKEZA3D_SHADERBASECUSTOM_H

#include "SharedOpenGLStructs.h"
#include "ShaderBaseOpenGL.h"
#include "ShaderBaseOpenGLQuad.h"
#include "../../../src/GUI/NodeEditorManager.h"
#include "../../Misc/cJSON.h"

class ShaderBaseCustom : public ShaderBaseOpenGL, public ShaderBaseOpenGLQuad
{
    bool enabled = true;
protected:
    std::string fileTypes;
    std::string label;
    ShaderCustomType type;
public:
    ShaderBaseCustom(
        const std::string &label,
        ShaderCustomType type,
        const std::string &vertexFilename,
        const std::string &fragmentFilename,
        const std::string &fileTypes, bool enableFeedback
    );

    ShaderBaseCustom(
        const std::string &label,
        ShaderCustomType type,
        const std::string &vertexFilename,
        const std::string &fileTypes,
        bool enableFeedback
    );

    ShaderBaseCustom(const std::string &label, ShaderCustomType type, NodeEditorManager *nodeManager);

    [[nodiscard]] const std::string &getLabel() const       { return label; }
    [[nodiscard]] bool isEnabled() const                    { return enabled; }
    [[nodiscard]] ShaderCustomType getType() const          { return type; }
    [[nodiscard]] std::string getTypesFile() const          { return fileTypes;}

    void setLabel(const std::string &value);
    void setEnabled(bool value);

    virtual void Reload() = 0;
    virtual cJSON* getTypesJSON() const = 0;
    virtual void render(GLuint fbo, GLuint texture) = 0;
    virtual void DrawImGuiProperties(const Image *diffuse, Image *specular) = 0;
};


#endif //BRAKEZA3D_SHADERBASECUSTOM_H