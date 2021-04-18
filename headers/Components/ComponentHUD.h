//
// Created by darkhead on 12/1/20.
//

#ifndef BRAKEDA3D_COMPONENTHUD_H
#define BRAKEDA3D_COMPONENTHUD_H

#include <SDL2/SDL_events.h>
#include "../Misc/TexturePackage.h"
#include "Component.h"
#include "ComponentWeapons.h"
#include "ComponentWindow.h"
#include "../GUI/GUIManager.h"
#include "../2D/TextWriter.h"

class ComponentHUD : public Component{
public:
    ComponentHUD();

    void onStart();
    void preUpdate();
    void onUpdate();
    void postUpdate();
    void onEnd();
    void onSDLPollEvent(SDL_Event *event, bool &finish);

    TexturePackage *HUDTextures;
    TextWriter *textureWriter;

    Counter *counterFaceAnimation;
    Counter *counterMinReactionTime;
    int currentFaceAnimationIndex = 0;
    std::vector<TextureAnimation *> faceAnimations;

    void loadImages();
    void loadStatusFaceImages();
    void setStatusFaceAnimation(int id);
    void writeText(int x, int y, const char *, bool bold);
    void writeTextCenter(const char *, bool bold);

    void drawHUD();

    enum StatusFace {
        EVIL = 0,
        KILL = 1,
        OUCH = 2,
        STAND = 3,
        DEAD = 4
    };

};


#endif //BRAKEDA3D_COMPONENTHUD_H
