//
// Created by darkhead on 8/1/20.
//

#ifndef BRAKEDA3D_COMPONENTRENDER_H
#define BRAKEDA3D_COMPONENTRENDER_H

#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include "Component.h"
#include "../Render/Image.h"
#include "../Render/Triangle3D.h"
#include "../Render/TextWriter.h"
#include "../Render/GlyphAtlas.h"
#include "../Render/SelectionManager.h"
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
#include "../OpenGL/Quad/ShaderOGLDepthMap.h"
#include "../OpenGL/Quad/ShaderOGLTint.h"
#include "../OpenGL/ShaderOGLLine3D.h"
#include "../OpenGL/ShaderOGLBonesTransforms.h"
#include "../OpenGL/Quad/ShaderOGLGrid.h"
#include "../OpenGL/ShaderOGLGroundCircle.h"
#include "../OpenGL/ShaderOGLGroundDecal.h"
#include "../OpenGL/ShaderOGLAxisQuad.h"
#include "../OpenGL/ShaderOGLRect.h"
#include "../OpenGL/ShaderOGLCircle2D.h"
#include "../OpenGL/ShaderOGLRenderDeferred.h"
#include "../OpenGL/Quad/ShaderOGLLightPass.h"
#include "../OpenGL/ShaderOGLShadowPass.h"
#include "../OpenGL/Quad/ShaderOGLShadowPassDebugLight.h"
#include "../OpenGL/ShaderOGLComputeParticles.h"
#include "../OpenGL/ShaderOGLGPUParticles.h"
#include "../Render/UI/UIManager.h"

struct FrustumPlane {
    float nx, ny, nz, d;
};

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
    ShaderOGLDepthMap *shaderOGLDepthMap = nullptr;
    ShaderOGLBonesTransforms *shaderOGLBonesTransforms = nullptr;
    ShaderOGLRenderDeferred *shaderOGLGBuffer = nullptr;
    ShaderOGLLightPass *shaderOGLLightPass = nullptr;
    ShaderOGLShadowPass *shaderShadowPass = nullptr;
    ShaderOGLShadowPassDebugLight *shaderShadowPassDebugLight = nullptr;
    ShaderOGLGrid *shaderOGLGrid = nullptr;
    ShaderOGLGroundCircle        *shaderGroundCircle      = nullptr;
    ShaderOGLGroundDecal         *shaderGroundDecal       = nullptr;
    ShaderOGLAxisQuad            *shaderAxisQuad          = nullptr;
    ShaderOGLRect                *shaderOGLRect           = nullptr;
    ShaderOGLComputeParticles    *shaderComputeParticles  = nullptr;
    ShaderOGLGPUParticles        *shaderGPUParticles      = nullptr;
    ShaderOGLCircle2D            *shaderCircle2D          = nullptr;
};

class ComponentRender : public Component
{
    int fps = 0;
    int fpsFrameCounter = 0;
    float frameTime = 0.f;
    inline static float sortFrameTime = 0.f;

    GLuint lastFrameBufferUsed = 0;
    GLuint lastProgramUsed = 0;

    SelectionManager selection;

    TextWriter *textWriter = nullptr;
    GlyphAtlas *glyphAtlas = nullptr;
    SceneLoader sceneLoader;
    ProjectLoader projectLoader;
    std::map<std::string, ShaderCustomType> ShaderTypesMapping = {
        {"Postprocessing", SHADER_POSTPROCESSING},
        {"Mesh3D", SHADER_OBJECT},
        {"NodeMesh3D", SHADER_NODE_OBJECT},
        {"NodePostProcessing", SHADER_NODE_POSTPROCESSING},
    };

    std::vector<ShaderBaseCustom*> sceneShaders;
    std::unordered_map<unsigned int, std::pair<Mesh3D*, std::string>> submeshRegistry;

    Shaders shaders;

