#ifndef BRAKEDA3D_COMPONENTBSP_H
#define BRAKEDA3D_COMPONENTBSP_H

#include <SDL2/SDL.h>
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

    BSPMap *bsp;
    BSPCollider *bspCollider{};

    Camera3D *camera{};
    std::thread *BSPLoading{};

    BSPMap *getBSP() const;

    void initMap(const char *bspFilename);

    void setCameraInBSPStartPosition() const;

    void setCamera(Camera3D *newCamera);

    BSPCollider *getBSPCollider() const;
};


#endif //BRAKEDA3D_COMPONENTBSP_H
