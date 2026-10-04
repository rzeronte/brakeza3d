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
#include "../Render/RenderQueueEntry.h"
#include "../Render/PickingQueueEntry.h"
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

    // Caché de estado GL compartida SOLO entre ShaderOGLRenderDeferred/Forward (los dos que
    // dibujan por submesh, cientos/miles de veces por frame -- ver Fase 1.1.1 del plan de
    // rendimiento). rsValid=false fuerza el próximo Apply* a emitir la llamada GL real sin
    // fiarse del valor cacheado -- se pone a false en cada frontera real (antes del bucle de
    // GBuffer, antes del de transparencias) para no heredar estado de otro paso/frame.
    bool rsValid     = false;
    bool rsDepthTest = true;
    bool rsDepthMask = true;
    bool rsBlend     = true;
    bool rsCull      = true;
    GLenum rsDepthFunc = GL_LESS;
    GLenum rsBlendSrc  = GL_SRC_ALPHA;
    GLenum rsBlendDst  = GL_ONE_MINUS_SRC_ALPHA;

    // Fase 3 (Etapa 1): cola de opacos, solo Mesh3D estático/Deferred por ahora. Se vacía al
    // principio de la Pasada 2 de onUpdateSceneObjects() y se consume (ordenada) al final de esa
    // misma pasada -- nunca sobrevive entre frames, no hace falta limpiarla en ningún otro sitio.
    std::vector<RenderQueueEntry> opaqueQueue;

    // Mesh3D con emisión activa: se dibujan en el G-Buffer DESPUÉS de todo lo demás (opacos y
    // custom shaders de objeto), con el 4º draw buffer (gBuffer.emission) activado solo para ellos.
    // Al ser los últimos, el depth test garantiza que solo escriben emisión donde son lo más
    // cercano -- nadie puede taparlos después dejando emisión obsoleta en ese píxel.
    std::vector<RenderQueueEntry> emissiveQueue;
    bool emissionUsedThisFrame = false;   // lo lee LightPass() para saltarse la lectura de gEmission

    // Fase 4b: misma idea que opaqueQueue pero para el pase de picking (ShaderOGLColor). Mismo
    // ciclo de vida -- vive solo dentro de un frame.
    std::vector<PickingQueueEntry> pickingQueue;

    // Mesh3D con custom shaders (p.ej. WaterRTS) deben escribir su G-Buffer DESPUES del pase
    // opaco normal, no entrelazados objeto-a-objeto -- si no, un objeto que se procesa antes que
    // otros en sceneObjects pinta su shader custom y luego FlushOpaqueQueue() (al final de la
    // pasada) lo repinta encima con el material por defecto sin animar. Mismo ciclo de vida que
    // opaqueQueue/pickingQueue -- vive solo dentro de un frame.
    std::vector<Mesh3D*> objectShaderQueue;

    // CameraBlock UBO (binding point 3; 0-2 los usan los UBOs de luces de ShaderOGLRenderForward).
    // Compartido por GBuffer/Render/Color: se rellena UNA vez por frame en vez de subir
    // projection/view como uniforms sueltos en cada draw.
    GLuint cameraUBO = 0;

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
    std::unordered_map<std::string, Image*> renderImageCache;

    Shaders shaders;

    UIManager* uiManager{nullptr};
