//
// Created by darkhead on 9/1/20.
//

#ifndef BRAKEDA3D_COMPONENTBSP_H
#define BRAKEDA3D_COMPONENTBSP_H

#include <SDL.h>
#include <thread>
#include "Component.h"
#include "../Render/BSPMap.h"
#include "../Misc/cJSON.h"
#include "../../src/Physics/BSPCollider.h"

class ComponentBSP : public Component {
public:
    ComponentBSP();

    void onStart();
    void preUpdate();
    void onUpdate();
    void postUpdate();
    void onEnd();
    void onSDLPollEvent(SDL_Event *event, bool &finish);

    BSPMap      *bsp;
    BSPCollider *bspCollider;

    Camera3D    *camera;
    std::thread *BSPLoading;

    cJSON *mapsJSONList;
    cJSON *weaponsJSONList;
    cJSON *ammoTypesJSONList;
    cJSON *enemiesJSONList;

    BSPMap *getBSP() const;

    void initParallelBSP(const char *bspFilename, std::vector<Triangle*> *frameTriangles);
    void setCameraInBSPStartPosition();
    void loadMapsFromJSON();
    void loadWeaponsJSON();
    void loadEnemiesJSON();

    void setCamera(Camera3D *camera);

    BSPCollider *getBSPCollider() const;
};


#endif //BRAKEDA3D_COMPONENTBSP_H
