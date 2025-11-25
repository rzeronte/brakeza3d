//
// Created by darkhead on 8/1/20.
//

#ifndef BRAKEDA3D_COMPONENTRENDER_H
#define BRAKEDA3D_COMPONENTRENDER_H

#include <vector>
#include "Component.h"
#include "../Objects/Triangle3D.h"
#include "../2D/TextWriter.h"
#include "../SceneLoader.h"
#include "../ProjectLoader.h"
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
#include "../OpenGL/ShaderOGLRenderDeferred.h"
#include "../OpenGL/ShaderOGLLightPass.h"
#include "../OpenGL/ShaderOGLShadowPass.h"
#include "../OpenGL/ShaderOGLShadowPassDebugLight.h"

class ComponentRender : public Component
{

    int fps = 0;
    int fpsFrameCounter = 0;
    float frameTime = 0.f;

    bool sceneShadersEnabled = false;
    GLuint lastFrameBufferUsed = 0;
    GLuint lastProgramUsed = 0;

    Object3D *selectedObject = nullptr;

    TextWriter *textWriter = nullptr;
    SceneLoader sceneLoader;
    ProjectLoader projectLoader;

    std::vector<ShaderOGLCustom*> sceneShaders;

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

    std::map<std::string, ShaderCustomTypes> ShaderTypesMapping = {
        {"Postprocessing", SHADER_POSTPROCESSING},
        {"Mesh3D", SHADER_OBJECT},
    };

public:
    ComponentRender();
    ~ComponentRender() override;

    void onStart() override;
    void preUpdate() override;
    void onUpdate() override;
    void postUpdate() override;
    void onEnd() override;
    void onSDLPollEvent(SDL_Event *event, bool &finish) override;

    [[nodiscard]] ShaderOGLLine3D *getShaderOGLLine3D() const;
    [[nodiscard]] ShaderOGLImage *getShaderOGLImage() const;
    [[nodiscard]] ShaderOGLRenderForward *getShaderOGLRenderForward() const;
    [[nodiscard]] ShaderOGLLine *getShaderOGLLine() const;
    [[nodiscard]] ShaderOGLWire *getShaderOGLWireframe() const;
    [[nodiscard]] ShaderOGLShading *getShaderOGLShading() const;
    [[nodiscard]] ShaderOGLPoints *getShaderOGLPoints() const;
    [[nodiscard]] ShaderOGLOutline *getShaderOGLOutline() const;
    [[nodiscard]] ShaderOGLColor *getShaderOGLColor() const;
    [[nodiscard]] ShaderOGLParticles *getShaderOGLParticles() const;
    [[nodiscard]] ShaderOGLDOF *getShaderOGLDOF() const;
    [[nodiscard]] ShaderOGLFog *getShaderOGLFOG() const;
    [[nodiscard]] ShaderOGLTint *getShaderOGLTint() const;
    [[nodiscard]] ShaderOGLBonesTransforms *getShaderOGLBonesTransforms() const;
    [[nodiscard]] ShaderOGLShadowPass *getShaderOGLShadowPass() const;
    [[nodiscard]] ShaderOGLShadowPassDebugLight *getShaderOGLShadowPassDebugLight() const;
    [[nodiscard]] ShaderOGLDepthMap *getShaderOGLDepthMap() const;
    [[nodiscard]] ShaderOGLRenderDeferred *getShaderOGLRenderDeferred() const;
    [[nodiscard]] ShaderOGLLightPass *getShaderOGLLightPass() const;
    [[nodiscard]] GLuint getLastFrameBufferUsed() const;
    [[nodiscard]] Object3D* getSelectedObject() const;
    [[nodiscard]] bool isSceneShadersEnabled() const;
    [[nodiscard]] int getFps() const;
    [[nodiscard]] GLuint getLastProgramUsed() const;
    [[nodiscard]] const std::map<std::string, ShaderCustomTypes> &getShaderTypesMapping() const;
    SceneLoader &getSceneLoader();
    ProjectLoader &getProjectLoader();
    std::vector<ShaderOGLCustom *> &getSceneShaders();
    ShaderOGLCustom *getSceneShaderByIndex(int i) const;
    ShaderOGLCustom *getSceneShaderByLabel(const std::string& name) const;
    void setSelectedObject(Object3D *o);
    void updateFPS();
    void updateSelectedObject3D();
    void loadShaderIntoScene(const std::string &folder, const std::string &name);
    void addShaderToScene(ShaderOGLCustom *shader);
    void setSceneShadersEnabled(bool value);
    void RunShadersOpenGLPostUpdate() const;
    void removeSceneShaderByIndex(int index);
    void removeSceneShader(const ShaderOGLCustom *);
    void RunShadersOpenGLPreUpdate() const;
    void setGlobalIlluminationDirection(const Vertex3D &d) const;
    void setGlobalIlluminationAmbient(const Vertex3D &a) const;
    void setGlobalIlluminationDiffuse(const Vertex3D &d) const;
    void setGlobalIlluminationSpecular(const Vertex3D &s) const;
    void drawLine(const Vertex3D &from, const Vertex3D &to, const Color &c) const;
    void setLastFrameBufferUsed(GLuint value);
    void setLastProgramUsed(GLuint value);
    void changeOpenGLFramebuffer(GLuint);
    void changeOpenGLProgram(GLuint);
    void resizeShadersFramebuffers() const;
    void ClearShadowMaps() const;
    void RenderLayersToGlobalFramebuffer() const;
    static bool compareDistances(const Object3D *obj1, const Object3D *obj2);
    static void FillOGLBuffers(std::vector<Mesh3DData> &meshes);
    static ShaderOGLCustom* getLoadedShader(const std::string &folder, const std::string &jsonFilename);
    static void deleteRemovedObjects();
    static void onUpdateSceneObjects();
    static Object3D *getObject3DFromClickPoint(int x, int y);
};

#endif //BRAKEDA3D_COMPONENTRENDER_H