//
// Created by darkhead on 8/1/20.
//

#ifndef BRAKEDA3D_COMPONENTRENDER_H
#define BRAKEDA3D_COMPONENTRENDER_H

#include <vector>
#include "Component.h"
#include "../Objects/Triangle3D.h"
#include "../2D/TextWriter.h"
#include "../Misc/SceneLoader.h"
#include "../Misc/ProjectLoader.h"
#include "../OpenGL/ShaderOpenGLImage.h"
#include "../OpenGL/ShaderOGLRenderForward.h"
#include "../OpenGL/ShaderOpenGLLine.h"
#include "../OpenGL/ShaderOpenGLWireframe.h"
#include "../OpenGL/ShaderOpenGLShading.h"
#include "../OpenGL/ShaderOpenGLPoints.h"
#include "../OpenGL/ShaderOpenGLOutline.h"
#include "../OpenGL/ShaderOpenGLColor.h"
#include "../OpenGL/ShaderOpenGLParticles.h"
#include "../OpenGL/ShaderOpenGLDOF.h"
#include "../OpenGL/ShaderOpenGLDepthMap.h"
#include "../OpenGL/ShaderOpenGLFOG.h"
#include "../OpenGL/ShaderOpenGLTint.h"
#include "../OpenGL/ShaderOpenGLLine3D.h"
#include "../OpenGL/ShaderOpenGLBonesTransforms.h"
#include "../OpenGL/ShaderOGLGRenderDeferred.h"
#include "../OpenGL/ShaderOGLLightPass.h"
#include "../OpenGL/ShaderOGLShadowPass.h"
#include "../OpenGL/ShaderOGLShadowPassDebugLight.h"

class ComponentRender : public Component {

    int fps = 0;
    int fpsFrameCounter = 0;
    float frameTime = 0.f;

    Object3D *selectedObject = nullptr;

    TextWriter *textWriter = nullptr;
    SceneLoader sceneLoader;
    ProjectLoader projectLoader;

    bool sceneShadersEnabled = false;
    std::vector<ShaderOpenGLCustom*> sceneShaders;

    ShaderOGLRenderForward *shaderOGLRender = nullptr;
    ShaderOpenGLImage *shaderOGLImage = nullptr;
    ShaderOpenGLLine *shaderOGLLine = nullptr;
    ShaderOpenGLWireframe *shaderOGLWireframe = nullptr;
    ShaderOpenGLLine3D *shaderOGLLine3D = nullptr;
    ShaderOpenGLShading *shaderOGLShading = nullptr;
    ShaderOpenGLPoints *shaderOGLPoints = nullptr;
    ShaderOpenGLOutline *shaderOGLOutline = nullptr;
    ShaderOpenGLColor *shaderOGLColor = nullptr;
    ShaderOpenGLParticles *shaderOGLParticles = nullptr;
    ShaderOpenGLDOF *shaderOGLDOFBlur = nullptr;
    ShaderOpenGLDepthMap *shaderOGLDepthMap = nullptr;
    ShaderOpenGLFOG *shaderOGLFOG = nullptr;
    ShaderOpenGLTint *shaderOGLTint = nullptr;
    ShaderOpenGLBonesTransforms *shaderOGLBonesTransforms = nullptr;
    ShaderOGLGRenderDeferred *shaderOGLGBuffer = nullptr;
    ShaderOGLLightPass *shaderOGLLightPass = nullptr;
    ShaderOGLShadowPass *shaderShadowPass = nullptr;
    ShaderOGLShadowPassDebugLight *shaderShadowPassDebugLight = nullptr;

    GLuint lastFrameBufferUsed = 0;
    GLuint lastProgramUsed = 0;

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

    static void onUpdateSceneObjects();

    void updateFPS();

    [[nodiscard]] Object3D* getSelectedObject() const;

    void setSelectedObject(Object3D *o);

    static Object3D *getObject3DFromClickPoint(int x, int y);

    void updateSelectedObject3D();

    [[nodiscard]] int getFps() const;

