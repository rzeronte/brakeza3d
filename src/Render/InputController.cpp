
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <LinearMath/btDefaultMotionState.h>
#include "../../headers/Render/InputController.h"
#include "../../headers/Render/Transforms.h"
#include "../../headers/Render/Maths.h"
#include "../../headers/PhysicsGame/Projectile3DBody.h"
#include "../../headers/Render/EngineBuffers.h"
#include "../../headers/Brakeza3D.h"

InputController::InputController()
{
    printf("InputController...\r\n");
}

void InputController::handleMouse(SDL_Event *event)
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
            Brakeza3D::get()->getCamera()->Yaw(event->motion.xrel);
            Brakeza3D::get()->getCamera()->Pitch(event->motion.yrel);
        }
    }

    // Firing
    if ( MousePressed ) {
    }

    this->keyboard = (unsigned char *) SDL_GetKeyboardState(NULL);

}

void InputController::handleKeyboardContinuous(SDL_Event *event, bool &end)
{
    if (keyboard[SDL_SCANCODE_W]) {
        Brakeza3D::get()->getCamera()->MoveForward();
    }
    if (keyboard[SDL_SCANCODE_S]) {
        Brakeza3D::get()->getCamera()->MoveBackward();
    }
    if (keyboard[SDL_SCANCODE_A]) {
        Brakeza3D::get()->getCamera()->StrafeLeft();
    }
    if (keyboard[SDL_SCANCODE_D]) {
        Brakeza3D::get()->getCamera()->StrafeRight();
    }
    if (keyboard[SDL_SCANCODE_RIGHT]) {
        Brakeza3D::get()->getCamera()->TurnRight();
    }
    if (keyboard[SDL_SCANCODE_LEFT]) {
        Brakeza3D::get()->getCamera()->TurnLeft();
    }
    if (keyboard[SDL_SCANCODE_DOWN]) {
        Brakeza3D::get()->getCamera()->PitchUp();
    }
    if (keyboard[SDL_SCANCODE_UP]) {
        Brakeza3D::get()->getCamera()->PitchDown();
    }

    if (keyboard[SDL_SCANCODE_SPACE]) {
        Brakeza3D::get()->getCamera()->Jump();
     }

    if (keyboard[SDL_SCANCODE_Q]) {
        //if ( weapon->getCurrentWeaponType()->ammo <= 0) return;


        if (!Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->isCadenceInProgress()) {
            if (Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->getHitType() != EngineSetup::getInstance()->WeaponsHitTypes::WEAPON_HIT_MELEE) {
                if (Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->ammo <= 0) return;

                Projectile3DBody *projectile = new Projectile3DBody();
                projectile->setPosition(*Brakeza3D::get()->getCamera()->getPosition());
                projectile->setLabel("projectile");
                projectile->setEnabled(true);
                projectile->setTimer(Brakeza3D::get()->getTimer());
                projectile->linkTexturesTo(Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->getProjectileTemplate());
                projectile->setAnimation(0);
                projectile->makeProjectileRigidBody(1, Brakeza3D::get()->getSceneObjects(), Brakeza3D::get()->getCamera(), Brakeza3D::get()->getCollisionManager()->getDynamicsWorld(), true, Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->speed);
                projectile->getBillboard()->setDimensions(
                    Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->projectileWidth,
                    Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->projectileHeight
                );

                // Reduce ammo for this weapon type
                Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->ammo--;

                // Giramos antes de hacer el rigidbody, para no alterar los cálculos en la dirección
                // del impulso en el interior del makeRigidBody
                projectile->setRotation(Brakeza3D::get()->getCamera()->getRotation());

                // Si ya estábamos disparando, no interrumpimos la animación
                Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->startAction();
                Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->setWeaponAnimation(EngineSetup::getInstance()->WeaponsActions::WEAPON_ACTION_FIRE);

                Tools::playMixedSound(Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->soundFire);
            }
        }
    }
}

void InputController::handleKeyboard(SDL_Event *event, bool &end)
{
    if (keyboard[SDL_SCANCODE_ESCAPE] && event->type == SDL_KEYDOWN ) {
        EngineSetup::getInstance()->MENU_ACTIVE = !EngineSetup::getInstance()->MENU_ACTIVE;
        if (!EngineSetup::getInstance()->MENU_ACTIVE) {
            Mix_HaltMusic();
            Mix_PlayMusic( EngineBuffers::getInstance()->snd_base_level_0, -1 );
            EngineSetup::getInstance()->SHOW_WEAPON = true;
        } else {
            Mix_HaltMusic();
            Mix_PlayMusic( EngineBuffers::getInstance()->snd_base_menu, -1 );
            EngineSetup::getInstance()->SHOW_WEAPON = false;
        }
    }

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

    if (keyboard[SDL_SCANCODE_DOWN]) {
        if (EngineSetup::getInstance()->MENU_ACTIVE) {
            if (Brakeza3D::get()->getMenuManager()->currentOptions+1 < Brakeza3D::get()->getMenuManager()->numOptions) {
                Brakeza3D::get()->getMenuManager()->currentOptions++;
            }
        }
    }

    if (keyboard[SDL_SCANCODE_UP]) {
        if (EngineSetup::getInstance()->MENU_ACTIVE) {
            if (Brakeza3D::get()->getMenuManager()->currentOptions > 0) {
                Brakeza3D::get()->getMenuManager()->currentOptions--;
            }
        }
    }

    if (keyboard[SDL_SCANCODE_RETURN]) {
        if (EngineSetup::getInstance()->MENU_ACTIVE && Brakeza3D::get()->getMenuManager()->options[Brakeza3D::get()->getMenuManager()->currentOptions]->label == "exit") {
            end = true;
            return;
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