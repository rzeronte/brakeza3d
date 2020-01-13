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

class ComponentBSP : public Component {
public:
    ComponentBSP(Camera3D *cam);

    void onStart();
    void preUpdate();
    void onUpdate();
    void postUpdate();
    void onEnd();
    void onSDLPollEvent(SDL_Event *event, bool &finish);

    BSPMap      *bsp;
    std::thread *loadingBSP;
    Camera3D    *camera;

    cJSON *mapsJSONList;
    cJSON *weaponsJSONList;
    cJSON *ammoTypesJSONList;
    cJSON *enemiesJSONList;


    BSPMap *getBsp() const;
    void initBSP(const char *bspFilename, std::vector<Triangle*> *frameTriangles);
    void setCameraInBSPStartPosition();
    void loadMapsFromJSON();
    void loadWeaponsJSON();
    void loadEnemiesJSON();

};


#endif //BRAKEDA3D_COMPONENTBSP_H