    UIManager* uiManager{nullptr};
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
    void UpdateFPS();
    void setSelectedObject(Object3D *o);
    void addToSelection(Object3D *o);
    void removeFromSelection(const Object3D *o);
    void clearSelection();
    void DrawSelectionBox() const;
    void DrawSelectionRectFill() const;
    void clearRightClickedObject();
    void clearLeftClickedObject();
    void registerSubmesh(unsigned int id, Mesh3D *mesh, const std::string &name);
    void unregisterSubmeshes(Mesh3D *mesh);
    ShaderBaseCustom* LoadShaderIntoScene(const std::string &name);
    void AddShaderToScene(ShaderBaseCustom *shader);
    void RemoveSceneShaderByIndex(int index);
    void RemoveSceneShader(const ShaderBaseCustom *);
    void clearSceneShaders();
    void setGlobalIlluminationDirection(Vertex3D d) const;
    void setGlobalIlluminationAmbient(Vertex3D a) const;
    void setGlobalIlluminationDiffuse(Vertex3D d) const;
    void setGlobalIlluminationSpecular(Vertex3D s) const;
    [[nodiscard]] Vertex3D getGlobalIlluminationDirection() const;
    [[nodiscard]] Vertex3D getGlobalIlluminationAmbient() const;
    [[nodiscard]] Vertex3D getGlobalIlluminationDiffuse() const;
    [[nodiscard]] Vertex3D getGlobalIlluminationSpecular() const;
    [[nodiscard]] Object3D* getSelectedObject() const;
    [[nodiscard]] const std::vector<Object3D*>& getSelectedObjects() const;
    [[nodiscard]] bool isObjectInSelection(const Object3D *o) const;
    [[nodiscard]] bool hasMultipleSelected() const;
    [[nodiscard]] Object3D* getLastRightClickedObject() const;
    [[nodiscard]] std::string getLastRightClickedSubmeshName() const;
    [[nodiscard]] Object3D* getLastLeftClickedObject() const;
    [[nodiscard]] std::string getLastLeftClickedSubmeshName() const;
    [[nodiscard]] std::pair<Mesh3D*, std::string> getSubmeshEntry(unsigned int id) const;
    [[nodiscard]] ShaderBaseCustom *getSceneShaderByLabel(const std::string& name) const;
    void DrawLine(const Vertex3D &from, const Vertex3D &to, const Color &c) const;
    void DrawLine2D(int x1, int y1, int x2, int y2, const Color &c, float weight) const;
    void DrawFilledRect(int x, int y, int w, int h, const Color &c) const;
    void DrawFilledRectToFB(int x, int y, int w, int h, const Color &c, const std::string &fb) const;
    void DrawImage2D(const std::string &path, int x, int y, int w, int h);
    void DrawImage2DToFB(const std::string &path, int x, int y, int w, int h, const std::string &fb);
    void DrawImage2DFromImage(Image *img, int x, int y, int w, int h) const;
    [[nodiscard]] GLuint getImageGLTexture(const std::string& path);
    void drawGroundCircle(Object3D* obj, float r, float g, float b, float a, float radius) const;
    void drawGroundCircleToFB(Object3D* obj, float r, float g, float b, float a, float radius, const std::string& fb) const;
    void drawGroundBlob(Object3D* obj, float r, float g, float b, float a, float radius) const;
    void drawGroundBlobToFB(Object3D* obj, float r, float g, float b, float a, float radius, const std::string& fb) const;
    void drawOutlineSubmesh(Object3D* obj, const std::string& submeshName, float r, float g, float b, float a, float thickness) const;
    void clearOutlineBatch() const;
    void drawOutlineSubmeshBatch(Object3D* obj, const std::string& submeshName, float r, float g, float b, float a, float thickness) const;
    void flushOutlines() const;
    [[nodiscard]] Vertex3D getSubmeshCenter(Object3D* obj, const std::string& submeshName) const;
    void drawGroundDecal(Object3D* obj, const std::string& texturePath, float r, float g, float b, float a, float radius) const;
    void drawGroundDecalToFB(Object3D* obj, const std::string& texturePath, float r, float g, float b, float a, float radius, const std::string& fb) const;
    void drawAxisQuad(Object3D* obj, float r, float g, float b, float a, float halfSize, ShaderOGLAxisQuad::Axis axis = ShaderOGLAxisQuad::AXIS_Y) const;
    void drawAxisQuadAt(const Vertex3D& pos, float r, float g, float b, float a, float halfSize, ShaderOGLAxisQuad::Axis axis = ShaderOGLAxisQuad::AXIS_Y) const;
    void DrawCircle3D(Vertex3D center, float radius, float r, float g, float b, float a) const;
    void DrawCircle2D(int x, int y, int size, float r, float g, float b, float a, float numWaves, float speed, float thickness, bool additive = false) const;
    void DrawCircle2DToFB(int x, int y, int size, float r, float g, float b, float a, float numWaves, float speed, float thickness, bool additive, const std::string &fb) const;
    void setLastFrameBufferUsed(GLuint value);
    void setLastProgramUsed(GLuint value);
    void ChangeOpenGLFramebuffer(GLuint);
    void ChangeOpenGLProgram(GLuint);
    void resizeShadersFramebuffers() const;
    void ClearShadowMaps() const;
    void LightPass() const;
    void RunShadowPass() const;
    void FlipBuffersToGlobal() const;
    void MoveSceneShaderUp(ShaderBaseCustom* shader);
    void MoveSceneShaderDown(ShaderBaseCustom* shader);

