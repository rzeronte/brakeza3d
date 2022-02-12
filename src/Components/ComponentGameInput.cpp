#include "../../include/Components/ComponentGameInput.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"


ComponentGameInput::ComponentGameInput(Player *player) {
    this->player = player;
    setEnabled(true);
}

void ComponentGameInput::onStart() {
    Logging::Log("ComponentGameInput onStart", "ComponentGameInput");

    ComponentsManager::get()->getComponentInput()->setEnabled(false);
}

void ComponentGameInput::preUpdate() {

}

void ComponentGameInput::onUpdate() {
}

void ComponentGameInput::postUpdate() {

}

void ComponentGameInput::onEnd() {

}

void ComponentGameInput::onSDLPollEvent(SDL_Event *event, bool &finish) {
    handleMouse(event);
    handleInGameInput(event, finish);
    handleKeyboardMovingCamera(event, finish);
}

void ComponentGameInput::handleMouse(SDL_Event *event) {
    if (event->type == SDL_BUTTON_RIGHT) {
    }

    if (event->type == SDL_BUTTON_LEFT) {
    }
}

void ComponentGameInput::handleKeyboardMovingCamera(SDL_Event *event, bool &end) {
    if (SETUP->MENU_ACTIVE) return;

    Uint8 *keyboard = ComponentsManager::get()->getComponentInput()->keyboard;

    if (keyboard[SDL_SCANCODE_SPACE]) {
        if (event->type == SDL_KEYDOWN) {
        }
    }

    // step sounds
    if (keyboard[SDL_SCANCODE_W] || keyboard[SDL_SCANCODE_S] || keyboard[SDL_SCANCODE_A] || keyboard[SDL_SCANCODE_D]) {
        if (event->type == SDL_KEYDOWN) {
        }
    } else {
        Mix_HaltChannel(EngineSetup::SoundChannels::SND_PLAYER_STEPS);
    }
}

void ComponentGameInput::handleInGameInput(SDL_Event *event, bool &end) {

    this->handleEscape(event);

    handleRespawnAfterDead(event);

    handleMenuKeyboard(end);

    this->handleKeyboardMovingPlayer(event);

    if (SETUP->MENU_ACTIVE || SETUP->LOADING) return;

    this->handleKeyboardMovingPlayer(event);
    //this->handleZoom(event);
    //this->handleCrouch(event);
    //this->handleFire(event);
    //this->handleWeaponReload(event);
    //this->handleSniper(event);
    //this->handleWeaponSelector();

}

void ComponentGameInput::handleEscape(SDL_Event *event) {
    Uint8 *keyboard = ComponentsManager::get()->getComponentInput()->keyboard;

    if (keyboard[SDL_SCANCODE_ESCAPE] && event->type == SDL_KEYDOWN && player->state != PlayerState::GAMEOVER) {
        SETUP->MENU_ACTIVE = !SETUP->MENU_ACTIVE;

        if (!SETUP->MENU_ACTIVE) {
            //SDL_SetRelativeMouseMode(SDL_TRUE);
            Mix_HaltMusic();
            Mix_PlayMusic(BUFFERS->soundPackage->getMusicByLabel("musicBaseLevel0"), -1);
            SETUP->DRAW_HUD = true;
            ComponentsManager::get()->getComponentInput()->setEnabled(true);
        } else {
            //SDL_SetRelativeMouseMode(SDL_FALSE);
            SDL_WarpMouseInWindow(ComponentsManager::get()->getComponentWindow()->window, SETUP->screenWidth / 2,
                                  SETUP->screenHeight / 2);
            Mix_HaltMusic();
            Mix_PlayMusic(BUFFERS->soundPackage->getMusicByLabel("musicMainMenu"), -1);
            SETUP->DRAW_HUD = false;
            ComponentsManager::get()->getComponentInput()->setEnabled(false);
        }

        Tools::playMixedSound(BUFFERS->soundPackage->getSoundByLabel("soundMenuAccept"),
                              EngineSetup::SoundChannels::SND_MENU, 0);
    }
}

