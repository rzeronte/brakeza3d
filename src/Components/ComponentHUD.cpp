#include "../../include/Components/ComponentHUD.h"
#include "../../include/ComponentsManager.h"
#include "../../include/2D/ButtonsCallbacks.h"
#include "../../include/Brakeza3D.h"

ComponentHUD::ComponentHUD() {
}

void ComponentHUD::onStart() {
    Logging::Log("ComponentHUD onStart", "ComponentHUD");

    HUDTextures = new TexturePackage();

    textWriter = new TextWriter(
        ComponentsManager::get()->getComponentWindow()->getRenderer(),
        ComponentsManager::get()->getComponentWindow()->getFontDefault(),
        std::string(SETUP->SPRITES_FOLDER + "conchars2.png").c_str()
    );

    shaderLasers = new ShaderProjectiles();
    shaderLasers->setEnabled(true);

    loadImages();
    loadButtons();
}

void ComponentHUD::preUpdate()
{
    if (!isEnabled()) {
        return;
    }

    if (SETUP->DRAW_CROSSHAIR) {
        Drawable::drawCrossHair();
    }


}

void ComponentHUD::onUpdate()
{
    if (!isEnabled()) return;

    drawHUD();
}

void ComponentHUD::postUpdate()
{
    if (!isEnabled()) return;

}

void ComponentHUD::onEnd()
{

}

void ComponentHUD::onSDLPollEvent(SDL_Event *event, bool &finish)
{
    for(auto & button : buttons) {
        button->onUpdate();
    }
}

void ComponentHUD::loadImages()
{
    HUDTextures->addItem(SETUP->HUD_FOLDER + "splash.png", "splash");
    HUDTextures->addItem(SETUP->HUD_FOLDER + "loading.png", "loading");
    HUDTextures->addItem(SETUP->ICONS_FOLDER + "gravitational_shield.png", "reflectionIcon");
    HUDTextures->addItem(SETUP->IMAGES_FOLDER + "hud_background.png", "hudBackground");
}

void ComponentHUD::drawHUD()
{
    auto componentManager = ComponentsManager::get();

    drawIconWeaponAndLevelName();
    drawShaderLasers();

    HUDTextures->getTextureByLabel("hudBackground")->getImage()->drawFlat(0, 0);

    if (SETUP->DRAW_FPS) {
        this->textWriter->writeTTFCenterHorizontal(
            15,
            std::to_string(componentManager->getComponentRender()->getFps()).c_str(),
            componentManager->getComponentGame()->getPrimaryColor(),
            0.5
        );
    }

    for (auto & button : buttons) {
        button->draw();
    }
}

const std::vector<Button *> &ComponentHUD::getButtons() const {
    return buttons;
}

void ComponentHUD::addButton(Button *button) {
    this->buttons.push_back(button);
}

void ComponentHUD::loadButtons() {
    const int offsetY = getButtonsOffsetY();
    const int offsetX = 16;
    int currentX = 1;
    //addButton(new Button(currentX + offsetX, offsetY, SETUP->ASSETS_FOLDER + "icons/weapon_instant.png", &callbackPlayerShoot));
    //addButton(new Button(currentX + offsetX * 1, offsetY, SETUP->ASSETS_FOLDER + "icons/weapon_instant.png", &callbackPlayerShoot2));
    //addButton(new Button(currentX + offsetX * 2, offsetY, SETUP->ASSETS_FOLDER + "icons/weapon_instant.png", &callbackPlayerShoot2));
}

void ComponentHUD::drawIconWeaponAndLevelName()
{
    auto game = ComponentsManager::get()->getComponentGame();
    auto player = game->getPlayer();

    player->getWeapon()->getIcon()->drawFlat(470, 440);

    textWriter->writeTextTTFAutoSize(
        510,
        435,
        std::to_string(player->getWeapon()->getAmmoAmount()).c_str(),
        game->getSecondaryColor(),
        0.50
    );

    textWriter->writeTextTTFAutoSize(
        510,
        462,
        game->getLevelInfo()->getLevelName().c_str(),
        game->getPrimaryColor(),
        0.25
    );

    if (player->isAllowedMakeReflections()) {
        auto reflectionImage = HUDTextures->getTextureByLabel("reflectionIcon")->getImage();
        int reflectionsNumber = MAX_REFLECTIONS - (int) player->getGravityShieldsNumber();
        for (int i = 0; i < reflectionsNumber; i++) {
            reflectionImage->drawFlat(5 + reflectionImage->width() * i, 440);
        }
    }
}

int ComponentHUD::getButtonsOffsetY() {
    const unsigned int innerMargin = 0;
    const int offsetY = SETUP->screenHeight - 16 - innerMargin;
    return offsetY;
}

void ComponentHUD::drawShaderLasers()
{
    const auto player = ComponentsManager::get()->getComponentGame()->getPlayer();
    const float fixedWidth = 1.0;
    const float health = (player->getStamina() * fixedWidth) / player->getStartStamina();
    const float energy = (player->getEnergy() * fixedWidth) / player->getStartEnergy();

    const int startPositionX = 110;
    const int width = 350;
    const float stroke = 0.05;

    shaderLasers->addLaser(
        startPositionX, 445,
        startPositionX + (width * health), 445 ,
        255, 0, 0,
        stroke
    );

    shaderLasers->addLaser(
        startPositionX, 465,
        startPositionX + (width * energy), 465,
        0, 255, 0,
        stroke
    );

    auto objectSelected = ComponentsManager::get()->getComponentRender()->getSelectedObject();

    auto enemy = dynamic_cast<EnemyGhost*> (objectSelected);
    if (enemy != nullptr) {
        const float enemyHealth = ((enemy->getStamina() * fixedWidth) / enemy->getStartStamina());

        shaderLasers->addLaser(
            startPositionX, 25,
            startPositionX + (width * enemyHealth), 25,
            255, 0, 255,
            stroke
        );
    }

    shaderLasers->update();
}


TextWriter *ComponentHUD::getTextWriter() const {
    return textWriter;
}

ComponentHUD::~ComponentHUD()
{
    delete shaderLasers;
    delete textWriter;
    delete HUDTextures;
}
