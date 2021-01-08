//
// Created by darkhead on 12/1/20.
//

#include "../../headers/Components/ComponentHUD.h"
#include "../../headers/ComponentsManager.h"

ComponentHUD::ComponentHUD()
{
    HUDTextures = new TexturePackage();
    counterFaceAnimation = new Counter();
    counterMinReactionTime = new Counter(0.5);
}

void ComponentHUD::onStart()
{
    std::cout << "ComponentHUD onStart" << std::endl;

    loadImages();
    loadStatusFaceImages();
    setStatusFaceAnimation(StatusFace::STAND);
}

void ComponentHUD::preUpdate() {

}

void ComponentHUD::onUpdate()
{
    if (SETUP->DRAW_HUD && !SETUP->MENU_ACTIVE) {

        if (SETUP->DRAW_CROSSHAIR) {
            Drawable::drawCrossHair();
        }

        counterFaceAnimation->update();
        counterMinReactionTime->update();

        if (counterMinReactionTime->isFinished()) {
            counterMinReactionTime->setEnabled( true ) ;
            if (ComponentsManager::get()->getComponentHUD()->currentFaceAnimationIndex != ComponentHUD::StatusFace::STAND
                && ComponentsManager::get()->getComponentHUD()->currentFaceAnimationIndex != ComponentHUD::StatusFace::DEAD
            ) {
                ComponentsManager::get()->getComponentHUD()->setStatusFaceAnimation(ComponentHUD::StatusFace::STAND);
            }
        }

        if (counterFaceAnimation->isFinished()) {
            counterFaceAnimation->setEnabled( true ) ;
            faceAnimations[currentFaceAnimationIndex]->nextFrame();
        }

        //drawHUD();

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
    HUDTextures->addItem(SETUP->HUD_FOLDER + "splash.png", "splash");
    HUDTextures->addItem(SETUP->HUD_FOLDER + "hud_health.png", "health");
    HUDTextures->addItem(SETUP->HUD_FOLDER + "hud_ammo.png", "ammo");
    HUDTextures->addItem(SETUP->HUD_FOLDER + "hud.png", "hud");
    HUDTextures->addItem(SETUP->HUD_FOLDER + "loading.png", "loading");
}

void ComponentHUD::loadStatusFaceImages()
{
    Logging::getInstance()->Log("Loading facing status images...");

    auto *faceEvil  = new TextureAnimation();
    auto *faceKill  = new TextureAnimation();
    auto *faceOuch  = new TextureAnimation();
    auto *faceStand = new TextureAnimation();
    auto *faceDead  = new TextureAnimation();

    faceEvil->setup(SETUP->ASSETS_FOLDER + "face/evil", 5, 3);
    faceKill->setup(SETUP->ASSETS_FOLDER + "face/kill", 5, 3);
    faceOuch->setup(SETUP->ASSETS_FOLDER + "face/ouch", 5, 3);
    faceStand->setup(SETUP->ASSETS_FOLDER + "face/stand", 5, 3);
    faceDead->setup(SETUP->ASSETS_FOLDER + "face/dead", 1, 1);

    this->faceAnimations.push_back(faceEvil);
    this->faceAnimations.push_back(faceKill);
    this->faceAnimations.push_back(faceOuch);
    this->faceAnimations.push_back(faceStand);
    this->faceAnimations.push_back(faceDead);
}

void ComponentHUD::setStatusFaceAnimation(int id)
{
    currentFaceAnimationIndex = id;
    counterMinReactionTime->setEnabled( true ) ;
    float step = (float) 1 / (float) this->faceAnimations[currentFaceAnimationIndex]->getFps();
    counterFaceAnimation->setStep( step );
}

void ComponentHUD::drawHUD()
{
    int textY  = 180;
    int textX = 10;
    int stepY = 10;

    ComponentsManager* componentManager = ComponentsManager::get();
    SDL_Renderer*      render = componentManager->getComponentWindow()->renderer;
    WeaponType*        WeaponType = componentManager->getComponentWeapons()->getCurrentWeaponType();
    TTF_Font*          font = componentManager->getComponentWindow()->fontSmall;

    // Weapon
    Tools::writeText(render, font, textX, textY, Color::green(), "Weapon: " +  ComponentsManager::get()->getComponentWeapons()->getCurrentWeaponType()->getClassname() );

    textY += stepY;

    // Ammo
    if (WeaponType->isAvailable()) {
        Tools::writeText(render, font, textX, textY, Color::green(), "Reloads: " + std::to_string(WeaponType->getAmmoType()->getReloads()));
        textY += stepY;
        Tools::writeText(render, font, textX, textY, Color::green(), "Ammo: " + std::to_string(WeaponType->getAmmoType()->getAmount()));
        textY += stepY;
    }


    // Stamina
    Tools::writeText(render, font, textX, textY, Color::green(), "Health: " + std::to_string( ComponentsManager::get()->getComponentGame()->getPlayer()->getStamina() ));

    textY += stepY;

    // kills
    Tools::writeText(render, font, textX, textY, Color::green(), "Kills: " + std::to_string( ComponentsManager::get()->getComponentGame()->getKills() ));

}
