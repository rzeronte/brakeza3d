#include "../../include/Components/ComponentHUD.h"
#include "../../include/ComponentsManager.h"
#include "../../include/2D/ButtonsCallbacks.h"
#include "../../include/Brakeza3D.h"
#include "../../include/Render/Transforms.h"

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
std::string(SETUP->SPRITES_FOLDER + SETUP->CONCHARS_SPRITE_FILE).c_str()
    );
}

void ComponentHUD::preUpdate() {

    if (!isEnabled()) {
        return;
    }

    if (SETUP->DRAW_CROSSHAIR) {
        Drawable::drawCrossHair();
    }

    drawHUD();
}

void ComponentHUD::onUpdate() {
    if (!isEnabled()) return;

    drawEnemies();

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
    HUDTextures->addItem(SETUP->HUD_FOLDER + "hud.png", "hud");
    HUDTextures->addItem(SETUP->HUD_FOLDER + "loading.png", "loading");
    HUDTextures->addItem(SETUP->HUD_FOLDER + "health_bar_empty.png", "healthEmptyBar");
    HUDTextures->addItem(SETUP->HUD_FOLDER + "health_bar_stamina.png", "healthBarStaminaPercent");
    HUDTextures->addItem(SETUP->HUD_FOLDER + "health_bar_energy.png", "healthBarEnergyPercent");

    iconsTextures->addItem(SETUP->HUD_FOLDER + "flare.png", "flare");
    iconsTextures->addItem(SETUP->HUD_FOLDER + "plague.png", "plague");
}

void ComponentHUD::writeTextMiddleScreen(const char *text, bool bold) const {
    int totalW = SETUP->screenWidth;
    int totalH = SETUP->screenHeight;

    int xPosition = (totalW / 2) - (int) (strlen(text) * CONCHARS_CHARACTER_W) / 2;
    this->writeText(xPosition, totalH / 2, text, bold);
}

void ComponentHUD::writeCenterHorizontal(int y, const char *text, bool bold) const {
    int totalW = SETUP->screenWidth;

    int xPosition = (totalW / 2) - (int) (strlen(text) * CONCHARS_CHARACTER_W) / 2;
    this->writeText(xPosition, y, text, bold);
}

void ComponentHUD::writeText(int x, int y, const char *text, bool bold) const {
    this->textureWriter->writeText(x, y, text, bold);
}

