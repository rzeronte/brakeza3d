#include "../../headers/Game/GameInputController.h"
#include "../../headers/Brakeza3D.h"
#include "../../headers/Render/EngineBuffers.h"

GameInputController::GameInputController(Player *player) : player(player)
{
}
void GameInputController::handleMouse(SDL_Event *event)
{
    if ( player->isDead() && !EngineSetup::getInstance()->MENU_ACTIVE ) {
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

    if (keyboard[SDL_SCANCODE_SPACE]) {
        this->jump();
    }

    if (keyboard[SDL_SCANCODE_W] || keyboard[SDL_SCANCODE_S] || keyboard[SDL_SCANCODE_A] || keyboard[SDL_SCANCODE_D]) {
        if (player->counterStep->isFinished()) {
            int rndStep = Tools::random(1, 6);
            Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("playerStep" + std::to_string(rndStep)), EngineSetup::SoundChannels::SND_PLAYER);
        }
    }

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
            EngineSetup::getInstance()->DRAW_HUD    = true;
        } else {
            Mix_HaltMusic();
            Mix_PlayMusic( EngineBuffers::getInstance()->soundPackage->getMusicByLabel("musicMainMenu"), -1 );
            EngineSetup::getInstance()->DRAW_WEAPON = false;
            EngineSetup::getInstance()->DRAW_HUD    = false;
        }

        Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("soundMenuAccept"), EngineSetup::SoundChannels::SND_MENU);
    }

    handleMenuKeyboard(end);

    if (player->isDead()) return;

    if (event->key.keysym.sym == SDLK_z ) {
        if (event->type == SDL_KEYDOWN) {
            Brakeza3D::get()->getCamera()->horizontal_fov = (float) EngineSetup::getInstance()->ZOOM_FOV;
        }

        if (event->type == SDL_KEYUP) {
            Brakeza3D::get()->getCamera()->horizontal_fov = (float) EngineSetup::getInstance()->HORIZONTAL_FOV;
        }

        Brakeza3D::get()->getCamera()->frustum->setup(
                *Brakeza3D::get()->getCamera()->getPosition(),
                Vertex3D(0, 0, 1),
                EngineSetup::getInstance()->up,
                EngineSetup::getInstance()->right,
                Brakeza3D::get()->getCamera()->getNearDistance(),
                Brakeza3D::get()->getCamera()->calcCanvasNearHeight(), Brakeza3D::get()->getCamera()->calcCanvasNearWidth(),
                Brakeza3D::get()->getCamera()->farDistance,
                Brakeza3D::get()->getCamera()->calcCanvasFarHeight(), Brakeza3D::get()->getCamera()->calcCanvasFarWidth()
        );

        Brakeza3D::get()->getCamera()->UpdateFrustum();
    }

    if (keyboard[SDL_SCANCODE_1]) {
        Brakeza3D::get()->getWeaponsManager()->currentWeapon = EngineSetup::getInstance()->WeaponsTypes::WEAPON_TYPE_GUN;
    }

    if (keyboard[SDL_SCANCODE_2]) {
        Brakeza3D::get()->getWeaponsManager()->currentWeapon = EngineSetup::getInstance()->WeaponsTypes::WEAPON_TYPE_SHOTGUN;
    }

    if (keyboard[SDL_SCANCODE_3]) {
        Brakeza3D::get()->getWeaponsManager()->currentWeapon = EngineSetup::getInstance()->WeaponsTypes::WEAPON_TYPE_MACHINEGUN;
    }

    if (keyboard[SDL_SCANCODE_4]) {
        Brakeza3D::get()->getWeaponsManager()->currentWeapon = EngineSetup::getInstance()->WeaponsTypes::WEAPON_TYPE_ROCKETLAUNCHER;
    }

    if (keyboard[SDL_SCANCODE_5]) {
        Brakeza3D::get()->getWeaponsManager()->currentWeapon = EngineSetup::getInstance()->WeaponsTypes::WEAPON_TYPE_LIGHTING;
    }

    if (keyboard[SDL_SCANCODE_6]) {
        Brakeza3D::get()->getWeaponsManager()->currentWeapon = EngineSetup::getInstance()->WeaponsTypes::WEAPON_TYPE_FREEZER;
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
                Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("soundMenuClick"), EngineSetup::SoundChannels::SND_MENU);
            }
        }
    }

    if (keyboard[SDL_SCANCODE_UP]) {
        if (EngineSetup::getInstance()->MENU_ACTIVE) {
            if (Brakeza3D::get()->getMenuManager()->currentOption > 0) {
                Brakeza3D::get()->getMenuManager()->currentOption--;
                Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("soundMenuClick"), EngineSetup::SoundChannels::SND_MENU);
            }
        }
    }

    if (keyboard[SDL_SCANCODE_RETURN]) {
        if (EngineSetup::getInstance()->MENU_ACTIVE && Brakeza3D::get()->getMenuManager()->options[Brakeza3D::get()->getMenuManager()->currentOption]->label == "exit") {
            end = true;
            return;
        }

        if (EngineSetup::getInstance()->MENU_ACTIVE && player->state == PlayerState::GAMEOVER && Brakeza3D::get()->getMenuManager()->options[Brakeza3D::get()->getMenuManager()->currentOption]->label == "new game") {
            Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("soundMenuAccept"), EngineSetup::SoundChannels::SND_MENU);

            player->newGame();
        }
    }
}

void GameInputController::jump()
{
    if( Brakeza3D::get()->getCamera()->kinematicController->onGround() ) {
        int rndJump = Tools::random(1, 4);
        Tools::playMixedSound( EngineBuffers::getInstance()->soundPackage->getSoundByLabel("playerJump" + std::to_string(rndJump)), EngineSetup::SoundChannels::SND_PLAYER);

        Brakeza3D::get()->getCamera()->kinematicController->jump(btVector3(0, EngineSetup::getInstance()->JUMP_FORCE.y, 0));
    }
}