//
// Created by darkhead on 14/1/20.
//

#ifndef BRAKEDA3D_COMPONENTGAME_H
#define BRAKEDA3D_COMPONENTGAME_H


#include "Component.h"
#include "../../src/Game/Player.h"

class ComponentGame : public Component {
public:
    ComponentGame();

    void onStart();
    void preUpdate();
    void onUpdate();
    void postUpdate();
    void onEnd();
    void onSDLPollEvent(SDL_Event *event, bool &finish);


    Player *player;
    int kills = 0;

    Player *getPlayer() const;

    void onUpdateIA();

    void resolveCollisions();

    void redScreen();

    void setKills(int kills);
    int  getKills() const;
};


#endif //BRAKEDA3D_COMPONENTGAME_H
