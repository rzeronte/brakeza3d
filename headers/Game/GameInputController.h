
#ifndef BRAKEZA3D_GAMEINPUTCONTROLLER_H
#define BRAKEZA3D_GAMEINPUTCONTROLLER_H

#include "../Input/InputController.h"
#include "../../src/Game/Player.h"

class GameInputController : public InputController
{
public:
    Player *player;

    GameInputController(Player *player);

    void handleKeyboardContinuous(SDL_Event *, bool &);
    void handleKeyboard(SDL_Event *event, bool &end);
    void handleMouse(SDL_Event *);
};


#endif //BRAKEZA3D_GAMEINPUTCONTROLLER_H
