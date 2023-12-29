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
    Logging::head("ComponentHUD onStart");

    HUDTextures = new TexturePackage();

    shaderLasers = new FXLaser();
    shaderLasers->setEnabled(true);

    loadImages();
    loadButtons();
}

void ComponentHUD::preUpdate()
{
    if (!isEnabled()) return;

    HUDTextures->getTextureByLabel("hudBackground")->drawFlatAlpha(
        0,
        0,
        ComponentsManager::get()->getComponentGame()->getTextWriter()->getAlpha(),
        ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer()
    );
}

void ComponentHUD::onUpdate()
{
    if (!isEnabled()) return;

    drawIconWeaponsAndLevelName();
    drawEnemyIconAndName();
    drawGhateringResources();

    shaderLasers->update();
}

void ComponentHUD::postUpdate()
{
    if (!isEnabled()) return;

    drawHUD();
    drawShaderLasers();
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
    for (auto & button : buttons) {
        button->draw(ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer());
    }
}

const std::vector<Button *> &ComponentHUD::getButtons() const {
    return buttons;
}

void ComponentHUD::addButton(Button *button) {
    this->buttons.push_back(button);
}

void ComponentHUD::loadButtons()
{
    const int offsetY = getButtonsOffsetY();
    const int offsetX = 16;
    int currentX = 1;
    //addButton(new Button(currentX + offsetX, offsetY, SETUP->ASSETS_FOLDER + "icons/weapon_instant.png", &callbackPlayerShoot));
    //addButton(new Button(currentX + offsetX * 1, offsetY, SETUP->ASSETS_FOLDER + "icons/weapon_instant.png", &callbackPlayerShoot2));
    //addButton(new Button(currentX + offsetX * 2, offsetY, SETUP->ASSETS_FOLDER + "icons/weapon_instant.png", &callbackPlayerShoot2));
}
void ComponentHUD::drawGhateringResources()
{
    const float sizeAmounts = 0.26f;
    const int gatheringResourcesOffsetX = 530;
    const int gatheringResourcesOffsetY = 13;
    auto game = ComponentsManager::get()->getComponentGame();
    auto textWriter = game->getTextWriter();
    auto player = game->getPlayer();

    auto fb = ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer();
    //coins
    HUDTextures->getTextureByLabel("coinIcon")->drawFlatAlpha(gatheringResourcesOffsetX, gatheringResourcesOffsetY, 255, fb);

    textWriter->writeTextTTFAutoSize(
            gatheringResourcesOffsetX + 16,
            gatheringResourcesOffsetY,
            (std::string("x") + std::to_string(player->getCoins())).c_str(),
            PaletteColors::getMenuOptions(),
            sizeAmounts
    );

    // human
    HUDTextures->getTextureByLabel("astrounautIcon")->drawFlatAlpha(gatheringResourcesOffsetX + 40, 13, 255, fb);
    textWriter->writeTextTTFAutoSize(
            gatheringResourcesOffsetX + 54,
            13,
            (std::string("x") + std::to_string(player->getRescuedHumans())).c_str(),
            PaletteColors::getMenuOptions(),
            sizeAmounts
    );
}

void ComponentHUD::drawIconWeaponsAndLevelName() const
{
    auto game = ComponentsManager::get()->getComponentGame();
    auto textWriter = game->getTextWriter();
    auto fb = ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer();

    textWriter->setFont(ComponentsManager::get()->getComponentWindow()->getFontDefault());

    HUDTextures->getTextureByLabel("staminaIcon")->drawFlatAlpha(this->offsetX + 38, this->offsetY - 2, textWriter->getAlpha(), fb);
    HUDTextures->getTextureByLabel("energyIcon")->drawFlatAlpha(this->offsetX + 38, this->offsetY + playerBarSeparation, textWriter->getAlpha(), fb);

    auto player = game->getPlayer();
    const float sizeAmounts = 0.26f;

    // weapon icon
    player->getWeapon()->getIcon()->drawFlatAlpha(this->offsetX + 270, this->offsetY, 255, fb);

    // ammo amount
    textWriter->writeTextTTFAutoSize(
        this->offsetX + 270,
        this->offsetY + player->getWeapon()->getIcon()->height(),
        (std::string("x") + std::to_string(player->getWeapon()->getAmmoAmount())).c_str(),
        PaletteColors::getMenuOptions(),
        sizeAmounts
    );

    // bomb icon
    auto weaponBomb = player->getWeaponTypeByLabel("bomb");
    float bombAlpha = 75;
    if (weaponBomb->getAmmoAmount() > 0) {
        bombAlpha = 255;
        // bomb ammo
        textWriter->writeTextTTFAutoSize(
            this->offsetX + 330,
            this->offsetY + weaponBomb->getIcon()->height(),
            (std::string("x") + std::to_string(weaponBomb->getAmmoAmount())).c_str(),
            PaletteColors::getMenuOptions(),
            sizeAmounts
        );
    }
    weaponBomb->getIcon()->drawFlatAlpha(this->offsetX + 330, this->offsetY, bombAlpha, fb);

    // shield
    auto weaponShield = player->getWeaponTypeByLabel("shield");
    float shieldAlpha = 75;
    if (weaponShield->getAmmoAmount() > 0) {
        shieldAlpha = 255;
        textWriter->writeTextTTFAutoSize(
            this->offsetX + 360,
            this->offsetY + weaponShield->getIcon()->height(),
            (std::string("x") + std::to_string(weaponShield->getAmmoAmount())).c_str(),
            PaletteColors::getMenuOptions(),
            sizeAmounts
        );
    }
    weaponShield->getIcon()->drawFlatAlpha(this->offsetX + 360, this->offsetY, shieldAlpha, fb);

    // level number
    textWriter->writeTextTTFAutoSize(
        this->offsetX + 388,
        this->offsetY,
        game->getLevelLoader()->getLevelName().c_str(),
        PaletteColors::getMenuOptions(),
        0.9f
    );

    // icon player reflection
    auto weaponReflection = player->getWeaponTypeByLabel("reflection");
    float reflectionAlpha = 75;
    if (weaponReflection->getAmmoAmount() > 0) {
        reflectionAlpha = 255;
        textWriter->writeTextTTFAutoSize(
            this->offsetX + 305,
            this->offsetY + weaponReflection->getIcon()->height(),
            (std::string("x") + std::to_string(weaponReflection->getAmmoAmount())).c_str(),
            PaletteColors::getMenuOptions(),
            sizeAmounts
        );
    }
    weaponReflection->getIcon()->drawFlatAlpha(this->offsetX + 300, this->offsetY, reflectionAlpha, fb);

    ComponentsManager::get()->getComponentGame()->getStoreManager()->drawBoughtItemsOnHUD(255);
}

