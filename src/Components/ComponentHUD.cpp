#include "../../include/Components/ComponentHUD.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"

ComponentHUD::ComponentHUD()
:
    shaderLasers(nullptr),
    HUDTextures(nullptr)
{
}

void ComponentHUD::onStart()
{
    Logging::Log("ComponentHUD onStart");

    HUDTextures = new TexturePackage();

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
}

void ComponentHUD::onUpdate()
{
    if (!isEnabled()) return;
     const float alpha = ComponentsManager::get()->getComponentGame()->getTextWriter()->getAlpha();

    HUDTextures->getTextureByLabel("hudBackground")->getImage()->drawFlatAlpha(0, 0, alpha);

    auto stamina = HUDTextures->getTextureByLabel("staminaIcon")->getImage();
    auto energy = HUDTextures->getTextureByLabel("energyIcon")->getImage();

    stamina->drawFlatAlpha(40, this->offsetY, alpha);
    energy->drawFlatAlpha(40, this->offsetY + playerBarSeparation, alpha);

    drawShaderLasers();

    if (SETUP->DRAW_CROSSHAIR) {
        Drawable::drawCrossHair();
    }
}

void ComponentHUD::postUpdate()
{
    if (!isEnabled()) return;

    drawHUD();
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
    HUDTextures->addItem(SETUP->IMAGES_FOLDER + "medals/shaded_medal_bronze.png", "medalBronze");
    HUDTextures->addItem(SETUP->IMAGES_FOLDER + "medals/shaded_medal_silver.png", "medalSilver");
    HUDTextures->addItem(SETUP->IMAGES_FOLDER + "medals/shaded_medal_gold.png", "medalGold");
    HUDTextures->addItem(SETUP->ICONS_FOLDER + "energy.png", "energyIcon");
    HUDTextures->addItem(SETUP->ICONS_FOLDER + "stamina.png", "staminaIcon");
    HUDTextures->addItem(SETUP->ICONS_FOLDER + "coin.png", "coinIcon");
    HUDTextures->addItem(SETUP->ICONS_FOLDER + "astronaut.png", "astrounautIcon");
}