public:
    ComponentRender() = default;
    ~ComponentRender() override;

    void onStart() override;
    void preUpdate() override;
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
    void DrawWidgetCacheToFB(GLuint tex, int rW, int rH, const std::string& fb, float alpha = 1.0f) const;
    // w/h: size of the override FBO (a widget cache), so the 2D draw helpers map onto it; 0 = the
    // size of the layer that was asked for
    static void setFBOOverride(GLuint fbo, int w = 0, int h = 0);
    static void clearFBOOverride();
    // Real pixel size of a 2D draw target: "ui" is WINDOW sized (ImGui draws there too), the other
    // layers are RENDER sized; an active FBO override reports its own size. The 2D helpers map
    // window-px coordinates onto it (before, they always assumed render size: with a render
    // resolution different from the window, the RTS UI ended squeezed in a corner).
    static void getTargetSize(const std::string& fb, int& w, int& h);
    // Sets glViewport to (w, h) for a draw into a target whose size differs from the render
    // resolution and puts the render-size viewport back on destruction (the rest of the frame
    // assumes it). No-op when the target is render sized -- the usual case.
    struct ScopedTargetViewport {
        bool changed{false};
        int rw{0}, rh{0};
        ScopedTargetViewport(int w, int h);
        ~ScopedTargetViewport();
        ScopedTargetViewport(const ScopedTargetViewport&) = delete;
        ScopedTargetViewport& operator=(const ScopedTargetViewport&) = delete;
    };
    static GLuint resolveEffectiveFBO(const std::string& fb);
    void DrawImage2D(const std::string &path, int x, int y, int w, int h);
    void DrawImage2DToFB(const std::string &path, int x, int y, int w, int h, const std::string &fb, float alpha = 1.0f);
    void DrawImage2DFromImageToFB(Image *img, int x, int y, int w, int h, const std::string &fb, float alpha = 1.0f);
    // 9-slice: corners keep their size, edges stretch along one axis, center stretches both.
    // (x,y,w,h) in window px; slice L/T/R/B in IMAGE px; sliceScale = screen (window) px per image px.
    void DrawImage2DNineSliceToFB(Image *img, float x, float y, float w, float h,
                                  const float slice[4], float sliceScale, const std::string &fb, float alpha = 1.0f);
    void DrawImage2DFromImage(Image *img, int x, int y, int w, int h) const;
    Image* getOrLoadImage(const std::string &path);
    [[nodiscard]] GLuint getImageGLTexture(const std::string& path);
    void drawGroundCircle(Object3D* obj, float r, float g, float b, float a, float radius) const;
    void drawGroundCircle(Object3D* obj, float r, float g, float b, float a, float radius, float thickness) const;
    void drawGroundCircleToFB(Object3D* obj, float r, float g, float b, float a, float radius, const std::string& fb) const;
    void drawGroundBlob(Object3D* obj, float r, float g, float b, float a, float radius) const;
    void drawGroundBlobToFB(Object3D* obj, float r, float g, float b, float a, float radius, const std::string& fb) const;
    void drawOutlineSubmesh(Object3D* obj, const std::string& submeshName, float r, float g, float b, float a, float thickness) const;
    // Tinte de color translúcido (a = opacidad del tinte) sobre un submesh, en la capa foreground.
    void drawFillSubmesh(Object3D* obj, const std::string& submeshName, float r, float g, float b, float a) const;
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
    void ApplyDepthTest(bool value);
    void ApplyDepthFunc(GLenum value);
    void ApplyDepthMask(bool value);
    void ApplyBlend(bool value);
    void ApplyBlendFunc(GLenum src, GLenum dst);
    void ApplyCulling(bool value);
    void InvalidateRenderStateCache();
    void RestoreDefaultRenderState();
    void EnqueueOpaque(Mesh3D *o, bool useFeedbackBuffer, GLuint fbo);
    void FlushOpaqueQueue();
    void FlushEmissiveQueue();
    void DrawRenderQueue(std::vector<RenderQueueEntry> &queue);
    void EnqueuePicking(Mesh3D *o, bool useFeedbackBuffer, GLuint fbo);
    void FlushPickingQueue();
    void EnqueueObjectShaders(Mesh3D *o);
    void FlushObjectShaderQueue();
    void CreateCameraUBO();
    void UpdateCameraUBO() const;
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
    static void BuildDedupedStaticGeometry(
        const std::vector<glm::vec4> &vertices,
        const std::vector<glm::vec3> &normals,
        const std::vector<glm::vec2> &uvs,
        GLuint &outVertexBuffer,
        GLuint &outUvBuffer,
        GLuint &outNormalBuffer,
        GLuint &outIndexBuffer,
        GLsizei &outIndexCount
    );
    static void DeleteRemovedObjects();
    static void onUpdateSceneObjects(std::vector<Object3D*> &sceneObjects);
    static void updateFrustum();
    static bool isInFrustum(const Object3D *o, float radiusOverride = -1.0f);
    static void MakeScreenShot(std::string filename = "");
    static ShaderBaseCustom* CreateCustomShaderFromDisk(const ShaderBaseCustomMetaInfo &info, Mesh3D* o);

    inline static FrustumPlane frustumPlanes[6] = {};
    inline static int lastFrameVisible = 0;
    inline static int lastFrameCulled  = 0;
    inline static int lastFrameLightsVisible = 0;
    inline static int lastFrameLightsCulled  = 0;

    // Objetos marcados removed=true en un frame anterior: sacados ya de sceneObjects/index,
    // pero con el delete real diferido un frame (ver DeleteRemovedObjects). Deja una ventana
    // completa de onUpdate() para que cualquier otro objeto con un puntero crudo hacia este
    // (p.ej. ParticleEmitter::followTarget) pueda ver isRemoved()==true en memoria todavía
    // válida y soltar su referencia antes del delete.
    inline static std::vector<Object3D*> pendingDeleteObjects;

    void clearEngineCache();
};

#endif //BRAKEDA3D_COMPONENTRENDER_H