int ComponentHUD::getButtonsOffsetY()
{
    const unsigned int innerMargin = 0;
    const int offsetY = (int) (SETUP->screenHeight - 16 - innerMargin);
    return offsetY;
}

void ComponentHUD::drawEnemyIconAndName()
{
    auto objectSelected = ComponentsManager::get()->getComponentRender()->getSelectedObject();
    auto textWriter = ComponentsManager::get()->getComponentGame()->getTextWriter();
    auto fb = ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer();

    auto enemy = dynamic_cast<EnemyGhost*>(objectSelected);
    if (enemy != nullptr) {
        enemy->getAvatar()->drawFlatAlpha(this->offsetX + 582, this->offsetY, 255, fb);

        textWriter->writeTextTTFAutoSize(
                this->offsetX + 424,
                this->offsetY + 14,
                enemy->getName().c_str(),
                PaletteColors::getStamina(),
                0.3
        );
    } else {
        HUDTextures->getTextureByLabel("emptyEnemy")->drawFlatAlpha(this->offsetX + 580, this->offsetY, 144, fb);
        textWriter->writeTextTTFAutoSize(
                this->offsetX + 526,
                this->offsetY + 14,
                "No target",
                PaletteColors::getStamina(),
                0.3
        );
    }
}

void ComponentHUD::drawShaderLasers()
{
    const auto player = ComponentsManager::get()->getComponentGame()->getPlayer();
    const float fixedWidth = 1.0f;
    const float health = ((float) player->getStamina() * fixedWidth) / (float) player->getStartStamina();
    const float energy = (player->getEnergy() * fixedWidth) / player->getStartEnergy();

    const int startPositionX = this->offsetX + 60.0f;
    const int width = 195.0f;
    const float stroke = 0.002f;
    const float topBarMargin = 5;

    // stamina
    shaderLasers->addLaser(
        glm::vec2(startPositionX, offsetY + topBarMargin),
        glm::vec2(startPositionX + (int) (width * health), offsetY + topBarMargin),
        PaletteColors::getStamina().toGLM(),
        stroke,
        false,
        false
    );

    // energy
    shaderLasers->addLaser(
        glm::vec2(startPositionX, this->offsetY + playerBarSeparation + topBarMargin),
        glm::vec2(startPositionX + (int) (width * energy), this->offsetY + playerBarSeparation + topBarMargin),
        PaletteColors::getEnergy().toGLM(),
        stroke,
        false,
        false
    );

    // Enemy stamina
    auto objectSelected = ComponentsManager::get()->getComponentRender()->getSelectedObject();
    auto enemy = dynamic_cast<EnemyGhost*>(objectSelected);
    if (enemy != nullptr) {
        const float enemyHealth = ((enemy->getStamina() * fixedWidth) / enemy->getStartStamina());

        const int positionLaserX = this->offsetX + 377 + width;
        const int positionLaserY = this->offsetY + topBarMargin;
        const int width = 146;

        shaderLasers->addLaser(
            glm::vec2(positionLaserX, positionLaserY),
            glm::vec2(positionLaserX - (int)(width * enemyHealth), positionLaserY),
            PaletteColors::getStamina().toGLM(),
            stroke,
            false,
            false
        );
    }
}

ComponentHUD::~ComponentHUD()
{
    delete shaderLasers;
    delete HUDTextures;
}

TexturePackage *ComponentHUD::getHudTextures() const {
    return HUDTextures;
}

FXLaser *ComponentHUD::getShaderLasers() const {
    return shaderLasers;
}