    static void deleteRemovedObjects();

    SceneLoader &getSceneLoader();

    ProjectLoader &getProjectLoader();

    std::vector<ShaderOpenGLCustom *> &getSceneShaders();

    void loadShaderIntoScene(const std::string &folder, const std::string &name);

    static ShaderOpenGLCustom* getLoadedShader(const std::string &folder, const std::string &jsonFilename);

    void addShaderToScene(ShaderOpenGLCustom *shader);

    [[nodiscard]] bool isSceneShadersEnabled() const;

    void setSceneShadersEnabled(bool value);

    void RunShadersOpenGLPostUpdate() const;

    void removeSceneShaderByIndex(int index);
    void removeSceneShader(const ShaderOpenGLCustom *);

    void RunShadersOpenGLPreUpdate() const;

    ShaderOpenGLCustom *getSceneShaderByIndex(int i) const;

    ShaderOpenGLCustom *getSceneShaderByLabel(const std::string& name) const;

    static bool compareDistances(const Object3D *obj1, const Object3D *obj2);

    void setGlobalIlluminationDirection(const Vertex3D &d) const;

    void setGlobalIlluminationAmbient(const Vertex3D &a) const;

    void setGlobalIlluminationDiffuse(const Vertex3D &d) const;

    void setGlobalIlluminationSpecular(const Vertex3D &s) const;

    void drawLine(const Vertex3D &from, const Vertex3D &to, const Color &c) const;

    [[nodiscard]] ShaderOpenGLLine3D *getShaderOGLLine3D() const;

    [[nodiscard]] ShaderOpenGLImage *getShaderOGLImage() const;

    [[nodiscard]] ShaderOGLRenderForward *getShaderOGLRenderForward() const;

    [[nodiscard]] ShaderOpenGLLine *getShaderOGLLine() const;

    [[nodiscard]] ShaderOpenGLWireframe *getShaderOGLWireframe() const;

    [[nodiscard]] ShaderOpenGLShading *getShaderOGLShading() const;

    [[nodiscard]] ShaderOpenGLPoints *getShaderOGLPoints() const;

    [[nodiscard]] ShaderOpenGLOutline *getShaderOGLOutline() const;

    [[nodiscard]] ShaderOpenGLColor *getShaderOGLColor() const;

    [[nodiscard]] ShaderOpenGLParticles *getShaderOGLParticles() const;

    [[nodiscard]] ShaderOpenGLDOF *getShaderOGLDOF() const;

    [[nodiscard]] ShaderOpenGLFOG *getShaderOGLFOG() const;

    [[nodiscard]] ShaderOpenGLTint *getShaderOGLTint() const;

    [[nodiscard]] ShaderOpenGLBonesTransforms *getShaderOGLBonesTransforms() const;

    [[nodiscard]] ShaderOGLShadowPass *getShaderOGLShadowPass() const;

    [[nodiscard]] ShaderOGLShadowPassDebugLight *getShaderOGLShadowPassDebugLight() const;

    [[nodiscard]] ShaderOpenGLDepthMap *getShaderOGLDepthMap() const;

    [[nodiscard]] ShaderOGLGRenderDeferred *getShaderOGLRenderDeferred() const;

    [[nodiscard]] ShaderOGLLightPass *getShaderOGLLightPass() const;

    [[nodiscard]] GLuint getLastFrameBufferUsed() const;

    void setLastFrameBufferUsed(GLuint value);

    [[nodiscard]] GLuint getLastProgramUsed() const;

    void setLastProgramUsed(GLuint value);

    void changeOpenGLFramebuffer(GLuint);

    void changeOpenGLProgram(GLuint);

    [[nodiscard]] const std::map<std::string, ShaderCustomTypes> &getShaderTypesMapping() const;

    void resizeShadersFramebuffers() const;

    static void FillOGLBuffers(std::vector<meshData> &meshes);

    void clearShadowMaps() const;

    void RenderLayersToGlobalFramebuffer() const;
};

#endif //BRAKEDA3D_COMPONENTRENDER_H