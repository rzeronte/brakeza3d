#include "ComponentHUD.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"

ComponentHUD::ComponentHUD()
:
    shaderLasers(nullptr),
    HUDTextures(nullptr),
    globalOffset(Point2D(110, 672)),
    offsetIcons(Point2D(-16, -13)),
    staminaOffset(Point2D(0, 0)),
    energyOffset(Point2D(474, 0)),
    staminaEnemyOffset(Point2D(146, -615)),
    avatarEnemyOffset(Point2D(-28, -625)),
    iconHealthOffset(Point2D(-7, -9)),
    iconEnergyOffset(Point2D(462, -11)),
    resourcesOffset(Point2D(869, -14))
{
}

void ComponentHUD::onStart()
{
    Logging::head("ComponentHUD onStart");

    HUDTextures = new TexturePackage();
    shaderLasers = new FXLaser(new Image(EngineSetup::get()->IMAGES_FOLDER + "noiseCloud.png"));
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
    const float sizeAmounts = 1.0f;
    const int gatheringResourcesOffsetX = globalOffset.x + resourcesOffset.x;
    const int gatheringResourcesOffsetY = globalOffset.y + resourcesOffset.y;

    auto game = ComponentsManager::get()->getComponentGame();
    auto textWriter = game->getTextWriter();
    auto player = game->getPlayer();

    auto fb = ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer();

    //coins
    HUDTextures->getTextureByLabel("coinIcon")->drawFlatAlpha(
        gatheringResourcesOffsetX,
        gatheringResourcesOffsetY,
        255,
        fb
    );

    textWriter->writeTextTTFAutoSize(
        gatheringResourcesOffsetX + 42,
        gatheringResourcesOffsetY,
        (std::string("x") + std::to_string(player->getCoins())).c_str(),
        PaletteColors::getMenuOptions(),
        sizeAmounts
    );

    // human
    HUDTextures->getTextureByLabel("astrounautIcon")->drawFlatAlpha(
        gatheringResourcesOffsetX + 145,
        gatheringResourcesOffsetY,
        255,
        fb
    );
    textWriter->writeTextTTFAutoSize(
        gatheringResourcesOffsetX + 190,
        gatheringResourcesOffsetY,
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

    HUDTextures->getTextureByLabel("staminaIcon")->drawFlatAlpha(
        globalOffset.x + iconHealthOffset.x,
        globalOffset.y + iconHealthOffset.y,
        textWriter->getAlpha(),
        fb
    );

    HUDTextures->getTextureByLabel("energyIcon")->drawFlatAlpha(
        globalOffset.x + iconEnergyOffset.x,
        globalOffset.y + iconEnergyOffset.y,
        textWriter->getAlpha(),
        fb
    );

    auto player = game->getPlayer();
    const float sizeAmounts = 0.26f;

    // weapon icon
    player->getWeapon()->getIcon()->drawFlatAlpha(globalOffset.x + offsetIcons.x + 270, globalOffset.y + offsetIcons.y, 255, fb);

    // ammo amount
    textWriter->writeTextTTFAutoSize(
        globalOffset.x + offsetIcons.x + 270,
        globalOffset.y + offsetIcons.y + player->getWeapon()->getIcon()->height(),
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
            globalOffset.x + offsetIcons.x + 330,
            globalOffset.y + offsetIcons.y + weaponBomb->getIcon()->height(),
            (std::string("x") + std::to_string(weaponBomb->getAmmoAmount())).c_str(),
            PaletteColors::getMenuOptions(),
            sizeAmounts
        );
    }
    weaponBomb->getIcon()->drawFlatAlpha(globalOffset.x + offsetIcons.x + 330, globalOffset.y + offsetIcons.y, bombAlpha, fb);

    // shield
    auto weaponShield = player->getWeaponTypeByLabel("shield");
    float shieldAlpha = 75;
    if (weaponShield->getAmmoAmount() > 0) {
        shieldAlpha = 255;
        textWriter->writeTextTTFAutoSize(
            globalOffset.x + offsetIcons.x + 360,
            globalOffset.y + offsetIcons.y + weaponShield->getIcon()->height(),
            (std::string("x") + std::to_string(weaponShield->getAmmoAmount())).c_str(),
            PaletteColors::getMenuOptions(),
            sizeAmounts
        );
    }
    weaponShield->getIcon()->drawFlatAlpha(globalOffset.x + offsetIcons.x + 360, globalOffset.y + offsetIcons.y, shieldAlpha, fb);

    // level number
    textWriter->writeTextTTFAutoSize(
        globalOffset.x + offsetIcons.x + 388,
        globalOffset.y + offsetIcons.y,
        game->getLevelLoader()->getLevelName().c_str(),
        PaletteColors::getMenuOptions(),
        1.0f
    );

    // icon player reflection
    auto weaponReflection = player->getWeaponTypeByLabel("reflection");
    float reflectionAlpha = 75;
    if (weaponReflection->getAmmoAmount() > 0) {
        reflectionAlpha = 255;
        textWriter->writeTextTTFAutoSize(
            globalOffset.x + offsetIcons.x + 305,
            globalOffset.y + offsetIcons.y + weaponReflection->getIcon()->height(),
            (std::string("x") + std::to_string(weaponReflection->getAmmoAmount())).c_str(),
            PaletteColors::getMenuOptions(),
            sizeAmounts
        );
    }

    weaponReflection->getIcon()->drawFlatAlpha(
        globalOffset.x + offsetIcons.x + 300,
        globalOffset.y + offsetIcons.y,
        reflectionAlpha, fb
    );

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
        enemy->getAvatar()->drawFlatAlpha(
            globalOffset.x + avatarEnemyOffset.x,
            globalOffset.y + avatarEnemyOffset.y,
            255,
            fb
        );

        textWriter->writeTextTTFAutoSize(
            globalOffset.x + avatarEnemyOffset.x,
            globalOffset.y + avatarEnemyOffset.y + enemy->getAvatar()->height() + 5,
            enemy->getName().c_str(),
            PaletteColors::getStamina(),
            1.0f
        );
    } else {
        HUDTextures->getTextureByLabel("emptyEnemy")->drawFlatAlpha(
            globalOffset.x + avatarEnemyOffset.x,
            globalOffset.y + avatarEnemyOffset.y,
            144,
            fb
        );

        textWriter->writeTextTTFAutoSize(
            globalOffset.x + avatarEnemyOffset.x,
            globalOffset.y + avatarEnemyOffset.y + (int) HUDTextures->getTextureByLabel("emptyEnemy")->height() + 5,
            "No target",
            PaletteColors::getStamina(),
            1.0f
        );
    }
}

