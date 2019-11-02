#include "../../headers/Game/GameInputController.h"
#include "../../headers/Brakeza3D.h"
#include "../../headers/Render/EngineBuffers.h"

GameInputController::GameInputController(Player *player) : player(player)
{
}
void GameInputController::handleMouse(SDL_Event *event)
{
    if (player->isDead()) {
        if (this->MousePressed) {
            player->respawn();
        }
        return;
    }

    InputController::handleMouse(event);
}

void GameInputController::handleKeyboardContinuous(SDL_Event *event, bool &end)
{
    if (player->isDead()) return;

    InputController::handleKeyboardContinuous(event, end);

    if (keyboard[SDL_SCANCODE_Q]) {

        if (!Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->isCadenceInProgress()) {
            if (Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->getHitType() != EngineSetup::getInstance()->WeaponsHitTypes::WEAPON_HIT_MELEE) {
                if (Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->ammo <= 0) return;
                player->shoot();
            }
        }
    }
}

void GameInputController::handleKeyboard(SDL_Event *event, bool &end)
{
    InputController::handleKeyboard(event, end);

    if (keyboard[SDL_SCANCODE_ESCAPE] && event->type == SDL_KEYDOWN && player->state != PlayerState::GAMEOVER) {
        EngineSetup::getInstance()->MENU_ACTIVE = !EngineSetup::getInstance()->MENU_ACTIVE;
        if (!EngineSetup::getInstance()->MENU_ACTIVE) {
            Mix_HaltMusic();
            Mix_PlayMusic( EngineBuffers::getInstance()->soundPackage->getMusicByLabel("musicBaseLevel0"), -1 );
            EngineSetup::getInstance()->DRAW_WEAPON = true;
            EngineSetup::getInstance()->DRAW_HUD = true;
        } else {
            Mix_HaltMusic();
            Mix_PlayMusic( EngineBuffers::getInstance()->soundPackage->getMusicByLabel("musicBaseLevel0"), -1 );
            EngineSetup::getInstance()->DRAW_WEAPON = false;
            EngineSetup::getInstance()->DRAW_HUD = false;
        }
    }

    handleMenuKeyboard(end);

    if (player->isDead()) return;

    if (keyboard[SDL_SCANCODE_1]) {
        Brakeza3D::get()->getWeaponsManager()->currentWeapon = EngineSetup::getInstance()->WeaponsTypes::WEAPON_TYPE_MELEE;
    }

    if (keyboard[SDL_SCANCODE_2]) {
        Brakeza3D::get()->getWeaponsManager()->currentWeapon = EngineSetup::getInstance()->WeaponsTypes::WEAPON_TYPE_GUN;
    }

    if (keyboard[SDL_SCANCODE_3]) {
        Brakeza3D::get()->getWeaponsManager()->currentWeapon = EngineSetup::getInstance()->WeaponsTypes::WEAPON_TYPE_MACHINEGUN;
    }

    if (keyboard[SDL_SCANCODE_4]) {
        Brakeza3D::get()->getWeaponsManager()->currentWeapon = EngineSetup::getInstance()->WeaponsTypes::WEAPON_TYPE_ROCKETLAUNCHER;
    }


    if (event->type == SDL_WINDOWEVENT) {
        switch (event->window.event) {
            case SDL_WINDOWEVENT_SHOWN:
                break;
            case SDL_WINDOWEVENT_HIDDEN:
                break;
            case SDL_WINDOWEVENT_EXPOSED:
                break;
            case SDL_WINDOWEVENT_MOVED:
                break;
            case SDL_WINDOWEVENT_RESIZED:
                break;
            case SDL_WINDOWEVENT_SIZE_CHANGED:
                break;
            case SDL_WINDOWEVENT_MINIMIZED:
                break;
            case SDL_WINDOWEVENT_MAXIMIZED:
                break;
            case SDL_WINDOWEVENT_RESTORED:
                break;
            case SDL_WINDOWEVENT_ENTER:
                break;
            case SDL_WINDOWEVENT_LEAVE:
                break;
            case SDL_WINDOWEVENT_FOCUS_GAINED:
                break;
            case SDL_WINDOWEVENT_FOCUS_LOST:
                break;
            case SDL_WINDOWEVENT_CLOSE:
                end = true;
                break;
#if SDL_VERSION_ATLEAST(2, 0, 5)
            case SDL_WINDOWEVENT_TAKE_FOCUS:
                break;
            case SDL_WINDOWEVENT_HIT_TEST:
                break;
#endif
            default:
                break;
        }
    }
}

void GameInputController::handleMenuKeyboard(bool &end)
{
    if (keyboard[SDL_SCANCODE_DOWN]) {
        if (EngineSetup::getInstance()->MENU_ACTIVE) {
            if (Brakeza3D::get()->getMenuManager()->currentOption + 1 < Brakeza3D::get()->getMenuManager()->numOptions) {
                Brakeza3D::get()->getMenuManager()->currentOption++;
            }
        }
    }

    if (keyboard[SDL_SCANCODE_UP]) {
        if (EngineSetup::getInstance()->MENU_ACTIVE) {
            if (Brakeza3D::get()->getMenuManager()->currentOption > 0) {
                Brakeza3D::get()->getMenuManager()->currentOption--;
            }
        }
    }

    if (keyboard[SDL_SCANCODE_RETURN]) {
        if (EngineSetup::getInstance()->MENU_ACTIVE && Brakeza3D::get()->getMenuManager()->options[Brakeza3D::get()->getMenuManager()->currentOption]->label == "exit") {
            end = true;
            return;
        }

        if (EngineSetup::getInstance()->MENU_ACTIVE && player->state == PlayerState::GAMEOVER && Brakeza3D::get()->getMenuManager()->options[Brakeza3D::get()->getMenuManager()->currentOption]->label == "new game") {
            player->newGame();
        }
    }
}