void ComponentGameInput::handleMenuKeyboard(bool &end) {
    Uint8 *keyboard = ComponentsManager::get()->getComponentInput()->keyboard;

    // Subir y bajar de opción
    if (keyboard[SDL_SCANCODE_DOWN]) {
        if (SETUP->MENU_ACTIVE) {
            if (ComponentsManager::get()->getComponentMenu()->currentOption + 1 <
                ComponentsManager::get()->getComponentMenu()->numOptions) {
                ComponentsManager::get()->getComponentMenu()->currentOption++;
                Tools::playMixedSound(BUFFERS->soundPackage->getSoundByLabel("soundMenuClick"),
                                      EngineSetup::SoundChannels::SND_MENU, 0);
            }
        }
    }

    if (keyboard[SDL_SCANCODE_UP]) {
        if (SETUP->MENU_ACTIVE) {
            if (ComponentsManager::get()->getComponentMenu()->currentOption > 0) {
                ComponentsManager::get()->getComponentMenu()->currentOption--;
                Tools::playMixedSound(BUFFERS->soundPackage->getSoundByLabel("soundMenuClick"),
                                      EngineSetup::SoundChannels::SND_MENU, 0);
            }
        }
    }

    // Enter en opción
    if (keyboard[SDL_SCANCODE_RETURN]) {
        if (SETUP->MENU_ACTIVE &&
            ComponentsManager::get()->getComponentMenu()->options[ComponentsManager::get()->getComponentMenu()->currentOption]->getAction() ==
            ComponentMenu::MNU_EXIT) {
            end = true;
            return;
        }

        if (SETUP->MENU_ACTIVE && player->state == PlayerState::GAMEOVER &&
            ComponentsManager::get()->getComponentMenu()->options[ComponentsManager::get()
                    ->getComponentMenu()->currentOption]->getAction() == ComponentMenu::MNU_NEW_GAME
                ) {
            Tools::playMixedSound(BUFFERS->soundPackage->getSoundByLabel("soundMenuAccept"),
                                  EngineSetup::SoundChannels::SND_MENU, 0);

            player->newGame();
        }

        if (SETUP->MENU_ACTIVE && player->state != PlayerState::GAMEOVER &&
            ComponentsManager::get()->getComponentMenu()->options[ComponentsManager::get()
                    ->getComponentMenu()->currentOption]->getAction() == ComponentMenu::MNU_NEW_GAME
                ) {

            Tools::playMixedSound(BUFFERS->soundPackage->getSoundByLabel("soundMenuAccept"),
                                  EngineSetup::SoundChannels::SND_MENU, 0);

            Mix_HaltMusic();
            Mix_PlayMusic(BUFFERS->soundPackage->getMusicByLabel("musicBaseLevel0"), -1);
            SETUP->DRAW_HUD = true;

            SETUP->MENU_ACTIVE = !SETUP->MENU_ACTIVE;
        }
    }
}

void ComponentGameInput::jump(bool soundJump) const {
    if (soundJump) {
        player->jump();
    }
}

void ComponentGameInput::handleFire(SDL_Event *event) const {
    if (event->key.keysym.sym == SDLK_q) {

        // First keydown
        if (event->type == SDL_KEYDOWN) {
            player->shoot();
        }

        // Looping keydown
        if (event->type == SDL_KEYDOWN) {
        }

        // keyup
        if (event->type == SDL_KEYUP) {
        }
    }
}

void ComponentGameInput::handleWeaponReload(SDL_Event *event) const {
    if (event->key.keysym.sym == SDLK_e) {

        // First keydown
        if (event->type == SDL_KEYDOWN) {
            player->reload();
        }

        // Looping keydown
        if (event->type == SDL_KEYDOWN) {
        }

        // keyup
        if (event->type == SDL_KEYUP) {
        }
    }
}

