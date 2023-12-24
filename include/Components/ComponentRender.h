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
#include "../EngineBuffers.h"
#include "../../include/Render/Drawable.h"
#include "../../include/Physics/BillboardBody.h"
#include "../../include/Render/Maths.h"
#include "../Shaders/ShaderBilinear.h"
#include "../Shaders/ShaderDepthOfField.h"
#include "../../src/Shaders/ShaderParticlesBlurBuffer.h"
#include "../2D/TextWriter.h"
#include "../Misc/SharedLUAContext.h"
#include "../Misc/SceneLoader.h"
#include "../Shaders/ShaderImage.h"
#include <CL/cl.h>

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
    std::vector<ShaderOpenCL*> sceneShaders;

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

    std::vector<LightPoint3D *> &getLightPoints();

    void initTiles();

    void updateFPS();

    [[nodiscard]] Object3D* getSelectedObject() const;

    void setSelectedObject(Object3D *o);

    static Object3D *getObject3DFromClickPoint(int xClick, int yClick, int &objectIndex);

    void updateSelectedObject3D();

    std::vector<Tile> &getTiles();

    [[nodiscard]] int getTilesWidth() const;

    [[nodiscard]] int getTilesHeight() const;

    [[nodiscard]] int getNumTiles() const;

    [[nodiscard]] int getFps() const;

    void deleteRemovedObjects();

    void loadConfig();

    EngineSetup::LuaStateScripts getStateLUAScripts();

    void playLUAScripts();

    void stopLUAScripts();

    void reloadLUAScripts();

    std::vector<ScriptLUA*> &getLUAScripts();

    void addLUAScript(ScriptLUA *script);

    void reloadScriptGlobals();

    void removeScript(ScriptLUA *script);

    void onStartScripts();

    void runScripts();

    SceneLoader &getSceneLoader();

    std::vector<ShaderOpenCL *> &getSceneShaders();

    void addShaderToScene(ShaderOpenCL *shader);

    [[nodiscard]] bool isSceneShadersEnabled() const;

    void setSceneShadersEnabled(bool sceneShadersEnabled);

    void runShadersOpenCLPostUpdate();

    void removeShader(int index);

    void runShadersOpenCLPreUpdate();

    ShaderOpenCL *getSceneShaderByIndex(int i);

};


#endif //BRAKEDA3D_COMPONENTRENDER_H
