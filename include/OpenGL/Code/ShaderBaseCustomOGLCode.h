#ifndef BRAKEZA3D_SHADEROPENGLCUSTOM_H
#define BRAKEZA3D_SHADEROPENGLCUSTOM_H

#define GL_GLEXT_PROTOTYPES

#include <string>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "../../Misc/cJSON.h"
#include "../../Render/Image.h"
#include <vector>
#include "../Base/SharedOpenGLStructs.h"
#include "../Base/ShaderBaseCustom.h"

class ShaderBaseCustomOGLCode: public ShaderBaseCustom
{
    GLuint resultFramebuffer = 0;
    GLuint textureResult = 0;
    std::vector<ShaderOGLCustomType> dataTypesDefaultValues;
protected:

    int numTextures = 0;

    ShaderBaseCustomOGLCode(const std::string &label, const std::string &typesFile, const std::string &vsFile, const std::string &fsFile, ShaderCustomType type, const cJSON *types);
public:
    explicit ShaderBaseCustomOGLCode(const std::string &label, const std::string &typesFile, const std::string &vsFile, const std::string &fsFile, ShaderCustomType type);
    std::vector<ShaderOGLCustomType> dataTypes;

    void PrepareBackground() override;
    void PrepareMainThread() override;

    static void DrawTypeImGuiControl(ShaderOGLCustomType &type);
    static int CountTypesByFilter(const std::vector<ShaderOGLCustomType> &types,const std::vector<ShaderOpenGLCustomDataType> &filterTypes);
    static void DrawTypeInternalImGuiControl(const ShaderOGLCustomType &type);

    void DrawImGuiProperties(const Image *diffuse, Image *specular) override;

    std::string sourceVS;
    std::string sourceFS;

    void Destroy() override;

protected:

    bool existDataType(const char *name, const char *type) const;
    void ParseTypesFromFileAttributes();
    static std::string dataTypesFileFor(std::string basicString);
    static std::string ExtractOnlyName(std::string &filename);
    void setDataTypesFromJSON(const cJSON *typesJSON);
    void addDataType(const char *name, const char *type, cJSON *value);

public:
    void onUpdate() const;
    void postUpdate(GLuint outputFBO, GLuint inputTexture);
    void setDataTypesUniforms();
    void UpdateFileTypes() const;
    void AddDataTypeEmpty(const char *name, const char *type);
    void removeDataType(const ShaderOGLCustomType &data);
    void setDataTypeValue(const std::string &name, const ShaderOpenGLCustomDataValue &newValue);
    void setDataTypeValue(const std::string &name, int newValue);
    void setDataTypeValue(const std::string &name, float newValue);
    void setDataTypeValue(const std::string &name, glm::vec2 newValue);
    void setDataTypeValue(const std::string &name, glm::vec3 newValue);
    void setDataTypeValue(const std::string &name, glm::vec4 newValue);
    void ResetNumberTextures();
    void IncreaseNumberTextures();
    [[nodiscard]] std::string getFolder() const;
    [[nodiscard]] cJSON* getTypesJSON() const override;
    [[nodiscard]] std::vector<ShaderOGLCustomType> &getDataTypes()      { return dataTypes; }
    static ShaderCustomType getShaderTypeFromString(const std::string &shaderName);
    static std::string getShaderTypeString(ShaderCustomType type);

    static void WriteEmptyCustomShaderToDisk(const std::string& name, const std::string& folder, ShaderCustomType type);
    static void RemoveCustomShaderFiles(const std::string& folder, const std::string &name);
    static ShaderCustomType ExtractTypeFromShaderName(const std::string& folder, const std::string &name);
    void Reload() override;
    void CaptureDragDropUpdateImage(ShaderOGLCustomType &type, const Image *texture) const;
    void CreateFramebuffer();
    void setTextureResult(GLuint value);
};


#endif //BRAKEZA3D_SHADEROPENGLCUSTOM_H
