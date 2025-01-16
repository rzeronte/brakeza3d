//
// Created by darkhead on 8/1/20.
//

#ifndef BRAKEDA3D_COMPONENTRENDER_H
#define BRAKEDA3D_COMPONENTRENDER_H

#include <vector>
#include <mutex>
#include "Component.h"
#include "../Objects/Triangle3D.h"
#include "../Render/Tile.h"
#include "../LUAManager.h"
#include "../../include/Render/Drawable.h"
#include "../../include/Physics/BillboardBody.h"
#include "../../include/Render/Maths.h"
#include "../2D/TextWriter.h"
#include "../Misc/SharedLUAContext.h"
#include "../Misc/SceneLoader.h"
#include "../OpenGL/ShaderOpenGLCustom.h"

class ComponentRender : public Component {
private:

    int fps;
    int fpsFrameCounter;
    float frameTime;

    std::string defaultGPU;

    std::vector<Tile> tiles;
    int sizeTileWidth;
    int sizeTileHeight;
    int numberTilesHorizontal;
    int numberTilesVertical;
    int numberTiles;
    int tilePixelsBufferSize;

    Object3D *selectedObject;

    EngineSetup::LuaStateScripts stateScripts;
    std::vector<ScriptLUA*> scripts;
    TextWriter *textWriter;
    SceneLoader sceneLoader;

    bool sceneShadersEnabled;
    std::vector<ShaderOpenGLCustom*> sceneShaders;

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

    void hiddenOctreeRemovalNode(OctreeNode *node, std::vector<Triangle *> &triangles);

    void initTiles();

    void updateFPS();

    [[nodiscard]] Object3D* getSelectedObject() const;

    void setSelectedObject(Object3D *o);

    static Object3D *getObject3DFromClickPoint(int xClick, int yClick, int &objectIndex);

    void updateSelectedObject3D();

    [[nodiscard]] int getFps() const;

    static void deleteRemovedObjects();

    EngineSetup::LuaStateScripts getStateLUAScripts();

    void playLUAScripts();

    void stopLUAScripts();

    void reloadLUAScripts();

    std::vector<ScriptLUA*> &getLUAScripts();

    void addLUAScript(ScriptLUA *script);

    LUADataValue getGlobalScriptVar(const char *scriptName, const char *varName);

    void reloadScriptGlobals();

    void removeScript(ScriptLUA *script);

    void onStartScripts();

    void runScripts();

    SceneLoader &getSceneLoader();

    std::vector<ShaderOpenGLCustom *> &getSceneShaders();

    void addShaderToScene(ShaderOpenGLCustom *shader);

    [[nodiscard]] bool isSceneShadersEnabled() const;

    void setSceneShadersEnabled(bool sceneShadersEnabled);

    void runShadersOpenCLPostUpdate();

    void removeShader(int index);

    void runShadersOpenCLPreUpdate();

    ShaderOpenGLCustom *getSceneShaderByIndex(int i);

    static bool compareDistances(Object3D *obj1, Object3D *obj2);

    void setGlobalIlluminationDirection(Vertex3D d);

    void setGlobalIlluminationAmbient(Vertex3D a);

    void setGlobalIlluminationDiffuse(Vertex3D d);

    void setGlobalIlluminationSpecular(Vertex3D s);
};


#endif //BRAKEDA3D_COMPONENTRENDER_H