void ComponentGameInput::handleSniper(SDL_Event *event) {
    if (event->key.keysym.sym == SDLK_LSHIFT) {

        Camera3D *cam = ComponentsManager::get()->getComponentCamera()->getCamera();

        if (event->type == SDL_KEYDOWN) {
            if (ComponentsManager::get()->getComponentWeapons()->getCurrentWeaponType()->isSniper()) {
                ComponentsManager::get()->getComponentWeapons()->getCurrentWeaponType()->setSniperEnabled(true);

                Tools::playMixedSound(BUFFERS->soundPackage->getSoundByLabel("sniperOn"),
                                      EngineSetup::SoundChannels::SND_WEAPON, 0);

                cam->frustum->setup(
                        cam->getPosition(),
                        Vertex3D(0, 0, 1),
                        SETUP->up,
                        SETUP->right,
                        EngineSetup::get()->ZOOM_FOV,
                        ((float) EngineSetup::get()->screenHeight / (float) EngineSetup::get()->screenWidth),
                        EngineSetup::get()->FRUSTUM_FARPLANE_DISTANCE
                );
                cam->updateFrustum();
            }
        }

        if (event->type == SDL_KEYUP) {
            if (ComponentsManager::get()->getComponentWeapons()->getCurrentWeaponType()->isSniper()) {

                ComponentsManager::get()->getComponentWeapons()->getCurrentWeaponType()->setSniperEnabled(false);

                cam->frustum->setup(
                        cam->getPosition(),
                        Vertex3D(0, 0, 1),
                        SETUP->up,
                        SETUP->right,
                        EngineSetup::get()->HORIZONTAL_FOV,
                        ((float) EngineSetup::get()->screenHeight / (float) EngineSetup::get()->screenWidth),
                        EngineSetup::get()->FRUSTUM_FARPLANE_DISTANCE
                );
                cam->updateFrustum();

            }
        }

        if ((event->type == SDL_KEYDOWN || event->type == SDL_KEYUP)) {
        }
    }
}

void ComponentGameInput::handleWeaponSelector() {
    SoundPackage *soundPackage = BUFFERS->soundPackage;
    Uint8 *keyboard = ComponentsManager::get()->getComponentInput()->keyboard;

    if (keyboard[SDL_SCANCODE_1]) {
    }

    if (keyboard[SDL_SCANCODE_2]) {
        Tools::playMixedSound(soundPackage->getSoundByLabel("switchWeapon"), EngineSetup::SoundChannels::SND_PLAYER, 0);
    }

    if (keyboard[SDL_SCANCODE_3] ) {
        Tools::playMixedSound(soundPackage->getSoundByLabel("switchWeapon"), EngineSetup::SoundChannels::SND_PLAYER, 0);
    }
}

void ComponentGameInput::handleZoom(SDL_Event *event)
{
    float horizontal_fov = SETUP->HORIZONTAL_FOV;
    if (event->key.keysym.sym == SDLK_z) {
        if (event->type == SDL_KEYDOWN) {
            horizontal_fov = SETUP->ZOOM_FOV;
        }

        if (event->type == SDL_KEYUP) {
            horizontal_fov = SETUP->HORIZONTAL_FOV;
        }

        ComponentsManager::get()->getComponentCamera()->getCamera()->frustum->setup(
                ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition(),
                Vertex3D(0, 0, 1),
                SETUP->up,
                SETUP->right,
                horizontal_fov,
                ((float) EngineSetup::get()->screenHeight / (float) EngineSetup::get()->screenWidth),
                EngineSetup::get()->FRUSTUM_FARPLANE_DISTANCE
        );

        ComponentsManager::get()->getComponentCamera()->getCamera()->updateFrustum();
    }
}

void ComponentGameInput::handleRespawnAfterDead(SDL_Event *event) {
    if (player->isDead() && !SETUP->MENU_ACTIVE) {
        if (event->type == SDL_KEYDOWN) {
            player->respawn();
        }

        return;
    }
}

bool ComponentGameInput::isEnabled() const {
    return enabled;
}

void ComponentGameInput::setEnabled(bool enabled) {
    ComponentGameInput::enabled = enabled;
}

void ComponentGameInput::handleKeyboardMovingPlayer(SDL_Event *pEvent) {
    Uint8 *keyboard = ComponentsManager::get()->getComponentInput()->keyboard;

    float speed = player->power * Brakeza3D::get()->getDeltaTime();
    speed = std::clamp(speed, 0.f, player->maxVelocity);

    if (keyboard[SDL_SCANCODE_A]) {
        Vertex3D velocity = player->getVelocity() - Vertex3D(speed, 0, 0);
        player->setVelocity(velocity);
    }
    if (keyboard[SDL_SCANCODE_D]) {
        Vertex3D velocity = player->getVelocity() + Vertex3D(speed, 0, 0);
        player->setVelocity(velocity);
    }
    if (keyboard[SDL_SCANCODE_S]) {
        Vertex3D velocity = player->getVelocity() - Vertex3D(0, 0, speed);
        player->setVelocity(velocity);
    }
    if (keyboard[SDL_SCANCODE_W]) {
        Vertex3D velocity = player->getVelocity() + Vertex3D(0, 0, speed);
        player->setVelocity(velocity);
    }
}