void ComponentHUD::drawShaderLasers()
{
    const auto player = ComponentsManager::get()->getComponentGame()->getPlayer();
    const float fixedWidth = 1.0f;
    const float health = ((float) player->getStamina() * fixedWidth) / (float) player->getStartStamina();
    const float energy = (player->getEnergy() * fixedWidth) / player->getStartEnergy();

    const int startPositionX = globalOffset.x + staminaOffset.x;
    const int width = 195.0f;

    // stamina
    shaderLasers->addLaser(
        glm::vec2(startPositionX, globalOffset.y + staminaOffset.y),
        glm::vec2(startPositionX + (int) (width * health), globalOffset.y + staminaOffset.y),
        PaletteColors::getStamina().toGLM(),
        strokeBars,
        intensityBars,
        0.05f
    );

    // energy
    const int startSecondLaserX = globalOffset.x + energyOffset.x;
    shaderLasers->addLaser(
        glm::vec2(startSecondLaserX, globalOffset.y + energyOffset.y),
        glm::vec2(startSecondLaserX + (int) (width * energy), globalOffset.y + energyOffset.y),
        PaletteColors::getEnergy().toGLM(),
        strokeBars,
        intensityBars,
        0.05f
    );

    // Enemy stamina
    auto objectSelected = ComponentsManager::get()->getComponentRender()->getSelectedObject();
    auto enemy = dynamic_cast<EnemyGhost*>(objectSelected);
    if (enemy != nullptr) {
        const float enemyHealth = (enemy->getStamina() * fixedWidth) / enemy->getStartStamina();

        const int positionLaserX = globalOffset.x + staminaEnemyOffset.x;
        const int positionLaserY = globalOffset.y + staminaEnemyOffset.y;
        const int width = 146;

        shaderLasers->addLaser(
            glm::vec2(positionLaserX, positionLaserY),
            glm::vec2(positionLaserX - (int)(width * enemyHealth), positionLaserY),
            PaletteColors::getStamina().toGLM(),
            strokeBars,
            intensityBars,
            0.05f
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
