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
#include "../OpenGL/Quad/ShaderOGLLine.h"
#include "../OpenGL/ShaderOGLWire.h"
#include "../OpenGL/ShaderOGLShading.h"
#include "../OpenGL/ShaderOGLPoints.h"
#include "../OpenGL/Quad/ShaderOGLOutline.h"
#include "../OpenGL/ShaderOGLColor.h"
#include "../OpenGL/ShaderOGLParticles.h"
#include "../OpenGL/Quad/ShaderOGLDOF.h"
#include "../OpenGL/Quad/ShaderOGLDepthMap.h"
#include "../OpenGL/Quad/ShaderOGLFog.h"
#include "../OpenGL/Quad/ShaderOGLTint.h"
#include "../OpenGL/ShaderOGLLine3D.h"
#include "../OpenGL/ShaderOGLBonesTransforms.h"
#include "../OpenGL/Quad/ShaderOGLGrid.h"
#include "../OpenGL/ShaderOGLRenderDeferred.h"
#include "../OpenGL/Quad/ShaderOGLLightPass.h"
#include "../OpenGL/ShaderOGLShadowPass.h"
#include "../OpenGL/Quad/ShaderOGLShadowPassDebugLight.h"

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
        {"NodeMesh3D", SHADER_NODE_OBJECT},
        {"NodePostProcessing", SHADER_NODE_POSTPROCESSING},
    };

    std::vector<ShaderBaseCustom*> sceneShaders;

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
    void AddShaderToScene(ShaderBaseCustom *shader);
    void RemoveSceneShaderByIndex(int index);
    void RemoveSceneShader(const ShaderBaseCustom *);
    void setGlobalIlluminationDirection(Vertex3D d) const;
    void setGlobalIlluminationAmbient(Vertex3D a) const;
    void setGlobalIlluminationDiffuse(Vertex3D d) const;
    void setGlobalIlluminationSpecular(Vertex3D s) const;
    void DrawLine(const Vertex3D &from, const Vertex3D &to, const Color &c) const;
    void setLastFrameBufferUsed(GLuint value);
    void setLastProgramUsed(GLuint value);
    void ChangeOpenGLFramebuffer(GLuint);
    void ChangeOpenGLProgram(GLuint);
    void resizeShadersFramebuffers() const;
    void ClearShadowMaps() const;

    void LightPass() const;

    void FlipBuffersToGlobal() const;

    SceneLoader &getSceneLoader()                                                                 { return sceneLoader; }
    ProjectLoader &getProjectLoader()                                                             { return projectLoader; }
    std::vector<ShaderBaseCustom *> &getSceneShaders()                                            { return sceneShaders; }
    Shaders *getShaders()                                                                         { return &shaders;}
    [[nodiscard]] ShaderBaseCustom *getSceneShaderByIndex(int i) const                            { return sceneShaders[i]; }
    [[nodiscard]] int getFps() const                                                              { return fps; }
    [[nodiscard]] ShaderOGLDepthMap *getShaderOGLDepthMap() const                                 { return shaders.shaderOGLDepthMap; }
    [[nodiscard]] ShaderOGLRenderDeferred *getShaderOGLRenderDeferred() const                     { return shaders.shaderOGLGBuffer; }
    [[nodiscard]] ShaderOGLLightPass *getShaderOGLLightPass() const                               { return shaders.shaderOGLLightPass; }
    [[nodiscard]] GLuint getLastFrameBufferUsed() const                                           { return lastFrameBufferUsed; }
    [[nodiscard]] Object3D* getSelectedObject() const                                             { return selectedObject; }
    [[nodiscard]] GLuint getLastProgramUsed() const                                               { return lastProgramUsed; }
    [[nodiscard]] const std::map<std::string, ShaderCustomType> &getShaderTypesMapping() const    { return ShaderTypesMapping; }
    [[nodiscard]] ShaderBaseCustom *getSceneShaderByLabel(const std::string& name) const;
    static bool compareDistances(const Object3D *obj1, const Object3D *obj2);
    static void PostProcessingShadersChain();
    static void FillOGLBuffers(std::vector<Mesh3DData> &meshes);
    static void DeleteRemovedObjects();
    static void onUpdateSceneObjects();
    static void MakeScreenShot(std::string filename = "");
    static ShaderBaseCustom* CreateCustomShaderFromDisk(const ShaderBaseCustomMetaInfo &info, Mesh3D* o);

    void MoveSceneShaderUp(ShaderBaseCustom* shader);
    void MoveSceneShaderDown(ShaderBaseCustom* shader);
};

#endif //BRAKEDA3D_COMPONENTRENDER_H