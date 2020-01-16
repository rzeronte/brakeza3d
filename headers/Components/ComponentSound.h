//
// Created by darkhead on 9/1/20.
//

#ifndef BRAKEDA3D_COMPONENTSOUND_H
#define BRAKEDA3D_COMPONENTSOUND_H


#include "Component.h"

class ComponentSound : public Component {
public:
    ComponentSound();

    void onStart();
    void preUpdate();
    void onUpdate();
    void postUpdate();
    void onEnd();
    void onSDLPollEvent(SDL_Event *event, bool &finish);

    void initSoundSystem();
    void loadSoundsJSON();
};


#endif //BRAKEDA3D_COMPONENTSOUND_H
