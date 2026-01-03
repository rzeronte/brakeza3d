//
// Created by Eduardo on 03/01/2026.
//

#include "../../../include/OpenGL/Base/ShaderBaseCustom.h"

#include "../../../include/Misc/Logging.h"

ShaderBaseCustom::ShaderBaseCustom(const std::string &label, ShaderCustomType type, const std::string &vertexFilename, const std::string &fragmentFilename, const std::string &fileTypes, bool enableFeedback)
    :
    ShaderBaseOpenGL(vertexFilename, fragmentFilename, enableFeedback),
    fileTypes(fileTypes),
    label(label),
    type(type)
{

}

ShaderBaseCustom::ShaderBaseCustom(const std::string &label, ShaderCustomType type, const std::string &vertexFilename, const std::string &fileTypes, bool enableFeedback)
    :
    ShaderBaseOpenGL(vertexFilename, enableFeedback),
    fileTypes(fileTypes),
    label(label),
    type(type)
{
}

ShaderBaseCustom::ShaderBaseCustom(const std::string &label, ShaderCustomType type, NodeEditorManager *nodeManager)
    :
    label(label),
    type(type)
{
}

void ShaderBaseCustom::setLabel(const std::string &value)
{
    label = value;
}

void ShaderBaseCustom::setEnabled(bool value)
{
    LOG_MESSAGE("[ShaderOGLCustom] Setting shader '%s' enabled to %d", label.c_str(), value);
    enabled = value;
}