void ComponentHUD::drawHUD()
{
    auto componentManager = ComponentsManager::get();
    auto textWriter = componentManager->getComponentGame()->getTextWriter();

    drawIconWeaponAndLevelName();

    if (SETUP->DRAW_FPS) {
        textWriter->writeTTFCenterHorizontal(
            10,
            std::to_string(componentManager->getComponentRender()->getFps()).c_str(),
            componentManager->getComponentGame()->getPrimaryColor(),
            0.3
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
    auto textWriter = game->getTextWriter();
    auto player = game->getPlayer();


    //coins
    auto iconCoin = HUDTextures->getTextureByLabel("coinIcon");
    iconCoin->getImage()->drawFlatAlpha(40, offsetY - 16, 255);
    textWriter->writeTextTTFAutoSize(
            40 + 16,
            this->offsetY - 17,
            (std::string("x") + std::to_string(player->getCoins())).c_str(),
            game->getPrimaryColor(),
            0.25
    );

    // weapon icon
    player->getWeapon()->getIcon()->drawFlatAlpha(270, this->offsetY, 255);

    // ammo amount
    textWriter->writeTextTTFAutoSize(
        270,
        this->offsetY + player->getWeapon()->getIcon()->height(),
        (std::string("x") + std::to_string(player->getWeapon()->getAmmoAmount())).c_str(),
        game->getPrimaryColor(),
        0.25
    );

    // bomb icon
    auto weaponBomb = player->getWeaponTypeByLabel("bomb");
    float bombAlpha = 75;
    if (weaponBomb->getAmmoAmount() > 0) {
        bombAlpha = 255;
        // bomb ammo
        textWriter->writeTextTTFAutoSize(
            330,
            this->offsetY + weaponBomb->getIcon()->height(),
            (std::string("x") + std::to_string(weaponBomb->getAmmoAmount())).c_str(),
            game->getPrimaryColor(),
            0.25
        );
    }
    weaponBomb->getIcon()->drawFlatAlpha(330, this->offsetY, bombAlpha);

    // human
    auto humanIcon = HUDTextures->getTextureByLabel("astrounautIcon");
    humanIcon->getImage()->drawFlatAlpha(360, offsetY, 255);
    textWriter->writeTextTTFAutoSize(
        360,
        this->offsetY + humanIcon->getImage()->height(),
        (std::string("x") + std::to_string(player->getRescuedHumans())).c_str(),
        game->getPrimaryColor(),
        0.25
    );

    // level number
    textWriter->writeTextTTFAutoSize(
        390,
        this->offsetY - 10,
        game->getLevelLoader()->getLevelName().c_str(),
        game->getPrimaryColor(),
        0.7
    );

    // icon player reflection
    float reflectionAlpha = 75;
    auto reflectionImage = HUDTextures->getTextureByLabel("reflectionIcon")->getImage();
    int reflectionsNumber = MAX_REFLECTIONS - (int) player->getGravityShieldsNumber();
    if (player->isAllowedMakeReflections() && reflectionsNumber > 0) {
        reflectionAlpha = 255;

        textWriter->writeTextTTFAutoSize(
            300,
            this->offsetY + reflectionImage->height(),
            (std::string("x") + std::to_string(reflectionsNumber)).c_str(),
            game->getPrimaryColor(),
            0.25
        );
    }

    reflectionImage->drawFlatAlpha(300, this->offsetY, reflectionAlpha);

    ComponentsManager::get()->getComponentGame()->getStoreManager()->drawHUD(255);
}

int ComponentHUD::getButtonsOffsetY()
{
    const unsigned int innerMargin = 0;
    const int offsetY = (int) (SETUP->screenHeight - 16 - innerMargin);
    return offsetY;
}

void ComponentHUD::drawShaderLasers()
{
    const auto player = ComponentsManager::get()->getComponentGame()->getPlayer();
    const float fixedWidth = 1.0f;
    const float health = ((float) player->getStamina() * fixedWidth) / (float) player->getStartStamina();
    const float energy = (player->getEnergy() * fixedWidth) / player->getStartEnergy();

    const int startPositionX = 60.0f;
    const int width = 195.0f;
    const float stroke = 0.15f;
    const float topBarMargin = 5;

    shaderLasers->addLaser(
        startPositionX, this->offsetY + topBarMargin,
        startPositionX + (int) (width * health), this->offsetY + topBarMargin,
        255, 0, 0,
        stroke,
        false,
        false
    );

    shaderLasers->addLaser(
        startPositionX, this->offsetY + playerBarSeparation + topBarMargin,
        startPositionX + (int) (width * energy), this->offsetY + playerBarSeparation + topBarMargin,
        0, 255, 0,
        stroke,
        false,
        false
    );

    auto objectSelected = ComponentsManager::get()->getComponentRender()->getSelectedObject();

    auto enemy = dynamic_cast<EnemyGhost*>(objectSelected);
    if (enemy != nullptr) {
        const float enemyHealth = ((enemy->getStamina() * fixedWidth) / enemy->getStartStamina());

        const int positionLaserX = 372 + width;
        const int positionLaserY = this->offsetY + topBarMargin;
        const int width = 146;

        shaderLasers->addLaser(
                positionLaserX, positionLaserY,
                positionLaserX - (int)(width * enemyHealth),
                positionLaserY,
                255, 0, 0,
                stroke,
                false,
                false
        );

        enemy->getAvatar()->drawFlatAlpha(580, this->offsetY, 255);
    }

    shaderLasers->update();
}

ComponentHUD::~ComponentHUD()
{
    delete shaderLasers;
    delete HUDTextures;
}

TexturePackage *ComponentHUD::getHudTextures() const {
    return HUDTextures;
}

int ComponentHUD::getRadioMessagesCounter() const {
    return radioMessagesCounter;
}

void ComponentHUD::setRadioMessagesCounter(int radioMessagesCounter) {
    ComponentHUD::radioMessagesCounter = radioMessagesCounter;
}
