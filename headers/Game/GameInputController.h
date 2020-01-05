
#ifndef BRAKEZA3D_GAMEINPUTCONTROLLER_H
#define BRAKEZA3D_GAMEINPUTCONTROLLER_H

#include "../Input/InputController.h"
#include "../../src/Game/Player.h"

class GameInputController : public InputController
{
public:
    Player *player;

    GameInputController(Player *player);

    void handleMovingCamera(SDL_Event *, bool &);
    void handleMouse(SDL_Event *);

    void handleInGameInput(SDL_Event *event, bool &end);
    void handleSniper(SDL_Event *event);
    void handleCrouch(SDL_Event *event);
    void handleFire(SDL_Event *event);
    void handleWeaponSelector();
    void handleZoom(SDL_Event *event);

    void handleMenuKeyboard(bool &end);

    void jump(bool checkOnGround, float YForce, bool soundJump);

};


#endif //BRAKEZA3D_GAMEINPUTCONTROLLER_H
