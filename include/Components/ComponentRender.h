//
// Created by darkhead on 8/1/20.
//

#ifndef BRAKEDA3D_COMPONENTRENDER_H
#define BRAKEDA3D_COMPONENTRENDER_H

#include <vector>
#include "Component.h"
#include "../Render/Triangle3D.h"
#include "../Render/TextWriter.h"
#include "../Loaders/ProjectLoader.h"
#include "../Loaders/SceneLoader.h"
#include "../OpenGL/ShaderOGLImage.h"
#include "../OpenGL/ShaderOGLRenderForward.h"
#include "../OpenGL/ShaderOGLLine.h"
#include "../OpenGL/ShaderOGLWire.h"
#include "../OpenGL/ShaderOGLShading.h"
#include "../OpenGL/ShaderOGLPoints.h"
#include "../OpenGL/ShaderOGLOutline.h"
#include "../OpenGL/ShaderOGLColor.h"
#include "../OpenGL/ShaderOGLParticles.h"
#include "../OpenGL/ShaderOGLDOF.h"
#include "../OpenGL/ShaderOGLDepthMap.h"
#include "../OpenGL/ShaderOGLFog.h"
#include "../OpenGL/ShaderOGLTint.h"
#include "../OpenGL/ShaderOGLLine3D.h"
#include "../OpenGL/ShaderOGLBonesTransforms.h"
#include "../OpenGL/ShaderOGLGrid.h"
#include "../OpenGL/ShaderOGLRenderDeferred.h"
#include "../OpenGL/ShaderOGLLightPass.h"
#include "../OpenGL/ShaderOGLShadowPass.h"
#include "../OpenGL/ShaderOGLShadowPassDebugLight.h"

struct Shaders {
    ShaderOGLRenderForward *shaderOGLRender = nullptr;
    ShaderOGLImage *shaderOGLImage = nullptr;
    ShaderOGLLine *shaderOGLLine = nullptr;
    ShaderOGLWire *shaderOGLWireframe = nullptr;
    ShaderOGLLine3D *shaderOGLLine3D = nullptr;
    ShaderOGLShading *shaderOGLShading = nullptr;
    ShaderOGLPoints *shaderOGLPoints = nullptr;
    ShaderOGLOutline *shaderOGLOutline = nullptr;
    ShaderOGLColor *shaderOGLColor = nullptr;
    ShaderOGLParticles *shaderOGLParticles = nullptr;
    ShaderOGLDOF *shaderOGLDOFBlur = nullptr;
    ShaderOGLDepthMap *shaderOGLDepthMap = nullptr;
    ShaderOGLFog *shaderOGLFOG = nullptr;
    ShaderOGLTint *shaderOGLTint = nullptr;
    ShaderOGLBonesTransforms *shaderOGLBonesTransforms = nullptr;
    ShaderOGLRenderDeferred *shaderOGLGBuffer = nullptr;
    ShaderOGLLightPass *shaderOGLLightPass = nullptr;
    ShaderOGLShadowPass *shaderShadowPass = nullptr;
    ShaderOGLShadowPassDebugLight *shaderShadowPassDebugLight = nullptr;
    ShaderOGLGrid *shaderOGLGrid = nullptr;
};

class ComponentRender : public Component
{
    int fps = 0;
    int fpsFrameCounter = 0;
    float frameTime = 0.f;

    GLuint lastFrameBufferUsed = 0;
    GLuint lastProgramUsed = 0;

    Object3D *selectedObject = nullptr;

    TextWriter *textWriter = nullptr;
    SceneLoader sceneLoader;
    ProjectLoader projectLoader;


    std::map<std::string, ShaderCustomType> ShaderTypesMapping = {
        {"Postprocessing", SHADER_POSTPROCESSING},
        {"Mesh3D", SHADER_OBJECT},
    };

    std::vector<ShaderOGLCustom*> sceneShaders;

    Shaders shaders;
public:
    ComponentRender() = default;
    ~ComponentRender() override;

    void onStart() override;
    void preUpdate() override;
    void DrawFPS() const;
    void onUpdate() override;
    void postUpdate() override;
    void onEnd() override;
    void RegisterShaders();
    void onSDLPollEvent(SDL_Event *event, bool &finish) override;
    void setSelectedObject(Object3D *o);
    void UpdateFPS();
    void UpdateSelectedObject3D();
    void LoadShaderIntoScene(const std::string &name);
    void AddShaderToScene(ShaderOGLCustom *shader);

    void RemoveSceneShaderByIndex(int index);
    void RemoveSceneShader(const ShaderOGLCustom *);
    void setGlobalIlluminationDirection(const Vertex3D &d) const;
    void setGlobalIlluminationAmbient(const Vertex3D &a) const;
    void setGlobalIlluminationDiffuse(const Vertex3D &d) const;
    void setGlobalIlluminationSpecular(const Vertex3D &s) const;
    void DrawLine(const Vertex3D &from, const Vertex3D &to, const Color &c) const;
    void setLastFrameBufferUsed(GLuint value);
    void setLastProgramUsed(GLuint value);
    void ChangeOpenGLFramebuffer(GLuint);
    void changeOpenGLProgram(GLuint);
    void resizeShadersFramebuffers() const;
    void ClearShadowMaps() const;
    void FlipBuffersToGlobal() const;

    SceneLoader &getSceneLoader()                                                                 { return sceneLoader; }
    ProjectLoader &getProjectLoader()                                                             { return projectLoader; }
    std::vector<ShaderOGLCustom *> &getSceneShaders()                                             { return sceneShaders; }
    Shaders *getShaders()                                                                         { return &shaders;}
    [[nodiscard]] ShaderOGLCustom *getSceneShaderByIndex(int i) const                             { return sceneShaders[i]; }
    [[nodiscard]] int getFps() const                                                              { return fps; }
    [[nodiscard]] ShaderOGLDepthMap *getShaderOGLDepthMap() const                                 { return shaders.shaderOGLDepthMap; }
    [[nodiscard]] ShaderOGLRenderDeferred *getShaderOGLRenderDeferred() const                     { return shaders.shaderOGLGBuffer; }
    [[nodiscard]] ShaderOGLLightPass *getShaderOGLLightPass() const                               { return shaders.shaderOGLLightPass; }
    [[nodiscard]] GLuint getLastFrameBufferUsed() const                                           { return lastFrameBufferUsed; }
    [[nodiscard]] Object3D* getSelectedObject() const                                             { return selectedObject; }
    [[nodiscard]] GLuint getLastProgramUsed() const                                               { return lastProgramUsed; }
    [[nodiscard]] const std::map<std::string, ShaderCustomType> &getShaderTypesMapping() const    { return ShaderTypesMapping; }
    [[nodiscard]] ShaderOGLCustom *getSceneShaderByLabel(const std::string& name) const;

    static bool compareDistances(const Object3D *obj1, const Object3D *obj2);
    static void RunSceneShadersPostUpdate();
    static void FillOGLBuffers(std::vector<Mesh3DData> &meshes);
    static void DeleteRemovedObjects();
    static void onUpdateSceneObjects();
    static void MakeScreenShot(std::string filename = "");
    static ShaderOGLCustom* CreateCustomShaderFromDisk(const ShaderOGLMetaInfo &info, Mesh3D* o);
};

#endif //BRAKEDA3D_COMPONENTRENDER_H