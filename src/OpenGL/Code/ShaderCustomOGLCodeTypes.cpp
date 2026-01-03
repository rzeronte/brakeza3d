//
// Created by Eduardo on 03/01/2026.
//

#include "../../../include/OpenGL/Code/ShaderCustomOGLCodeTypes.h"

std::map<std::string, ShaderTypeInfo> GLSLTypeMapping = {
    {"int", {ShaderOpenGLCustomDataType::INT, "Integer" }},
    {"float", {ShaderOpenGLCustomDataType::FLOAT, "Float" }},
    {"vec2", {ShaderOpenGLCustomDataType::VEC2, "Vec2" }},
    {"vec3", {ShaderOpenGLCustomDataType::VEC3, "Vec3" }},
    {"vec4", {ShaderOpenGLCustomDataType::VEC4, "Vec4" }},
    {"texture", {ShaderOpenGLCustomDataType::TEXTURE2D, "Texture"}},
    {"diffuse", {ShaderOpenGLCustomDataType::DIFFUSE, "Diffuse Mesh3D texture (internal)" }},
    {"specular", {ShaderOpenGLCustomDataType::SPECULAR, "Specular Mesh3D texture (internal)" }},
    {"delta_time", {ShaderOpenGLCustomDataType::DELTA_TIME, "Delta time (internal)" }},
    {"execution_time", {ShaderOpenGLCustomDataType::EXECUTION_TIME, "Execution time (internal)" }},
    {"scene", {ShaderOpenGLCustomDataType::SCENE, "Scene texture (internal)" }},
    {"depth", {ShaderOpenGLCustomDataType::DEPTH, "Depth texture (internal)" }}
};