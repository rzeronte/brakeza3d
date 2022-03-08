#include "../../include/Components/ComponentHUD.h"
#include "../../include/ComponentsManager.h"
#include "../../include/2D/ButtonsCallbacks.h"

ComponentHUD::ComponentHUD() {
    HUDTextures = new TexturePackage();
    iconsTextures = new TexturePackage();
}

void ComponentHUD::onStart() {
    Logging::Log("ComponentHUD onStart", "ComponentHUD");

    loadImages();
    loadButtons();

    textureWriter = new TextWriter(
ComponentsManager::get()->getComponentWindow()->renderer,
std::string(EngineSetup::get()->SPRITES_FOLDER + EngineSetup::get()->CONCHARS_SPRITE_FILE).c_str()
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
    for(auto & button : buttons) {
        button->onUpdate();
    }
}

void ComponentHUD::loadImages() {
    HUDTextures->addItem(SETUP->HUD_FOLDER + "splash.png", "splash");
    HUDTextures->addItem(SETUP->HUD_FOLDER + "hud_health.png", "health");
    HUDTextures->addItem(SETUP->HUD_FOLDER + "hud_ammo.png", "ammoType");
    HUDTextures->addItem(SETUP->HUD_FOLDER + "hud.png", "hud");
    HUDTextures->addItem(SETUP->HUD_FOLDER + "loading.png", "loading");

    iconsTextures->addItem(SETUP->HUD_FOLDER + "flare.png", "flare");
    iconsTextures->addItem(SETUP->HUD_FOLDER + "plague.png", "plague");
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

    for (int i = 0; i < buttons.size(); i++) {
        buttons[i]->draw();
    }
}

const std::vector<Button *> &ComponentHUD::getButtons() const {
    return buttons;
}

void ComponentHUD::addButton(Button *button) {
    this->buttons.push_back(button);
}

void ComponentHUD::loadButtons() {
    const int offsetY =  EngineSetup::get()->screenHeight - 16;
    const int offsetX = 16;
    int currentX = 0;
    /*addButton(new Button(currentX + offsetX * 0, offsetY, SETUP->HUD_FOLDER + "flare.png", &callbackPlayerShoot));
    addButton(new Button(currentX + offsetX * 1, offsetY, SETUP->HUD_FOLDER + "plague.png", &callbackPlayerShoot2));
    addButton(new Button(currentX + offsetX * 2, offsetY, SETUP->HUD_FOLDER + "cold-fire.png", &callbackPlayerShoot2));
    addButton(new Button(currentX + offsetX * 3, offsetY, SETUP->HUD_FOLDER + "earth-1.png", &callbackPlayerShoot2));
    addButton(new Button(currentX + offsetX * 4, offsetY, SETUP->HUD_FOLDER + "needles.png", &callbackPlayerShoot2));
    addButton(new Button(currentX + offsetX * 5, offsetY, SETUP->HUD_FOLDER + "nova-mega.png", &callbackPlayerShoot2));
    addButton(new Button(currentX + offsetX * 6, offsetY, SETUP->HUD_FOLDER + "thunder-2.png", &callbackPlayerShoot2));*/
}

