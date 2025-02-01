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
#include "../../include/Render/Drawable.h"
#include "../../include/Render/Maths.h"
#include "../2D/TextWriter.h"
#include "../Misc/SharedLUAContext.h"
#include "../Misc/SceneLoader.h"
#include "../OpenGL/ShaderOpenGLCustom.h"
#include "../Misc/ProjectLoader.h"

class ComponentRender : public Component {
private:

    int fps;
    int fpsFrameCounter;
    float frameTime;

    std::vector<Tile> tiles;
    int sizeTileWidth;
    int sizeTileHeight;
    int numberTilesHorizontal;
    int numberTilesVertical;
    int numberTiles;
    int tilePixelsBufferSize;

    Object3D *selectedObject;

    TextWriter *textWriter;
    SceneLoader sceneLoader;
    ProjectLoader projectLoader;

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

    static Object3D *getObject3DFromClickPoint(int xClick, int yClick);

    void updateSelectedObject3D();

    [[nodiscard]] int getFps() const;

    static void deleteRemovedObjects();


    SceneLoader &getSceneLoader();

    ProjectLoader &getProjectLoader();

    std::vector<ShaderOpenGLCustom *> &getSceneShaders();

    void addShaderToScene(ShaderOpenGLCustom *shader);

    [[nodiscard]] bool isSceneShadersEnabled() const;

    void setSceneShadersEnabled(bool sceneShadersEnabled);

    void runShadersOpenCLPostUpdate();

    void removeShader(int index);

    void runShadersOpenCLPreUpdate();

    ShaderOpenGLCustom *getSceneShaderByIndex(int i);

    static bool compareDistances(Object3D *obj1, Object3D *obj2);

    static void setGlobalIlluminationDirection(Vertex3D d);

    static void setGlobalIlluminationAmbient(Vertex3D a);

    static void setGlobalIlluminationDiffuse(Vertex3D d);

    static void setGlobalIlluminationSpecular(Vertex3D s);
};


#endif //BRAKEDA3D_COMPONENTRENDER_H
