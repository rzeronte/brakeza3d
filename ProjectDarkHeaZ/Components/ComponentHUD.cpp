#include "ComponentHUD.h"
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

    stamina->drawFlatAlpha(38, this->offsetY - 2, alpha);
    energy->drawFlatAlpha(38, this->offsetY + playerBarSeparation, alpha);

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
    HUDTextures->addItem(SETUP->ICONS_FOLDER + "weapon_reflection.png", "reflectionIcon");
    HUDTextures->addItem(SETUP->IMAGES_FOLDER + "hud_background.png", "hudBackground");
    HUDTextures->addItem(SETUP->IMAGES_FOLDER + "medals/shaded_medal_bronze.png", "medalBronze");
    HUDTextures->addItem(SETUP->IMAGES_FOLDER + "medals/shaded_medal_silver.png", "medalSilver");
    HUDTextures->addItem(SETUP->IMAGES_FOLDER + "medals/shaded_medal_gold.png", "medalGold");
    HUDTextures->addItem(SETUP->ICONS_FOLDER + "energy.png", "energyIcon");
    HUDTextures->addItem(SETUP->ICONS_FOLDER + "stamina.png", "staminaIcon");
    HUDTextures->addItem(SETUP->ICONS_FOLDER + "coin.png", "coinIcon");
    HUDTextures->addItem(SETUP->ICONS_FOLDER + "astronaut.png", "astrounautIcon");
    HUDTextures->addItem(SETUP->ICONS_FOLDER + "empty_enemy.png", "emptyEnemy");
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
            componentManager->getComponentGame()->getPalette().getFirst(),
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
    const int gatheringResourcesOffsetX = 530;
    const int gatheringResourcesOffsetY = 13;

    //coins
    auto iconCoin = HUDTextures->getTextureByLabel("coinIcon");
    iconCoin->getImage()->drawFlatAlpha(gatheringResourcesOffsetX, gatheringResourcesOffsetY, 255);
    textWriter->writeTextTTFAutoSize(
        gatheringResourcesOffsetX + 16,
        gatheringResourcesOffsetY,
        (std::string("x") + std::to_string(player->getCoins())).c_str(),
        game->getPalette().getFirst(),
        0.25
    );

    // human
    auto humanIcon = HUDTextures->getTextureByLabel("astrounautIcon");
    humanIcon->getImage()->drawFlatAlpha(gatheringResourcesOffsetX + 40, 13, 255);
    textWriter->writeTextTTFAutoSize(
        gatheringResourcesOffsetX + 54,
        13,
        (std::string("x") + std::to_string(player->getRescuedHumans())).c_str(),
        game->getPalette().getFirst(),
        0.25
    );

    // weapon icon
    player->getWeapon()->getIcon()->drawFlatAlpha(270, this->offsetY, 255);

    // ammo amount
    textWriter->writeTextTTFAutoSize(
        270,
        this->offsetY + player->getWeapon()->getIcon()->height(),
        (std::string("x") + std::to_string(player->getWeapon()->getAmmoAmount())).c_str(),
        game->getPalette().getFirst(),
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
            game->getPalette().getFirst(),
            0.25
        );
    }
    weaponBomb->getIcon()->drawFlatAlpha(330, this->offsetY, bombAlpha);

    // shield
    auto weaponShield = player->getWeaponTypeByLabel("shield");
    float shieldAlpha = 75;
    if (weaponShield->getAmmoAmount() > 0) {
        shieldAlpha = 255;
        textWriter->writeTextTTFAutoSize(
            360,
            this->offsetY + weaponShield->getIcon()->height(),
            (std::string("x") + std::to_string(weaponShield->getAmmoAmount())).c_str(),
            game->getPalette().getFirst(),
            0.25
        );
    }
    weaponShield->getIcon()->drawFlatAlpha(360, this->offsetY, bombAlpha);

    // level number
    textWriter->writeTextTTFAutoSize(
        390,
        this->offsetY - 10,
        game->getLevelLoader()->getLevelName().c_str(),
        game->getPalette().getFirst(),
        0.7
    );

    // icon player reflection
    auto weaponReflection = player->getWeaponTypeByLabel("reflection");
    float reflectionAlpha = 75;
    if (weaponReflection->getAmmoAmount() > 0) {
        reflectionAlpha = 255;
        textWriter->writeTextTTFAutoSize(
            300,
            this->offsetY + weaponReflection->getIcon()->height(),
            (std::string("x") + std::to_string(weaponReflection->getAmmoAmount())).c_str(),
            game->getPalette().getFirst(),
            0.25
        );
    }
    weaponReflection->getIcon()->drawFlatAlpha(300, this->offsetY, reflectionAlpha);

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
        ComponentsManager::get()->getComponentGame()->getPalette().getStamina(),
        stroke,
        false,
        false
    );

    shaderLasers->addLaser(
        startPositionX, this->offsetY + playerBarSeparation + topBarMargin,
        startPositionX + (int) (width * energy), this->offsetY + playerBarSeparation + topBarMargin,
        ComponentsManager::get()->getComponentGame()->getPalette().getEnergy(),
        stroke,
        false,
        false
    );

    auto objectSelected = ComponentsManager::get()->getComponentRender()->getSelectedObject();
    auto textWriter = ComponentsManager::get()->getComponentGame()->getTextWriter();

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
            ComponentsManager::get()->getComponentGame()->getPalette().getStamina(),
            stroke,
            false,
            false
        );

        enemy->getAvatar()->drawFlatAlpha(580, this->offsetY, 255);

        textWriter->writeTextTTFAutoSize(
            418,
            this->offsetY + 11,
            enemy->getName().c_str(),
            ComponentsManager::get()->getComponentGame()->getPalette().getStamina(),
            0.25
        );
    } else {
        HUDTextures->getTextureByLabel("emptyEnemy")->getImage()->drawFlatAlpha(580, this->offsetY, 144);
        textWriter->writeTextTTFAutoSize(
            537,
            this->offsetY + 10,
            "No target",
            ComponentsManager::get()->getComponentGame()->getPalette().getStamina(),
            0.25
        );

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

ShaderProjectiles *ComponentHUD::getShaderLasers() const {
    return shaderLasers;
}