    void RenderAvatars();
    static bool isAvatarTypeEnabled(ObjectType type);
    [[nodiscard]] Object3D* hitTestAvatar(int screenX, int screenY) const;

    [[nodiscard]] SceneLoader& getSceneLoader()                                                   { return sceneLoader; }
    [[nodiscard]] ProjectLoader& getProjectLoader()                                               { return projectLoader; }
    [[nodiscard]] std::vector<ShaderBaseCustom*>& getSceneShaders()                               { return sceneShaders; }
    [[nodiscard]] Shaders* getShaders()                                                           { return &shaders; }
    [[nodiscard]] ShaderBaseCustom* getSceneShaderByIndex(int i) const                            { return sceneShaders[i]; }
    [[nodiscard]] int getFps() const                                                              { return fps; }
    [[nodiscard]] ShaderOGLDepthMap* getShaderOGLDepthMap() const                                 { return shaders.shaderOGLDepthMap; }
    [[nodiscard]] ShaderOGLRenderDeferred* getShaderOGLRenderDeferred() const                     { return shaders.shaderOGLGBuffer; }
    [[nodiscard]] ShaderOGLLightPass* getShaderOGLLightPass() const                               { return shaders.shaderOGLLightPass; }
    [[nodiscard]] GLuint getLastFrameBufferUsed() const                                           { return lastFrameBufferUsed; }
    [[nodiscard]] GLuint getLastProgramUsed() const                                               { return lastProgramUsed; }
    [[nodiscard]] const std::map<std::string, ShaderCustomType>& getShaderTypesMapping() const    { return ShaderTypesMapping; }
    [[nodiscard]] SelectionManager& getSelectionManager()                                         { return selection; }
    [[nodiscard]] TextWriter* getTextWriter() const                                               { return textWriter; }
    [[nodiscard]] UIManager* getUIManager() const                                                 { return uiManager; }
    [[nodiscard]] GlyphAtlas* getGlyphAtlas() const                                               { return glyphAtlas; }
    static int getLastFrameVisible()                                                              { return lastFrameVisible; }
    static int getLastFrameCulled()                                                               { return lastFrameCulled; }
    static int getLastFrameLightsVisible()                                                        { return lastFrameLightsVisible; }
    static int getLastFrameLightsCulled()                                                         { return lastFrameLightsCulled; }
    static void setLastFrameLightsVisible(int v)                                                  { lastFrameLightsVisible = v; }
    static void setLastFrameLightsCulled(int v)                                                   { lastFrameLightsCulled = v; }

    static bool compareDistances(const Object3D *obj1, const Object3D *obj2);
    static void PostProcessingShadersChain();
    static void FillOGLBuffers(std::vector<Mesh3DData> &meshes, bool withFeedbackBuffers = false);
    static void DeleteRemovedObjects();
    static void onUpdateSceneObjects();
    static void updateFrustum();
    static bool isInFrustum(const Object3D *o, float radiusOverride = -1.0f);
    static void MakeScreenShot(std::string filename = "");
    static ShaderBaseCustom* CreateCustomShaderFromDisk(const ShaderBaseCustomMetaInfo &info, Mesh3D* o);

    inline static FrustumPlane frustumPlanes[6] = {};
    inline static int lastFrameVisible = 0;
    inline static int lastFrameCulled  = 0;
    inline static int lastFrameLightsVisible = 0;
    inline static int lastFrameLightsCulled  = 0;

    void clearEngineCache();
};

#endif //BRAKEDA3D_COMPONENTRENDER_H