//
// Created by darkhead on 8/1/20.
//

#ifndef BRAKEDA3D_COMPONENTRENDER_H
#define BRAKEDA3D_COMPONENTRENDER_H


#include <vector>
#include "Component.h"
#include "../Objects/Triangle3D.h"
#include "../Render/Tile.h"
#include "../EngineBuffers.h"
#include "../../headers/Render/Drawable.h"
#include "../../headers/Physics/BillboardBody.h"
#include "../../headers/Render/Maths.h"
#include "ComponentBSP.h"
#include "ComponentWindow.h"
#include "ComponentCollisions.h"
#include "ComponentCamera.h"

struct Fragment {
    float alpha, theta, gamma, depth, affineUV, texU, texV, lightU, lightV;
};

class ComponentRender : public Component {
public:

    ComponentRender();

    void onStart();
    void preUpdate();
    void onUpdate();
    void postUpdate();
    void onEnd();
    void onSDLPollEvent(SDL_Event *event, bool &finish);

    void onUpdateBSP();
    void onUpdateSceneObjects();
    void hiddenSurfaceRemoval();
    void hiddenSurfaceRemovalTriangle(Triangle *t);

    void drawVisibleTriangles();

    void handleTrianglesToTiles(std::vector<Triangle*> &visibleTriangles);

    void drawTilesGrid();
    void drawTriangles(std::vector<Triangle*> &visibleTriangles);
    void processTriangle(Triangle *t);
    void triangleRasterizer(Triangle *t);
    void processPixel(Triangle *t, int bufferIndex, const int x, const int y, Fragment *, bool bilinear);
    void drawTilesTriangles(std::vector<Triangle*> *visibleTriangles);
    void drawSceneObjectsAxis();

    void initTiles();
    void drawTileTriangles(int i, std::vector<Triangle*> &visibleTriangles);
    void softwareRasterizerForTile(Triangle *t, int minTileX, int minTileY, int maxTileX, int maxTileY);
    void drawWireframe(Triangle *t);

    std::vector<Triangle *> &getFrameTriangles();
    std::vector<Triangle *> &getVisibleTriangles();
    std::vector<LightPoint3D *> &getLightPoints();

    std::mutex lockFrameTriangles;
    std::vector<Triangle*> frameTriangles;
    std::vector<Triangle*> clippedTriangles;
    std::vector<Triangle*> visibleTriangles;
    std::vector<LightPoint3D*> lightpoints;

    std::vector<Tile> tiles;
    int sizeTileWidth  = (EngineSetup::getInstance()->screenWidth/2);
    int sizeTileHeight = (EngineSetup::getInstance()->screenHeight/2);
    int tilesWidth;
    int tilesHeight;
    int numTiles;
    int tilePixelsBufferSize;
};


#endif //BRAKEDA3D_COMPONENTRENDER_H
