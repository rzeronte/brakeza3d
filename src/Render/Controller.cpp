
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <LinearMath/btDefaultMotionState.h>
#include "../../headers/Render/Controller.h"
#include "../../headers/Render/Logging.h"
#include "../../headers/Render/Transforms.h"
#include "../../headers/Render/Maths.h"
#include "../../headers/Physics/Projectile3DBody.h"

Controller::Controller()
{
}

void Controller::handleMouse(SDL_Event *event, Camera3D *camera, btDiscreteDynamicsWorld* dynamicsWorld, std::vector<SpriteDirectional3DBody*> &projectiles, Timer *timer, MenuManager *menu, WeaponsManager *weapon)
{

    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) return;

    if (event->type == SDL_MOUSEBUTTONDOWN) {
        MousePressed = true;
    }

    if (event->type == SDL_MOUSEBUTTONUP) {
        MousePressed = false;
    }

    if (event->type == SDL_MOUSEMOTION) {
        MouseMotion = true;
    }

    // Camera rotation
    if (MouseMotion && MousePressed) {
        MouseMotion = false;
        if (event->type == SDL_MOUSEMOTION) {
            camera->Yaw(event->motion.xrel);
            camera->Pitch(event->motion.yrel);
        }
    }

    // Firing
    if ( MousePressed ) {
        /*
        Logging::getInstance()->Log("Fire");
        SpriteDirectional3DBody *projectile = new SpriteDirectional3DBody();
        projectile->setPosition(*camera->getPosition());
        projectile->setLabel("projectile");
        projectile->setEnabled(true);
        projectile->setTimer(timer);
        projectile->linkTexturesTo(bulletTemplate);
        projectile->setAnimation(0);
        projectile->makeRigidBody(1, projectiles, camera, dynamicsWorld);
        // Giramos antes de hacer el rigidbody, para no alterar los cálculos en la dirección
        // del impulso en el interior del rigidbody
        projectile->setRotation(camera->getRotation());*/
    }

    this->keyboard = (unsigned char *) SDL_GetKeyboardState(NULL);

}

void Controller::handleKeyboardContinuous(SDL_Event *event, Camera3D *camera, bool &end, btDiscreteDynamicsWorld* dynamicsWorld, std::vector<SpriteDirectional3DBody*> &projectiles, Timer *timer, MenuManager *menu, WeaponsManager *weapon)
{
    if (keyboard[SDL_SCANCODE_W]) {
        camera->MoveForward();
    }
    if (keyboard[SDL_SCANCODE_S]) {
        camera->MoveBackward();
    }
    if (keyboard[SDL_SCANCODE_A]) {
        camera->StrafeLeft();
    }
    if (keyboard[SDL_SCANCODE_D]) {
        camera->StrafeRight();
    }
    if (keyboard[SDL_SCANCODE_RIGHT]) {
        camera->TurnRight();
    }
    if (keyboard[SDL_SCANCODE_LEFT]) {
        camera->TurnLeft();
    }
    if (keyboard[SDL_SCANCODE_DOWN]) {
        camera->PitchUp();
    }
    if (keyboard[SDL_SCANCODE_UP]) {
        camera->PitchDown();
    }

    if (keyboard[SDL_SCANCODE_SPACE]) {
        camera->Jump();
     }

    if (keyboard[SDL_SCANCODE_Q]) {

        this->firing = true;
        if (weapon->getCurrentWeaponType()->cadenceTimerTest()) {
            if (weapon->getCurrentWeaponType()->getHitType() != EngineSetup::getInstance()->WeaponsHitTypes::WEAPON_HIT_MELEE) {
                Projectile3DBody *projectile = new Projectile3DBody();
                projectile->setPosition(*camera->getPosition());
                projectile->setLabel("projectile");
                projectile->setEnabled(true);
                projectile->setTimer(timer);
                projectile->linkTexturesTo(weapon->getCurrentWeaponType()->getProjectileTemplate());
                projectile->setAnimation(0);
                projectile->makeRigidBody(1, projectiles, camera, dynamicsWorld, true, weapon->getCurrentWeaponType()->speed);
                projectile->getBillboard()->setDimensions(
                    weapon->getCurrentWeaponType()->projectileWidth,
                    weapon->getCurrentWeaponType()->projectileHeight
                );

                // Giramos antes de hacer el rigidbody, para no alterar los cálculos en la dirección
                // del impulso en el interior del makeRigidBody
                projectile->setRotation(camera->getRotation());

                weapon->getCurrentWeaponType()->startFireAction();
                Tools::playMixedSound(EngineBuffers::getInstance()->snd_weapon_1);
            }
        }
    }
}