void ComponentHUD::drawHUD() {

    auto componentManager = ComponentsManager::get();
    auto componentGame = componentManager->getComponentGame();

    drawPlayerStamina(8);
    drawPlayerEnergy(8 + 28 + 1);

    drawEnemySelectedStamina(8);

    if (SETUP->DRAW_FPS) {
        writeCenterHorizontal(
            10,
            std::to_string(componentManager->getComponentRender()->fps).c_str(),
            false
        );
    }

    for (int i = 0; i < buttons.size(); i++) {
        buttons[i]->draw();
        this->textureWriter->writeText(i * 16 + 2, getButtonsOffsetY() + 1, std::to_string(i).c_str(), false);
    }

    if (componentGame->getPlayer()->getWeapon() != nullptr) {
        Weapon *weaponType = componentGame->getPlayer()->getWeapon();
        if (weaponType->isAvailable()) {
        }
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
    addButton(new Button(currentX + offsetX * 0, offsetY, SETUP->HUD_FOLDER + "flare.png", &callbackPlayerShoot));
    addButton(new Button(currentX + offsetX * 1, offsetY, SETUP->HUD_FOLDER + "plague.png", &callbackPlayerShoot2));
    addButton(new Button(currentX + offsetX * 2, offsetY, SETUP->HUD_FOLDER + "cold-fire.png", &callbackPlayerShoot2));
    addButton(new Button(currentX + offsetX * 3, offsetY, SETUP->HUD_FOLDER + "earth-1.png", &callbackPlayerShoot2));
    addButton(new Button(currentX + offsetX * 4, offsetY, SETUP->HUD_FOLDER + "needles.png", &callbackPlayerShoot2));
    addButton(new Button(currentX + offsetX * 5, offsetY, SETUP->HUD_FOLDER + "nova-mega.png", &callbackPlayerShoot2));
    addButton(new Button(currentX + offsetX * 6, offsetY, SETUP->HUD_FOLDER + "thunder-2.png", &callbackPlayerShoot2));
}

void ComponentHUD::drawPlayerStamina(int y) {
    const int offsetX = 2;
    const int offsetY = y;
    const int innerPercentOffsetX = 3;
    const int innerPercentOffsetY = 4;

    auto player = ComponentsManager::get()->getComponentGame()->getPlayer();

    auto backgroundHealthBar= HUDTextures->getTextureByLabel("healthEmptyBar")->getImage();
    backgroundHealthBar->drawFlat(offsetX, offsetY);

    const int fixedWidth = 118;
    const int currentPercentage = (int) ((player->getStamina() * fixedWidth) / (int) player->getStartStamina());

    auto healthBarStaminaPercent = HUDTextures->getTextureByLabel("healthBarStaminaPercent")->getImage();
    for (int i = 0; i < currentPercentage ; i++) {
        healthBarStaminaPercent->drawFlat(offsetX + i + innerPercentOffsetX, offsetY + innerPercentOffsetY);
    }

    if (player->getWeapon() != nullptr) {
        player->getWeapon()->getIcon()->drawFlat(offsetX, + 28 + 2 + offsetY + backgroundHealthBar->height() + 1);
        this->textureWriter->writeText(offsetX, offsetY + 28*2 + 2, std::to_string(player->getWeapon()->getAmmoAmount()).c_str(), false);
    }
}

void ComponentHUD::drawPlayerEnergy(int y)
{
    const int offsetX = 2;
    const int offsetY = y;
    const int innerPercentOffsetX = 3;
    const int innerPercentOffsetY = 4;

    auto player = ComponentsManager::get()->getComponentGame()->getPlayer();
    auto backgroundEnergyBar= HUDTextures->getTextureByLabel("healthEmptyBar")->getImage();
    backgroundEnergyBar->drawFlat(offsetX, offsetY);

    const int fixedWidth = 118;
    const int currentPercentage = (int) ((player->getEnergy() * fixedWidth) / player->getStartEnergy());

    auto healthBarEnergyPercent = HUDTextures->getTextureByLabel("healthBarEnergyPercent")->getImage();

    for (int i = 0; i < currentPercentage ; i++) {
        healthBarEnergyPercent->drawFlat(offsetX + i + innerPercentOffsetX, offsetY + innerPercentOffsetY);
    }
}


void ComponentHUD::drawEnemySelectedStamina(int y)
{

    auto objectSelected = ComponentsManager::get()->getComponentRender()->getSelectedObject();

    auto enemy = dynamic_cast<EnemyGhost*> (objectSelected);
    if (enemy == nullptr) {
        return;
    }

    auto backgroundHealthBar= HUDTextures->getTextureByLabel("healthEmptyBar")->getImage();

    const int offsetX = SETUP->screenWidth - backgroundHealthBar->width() - 2;
    const int offsetY = y;
    const int innerPercentOffsetX = 3;
    const int innerPercentOffsetY = 4;

    backgroundHealthBar->drawFlat(offsetX, offsetY);

    const int fixedWidth = 118;
    const int currentPercentage = (enemy->getStamina() * fixedWidth) / enemy->getStartStamina();

    auto healthBarStaminaPercent = HUDTextures->getTextureByLabel("healthBarStaminaPercent")->getImage();
    for (int i = 0; i < currentPercentage ; i++) {
        healthBarStaminaPercent->drawFlat(offsetX + i + innerPercentOffsetX, offsetY + innerPercentOffsetY);
    }
}

int ComponentHUD::getButtonsOffsetY() {
    const unsigned int innerMargin = 0;
    const int offsetY = SETUP->screenHeight - 16 - innerMargin;
    return offsetY;
}

void ComponentHUD::drawEnemies()
{
    auto objects = Brakeza3D::get()->getSceneObjects();
    auto camera = ComponentsManager::get()->getComponentCamera()->getCamera();

    for (auto object : objects) {
        auto enemy = dynamic_cast<EnemyGhost*> (object);
        if (enemy != nullptr) {
            Vertex3D v = object->getPosition();

            Transforms::cameraSpace(v, v, camera);
            v = Transforms::PerspectiveNDCSpace(v, camera->frustum);

            Point2D screenPoint;
            Transforms::screenSpace(screenPoint, v);
            drawEnemyStats(screenPoint, 15, enemy->getStamina(), enemy->getStartStamina(), Color::red());
        }
    }
}

void ComponentHUD::drawEnemyStats(Point2D screenPoint, float fixedWidth, float value, float startValue, Color c) {
    const int currentPercentage = (int) ((value * fixedWidth) / startValue);

    for (int i = 0; i < currentPercentage; i++) {
        EngineBuffers::getInstance()->setVideoBuffer(screenPoint.x + i - (fixedWidth*0.5) , screenPoint.y, c.getColor());
    }
}
