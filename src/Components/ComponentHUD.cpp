//
// Created by darkhead on 12/1/20.
//

#include "../../headers/Components/ComponentHUD.h"
#include "../../headers/ComponentsManager.h"

ComponentHUD::ComponentHUD()
{
    HUDTextures = new TexturePackage();
}

void ComponentHUD::onStart()
{
    std::cout << "ComponentHUD onStart" << std::endl;

    this->loadImages();
}

void ComponentHUD::preUpdate() {

}

void ComponentHUD::onUpdate()
{
    if (EngineSetup::getInstance()->DRAW_HUD) {
        drawHUD();
    }
}

void ComponentHUD::postUpdate() {

}

void ComponentHUD::onEnd() {

}

void ComponentHUD::onSDLPollEvent(SDL_Event *event, bool &finish) {

}

void ComponentHUD::loadImages()
{
    HUDTextures->addItem(EngineSetup::getInstance()->HUD_FOLDER + "splash.png", "splash");
    HUDTextures->addItem(EngineSetup::getInstance()->HUD_FOLDER + "hud_health.png", "health");
    HUDTextures->addItem(EngineSetup::getInstance()->HUD_FOLDER + "hud_ammo.png", "ammo");
    HUDTextures->addItem(EngineSetup::getInstance()->HUD_FOLDER + "hud.png", "hud");
    HUDTextures->addItem(EngineSetup::getInstance()->HUD_FOLDER + "loading.png", "loading");
}


void ComponentHUD::drawHUD()
{
    SDL_Rect r;
    float iconsY = 215;
    float textY  = 212;

    // HUD Base
    r.x = 0; r.y = EngineSetup::getInstance()->screenHeight - this->HUDTextures->getTextureByLabel("hud")->getSurface(1)->h;
    SDL_BlitSurface(this->HUDTextures->getTextureByLabel("hud")->getSurface(1), NULL, ComponentsManager::get()->getComponentWindow()->screenSurface, &r);

    // Ammo
    if (ComponentsManager::get()->getComponentWeapons()->getCurrentWeaponType()->isAvailable()) {
        r.x = 7; r.y = iconsY;
        SDL_BlitSurface(this->HUDTextures->getTextureByLabel("ammo")->getSurface(1), NULL, ComponentsManager::get()->getComponentWindow()->screenSurface, &r);
        Tools::writeText(ComponentsManager::get()->getComponentWindow()->renderer, ComponentsManager::get()->getComponentWindow()->fontDefault, 25, textY, Color::gray(), std::to_string(
                ComponentsManager::get()->getComponentWeapons()->getCurrentWeaponType()->getAmmoType()->getAmount()));
    }

    // Stamina
    r.x = 57; r.y = iconsY;
    SDL_BlitSurface(this->HUDTextures->getTextureByLabel("health")->getSurface(1), NULL, ComponentsManager::get()->getComponentWindow()->screenSurface, &r);
    Tools::writeText(ComponentsManager::get()->getComponentWindow()->renderer, ComponentsManager::get()->getComponentWindow()->fontDefault, 78, textY, Color::gray(), std::to_string(69 ));

    // kills
    //Tools::writeText(Brakeza3D::get()->renderer, Brakeza3D::get()->fontDefault, 122, textY, Color::gray(), std::to_string(this->kills));

    // Weapon Icon
    if (ComponentsManager::get()->getComponentWeapons()->getCurrentWeaponType()->isAvailable()) {
        r.x = 183  ; r.y = iconsY-2;
        SDL_BlitSurface(ComponentsManager::get()->getComponentWeapons()->getCurrentWeaponType()->iconHUD, NULL, ComponentsManager::get()->getComponentWindow()->screenSurface, &r);
    }
}
