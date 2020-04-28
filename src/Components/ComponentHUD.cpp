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
    SDL_Rect r;
    int iconsY = 215;
    int textY  = 212;
    ComponentsManager* componentManager = ComponentsManager::get();
    SDL_Surface *screenSurface = componentManager->getComponentWindow()->screenSurface;
    SDL_Renderer *render = componentManager->getComponentWindow()->renderer;
    WeaponType *WeaponType = componentManager->getComponentWeapons()->getCurrentWeaponType();
    TTF_Font* font = componentManager->getComponentWindow()->fontDefault;

    // HUD Base
    r.x = 0; r.y = SETUP->screenHeight - this->HUDTextures->getTextureByLabel("hud")->getSurface(1)->h;
    SDL_BlitSurface(this->HUDTextures->getTextureByLabel("hud")->getSurface(1), NULL, screenSurface, &r);

    // Ammo
    if (WeaponType->isAvailable()) {
        r.x = 7; r.y = iconsY;
        SDL_BlitSurface(this->HUDTextures->getTextureByLabel("ammo")->getSurface(1), NULL, screenSurface, &r);
        Tools::writeText(render, font, 25, textY, Color::green(), std::to_string(WeaponType->getAmmoType()->getAmount()));
    }

    // Stamina
    r.x = 57; r.y = iconsY;
    SDL_BlitSurface(this->HUDTextures->getTextureByLabel("health")->getSurface(1), NULL, screenSurface, &r);
    Tools::writeText(render, font, 78, textY, Color::green(), std::to_string( ComponentsManager::get()->getComponentGame()->getPlayer()->getStamina() ));

    // kills
    Tools::writeText(render, font, 122, textY, Color::green(), std::to_string( ComponentsManager::get()->getComponentGame()->getKills() ));

    // Weapon Icon
    if (WeaponType->isAvailable()) {
        r.x = 183 ; r.y = iconsY-2;
        SDL_BlitSurface(WeaponType->iconHUD, NULL, screenSurface, &r);
    }

    r.x = 151 ; r.y = iconsY-3;
    SDL_BlitSurface(this->faceAnimations[currentFaceAnimationIndex]->getCurrentFrame()->getSurface(1), NULL, screenSurface, &r);
}