void Controller::handleKeyboard(SDL_Event *event, Camera3D *camera, bool &end, btDiscreteDynamicsWorld* dynamicsWorld, std::vector<SpriteDirectional3DBody*> &projectiles, Timer *timer, MenuManager *menu, WeaponsManager *weapon)
{
    if (keyboard[SDL_SCANCODE_ESCAPE] && event->type == SDL_KEYDOWN ) {
        EngineSetup::getInstance()->MENU_ACTIVE = !EngineSetup::getInstance()->MENU_ACTIVE;
        if (!EngineSetup::getInstance()->MENU_ACTIVE) {
            Mix_HaltMusic();
            Mix_PlayMusic( EngineBuffers::getInstance()->snd_base_level_0, -1 );
            EngineSetup::getInstance()->SHOW_WEAPON = true;
        } else {
            Mix_HaltMusic();
            Mix_PlayMusic(EngineBuffers::getInstance()->snd_base_menu, -1 );
            EngineSetup::getInstance()->SHOW_WEAPON = false;
        }
    }

    if (keyboard[SDL_SCANCODE_1]) {
        weapon->currentWeapon = EngineSetup::getInstance()->WeaponsTypes::WEAPON_TYPE_MELEE;
        weapon->getCurrentWeaponType()->getCurrentWeaponAnimation()->timer->start();
    }

    if (keyboard[SDL_SCANCODE_2]) {
        weapon->currentWeapon = EngineSetup::getInstance()->WeaponsTypes::WEAPON_TYPE_GUN;
        weapon->getCurrentWeaponType()->getCurrentWeaponAnimation()->timer->start();
    }

    if (keyboard[SDL_SCANCODE_3]) {
        weapon->currentWeapon = EngineSetup::getInstance()->WeaponsTypes::WEAPON_TYPE_MACHINEGUN;
        weapon->getCurrentWeaponType()->getCurrentWeaponAnimation()->timer->start();
    }

    if (keyboard[SDL_SCANCODE_4]) {
        weapon->currentWeapon = EngineSetup::getInstance()->WeaponsTypes::WEAPON_TYPE_ROCKETLAUNCHER;
        weapon->getCurrentWeaponType()->getCurrentWeaponAnimation()->timer->start();
    }

    if (keyboard[SDL_SCANCODE_DOWN]) {
        if (EngineSetup::getInstance()->MENU_ACTIVE) {
            if (menu->currentOptions+1 < menu->numOptions) {
                menu->currentOptions++;
            }
        }
    }

    if (keyboard[SDL_SCANCODE_UP]) {
        if (EngineSetup::getInstance()->MENU_ACTIVE) {
            if (menu->currentOptions > 0) {
                menu->currentOptions--;
            }
        }
    }

    if (keyboard[SDL_SCANCODE_RETURN]) {
        Logging::getInstance()->Log(menu->options[weapon->currentWeapon]->label);
        if (EngineSetup::getInstance()->MENU_ACTIVE && menu->options[menu->currentOptions]->label == "exit") {
            end = true;
        }
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


bool Controller::isFiring()
{
    return firing;
}

void Controller::resetFlags()
{
    this->firing = false;
}