#include "../../include/Components/ComponentHUD.h"
#include "../../include/ComponentsManager.h"
#include "../../include/2D/ButtonsCallbacks.h"
#include "../../include/Brakeza3D.h"

ComponentHUD::ComponentHUD() {
    HUDTextures = new TexturePackage();
}

void ComponentHUD::onStart() {
    Logging::Log("ComponentHUD onStart", "ComponentHUD");

    loadImages();
    loadButtons();

    textWriter = new TextWriter(
        ComponentsManager::get()->getComponentWindow()->renderer,
        std::string(SETUP->SPRITES_FOLDER + "conchars2.png").c_str()
    );

    shaderBar = new ShaderHorizontalBar(Color(255, 0, 0),0.90);
    shaderBar->setPhaseRender(EngineSetup::ShadersPhaseRender::PREUPDATE);

    shaderBarEnergy = new ShaderHorizontalBar(Color(0, 255, 0),0.95);
    shaderBarEnergy->setPhaseRender(EngineSetup::ShadersPhaseRender::PREUPDATE);

    shaderSelectedEnemyStamina = new ShaderHorizontalBar(Color(128, 0, 128),0.05);
    shaderSelectedEnemyStamina->setPhaseRender(EngineSetup::ShadersPhaseRender::PREUPDATE);
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

void ComponentHUD::loadImages() {
    HUDTextures->addItem(SETUP->HUD_FOLDER + "splash.png", "splash");
    HUDTextures->addItem(SETUP->HUD_FOLDER + "hud.png", "hud");
    HUDTextures->addItem(SETUP->HUD_FOLDER + "loading.png", "loading");
    HUDTextures->addItem(SETUP->HUD_FOLDER + "health_bar_empty.png", "healthEmptyBar");
    HUDTextures->addItem(SETUP->HUD_FOLDER + "health_bar_stamina.png", "healthBarStaminaPercent");
    HUDTextures->addItem(SETUP->HUD_FOLDER + "health_bar_energy.png", "healthBarEnergyPercent");
    HUDTextures->addItem(SETUP->ICONS_FOLDER + "gravitational_shield.png", "gravitationalShield");

}

void ComponentHUD::drawHUD()
{
    auto componentManager = ComponentsManager::get();

    drawShaderBars();
    drawEnemySelectedShaderStamina();

    drawPlayerStamina(15);

    if (SETUP->DRAW_FPS) {
        this->textWriter->writeTTFCenterHorizontal(
            15,
            std::to_string(componentManager->getComponentRender()->fps).c_str(),
            Color::red(),
            0.5
        );
    }

    for (auto & button : buttons) {
        button->draw();
    }

    if (ComponentsManager::get()->getComponentGame()->getGameState() != EngineSetup::GameState::GAMING) {
        this->textWriter->writeTTFCenterHorizontal(
            40,
            componentManager->getComponentGame()->getLevelInfo()->getLevelName().c_str(),
            Color::red(),
            0.5
        );
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

void ComponentHUD::drawPlayerStamina(int y)
{
    auto game = ComponentsManager::get()->getComponentGame();
    auto player = game->getPlayer();

    player->getWeapon()->getIcon()->drawFlat(455, 420);

    textWriter->writeTextTTFAutoSize(
        510,
        430,
        std::to_string(player->getWeapon()->getAmmoAmount()).c_str(),
        Color::green(),
        0.75
    );

    if (player->isAllowGravitationalShields()) {
        auto gravitationalShieldImage = HUDTextures->getTextureByLabel("gravitationalShield")->getImage();
        int gravitationalShieldsNumber = MAX_GRAVITATIONAL_SHIELDS - (int) player->getGravityShieldsNumber();
        for (int i = 0; i < gravitationalShieldsNumber; i++) {
            gravitationalShieldImage->drawFlat(60 + gravitationalShieldImage->width() * i, 400);
        }
    }
}

int ComponentHUD::getButtonsOffsetY() {
    const unsigned int innerMargin = 0;
    const int offsetY = SETUP->screenHeight - 16 - innerMargin;
    return offsetY;
}

void ComponentHUD::drawShaderBars()
{
    float fixedWidth = 0.6;
    float health =  ((ComponentsManager::get()->getComponentGame()->getPlayer()->getStamina() * fixedWidth) / (int) ComponentsManager::get()->getComponentGame()->getPlayer()->getStartStamina());
    float energy =  ((ComponentsManager::get()->getComponentGame()->getPlayer()->getEnergy() * fixedWidth) / (int) ComponentsManager::get()->getComponentGame()->getPlayer()->getStartEnergy());

    shaderBar->setValue(health);
    shaderBarEnergy->setValue(energy);

    shaderBar->update();
    shaderBarEnergy->update();
}

void ComponentHUD::drawEnemySelectedShaderStamina()
{
    auto objectSelected = ComponentsManager::get()->getComponentRender()->getSelectedObject();

    auto enemy = dynamic_cast<EnemyGhost*> (objectSelected);
    if (enemy == nullptr) {
        return;
    }

    float fixedWidth = 0.6;
    float health =  ((enemy->getStamina() * fixedWidth) / (int) enemy->getStartStamina());

    shaderSelectedEnemyStamina->setValue(health);
    shaderSelectedEnemyStamina->update();

}

TextWriter *ComponentHUD::getTextWriter() const {
    return textWriter;
}
