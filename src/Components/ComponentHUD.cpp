#include "../../include/Components/ComponentHUD.h"
#include "../../include/ComponentsManager.h"

ComponentHUD::ComponentHUD() {
    HUDTextures = new TexturePackage();
}

void ComponentHUD::onStart() {
    Logging::Log("ComponentHUD onStart", "ComponentHUD");

    loadImages();

    textureWriter = new TextWriter(
            ComponentsManager::get()->getComponentWindow()->renderer,
            std::string(EngineSetup::get()->SPRITES_FOLDER + "conchars.png").c_str()
    );
}

void ComponentHUD::preUpdate() {
}

void ComponentHUD::onUpdate() {
    if (SETUP->DRAW_HUD) {
        if (SETUP->DRAW_CROSSHAIR) {
            Drawable::drawCrossHair();
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

void ComponentHUD::loadImages() {
    HUDTextures->addItem(SETUP->HUD_FOLDER + "splash.png", "splash");
    HUDTextures->addItem(SETUP->HUD_FOLDER + "hud_health.png", "health");
    HUDTextures->addItem(SETUP->HUD_FOLDER + "hud_ammo.png", "ammoType");
    HUDTextures->addItem(SETUP->HUD_FOLDER + "hud.png", "hud");
    HUDTextures->addItem(SETUP->HUD_FOLDER + "loading.png", "loading");
}

void ComponentHUD::writeTextMiddleScreen(const char *text, bool bold) const {
    int totalW = EngineSetup::get()->screenWidth;
    int totalH = EngineSetup::get()->screenHeight;

    int xPosition = (totalW / 2) - (int) (strlen(text) * CONCHARS_CHARACTER_W) / 2;
    this->writeText(xPosition, totalH / 2, text, bold);
}

void ComponentHUD::writeCenterHorizontal(int y, const char *text, bool bold) const {
    int totalW = EngineSetup::get()->screenWidth;

    int xPosition = (totalW / 2) - (int) (strlen(text) * CONCHARS_CHARACTER_W) / 2;
    this->writeText(xPosition, y, text, bold);
}

void ComponentHUD::writeText(int x, int y, const char *text, bool bold) const {
    this->textureWriter->writeText(x, y, text, bold);
}

void ComponentHUD::drawHUD() {

    int textY = 180;
    int textX = 10;
    int stepY = 10;

    ComponentsManager *componentManager = ComponentsManager::get();

    textY += stepY;

    if (SETUP->DRAW_FPS) {
        writeCenterHorizontal(
            10,
            std::to_string(componentManager->getComponentRender()->fps).c_str(),
            false
        );
    }

    // Ammo
    if (!componentManager->getComponentWeapons()->isEmptyWeapon()) {
        WeaponType *weaponType = componentManager->getComponentWeapons()->getCurrentWeaponType();
        if (weaponType->isAvailable()) {
            this->textureWriter->writeText(textX, textY, weaponType->getLabel().c_str(), false);
            textY += stepY;
        }
    }
}
