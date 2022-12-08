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

    textureWriter = new TextWriter(
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
    //drawEnemies();
}

void ComponentHUD::postUpdate()
{
    if (!isEnabled()) return;

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
    HUDTextures->addItem(SETUP->ICONS_FOLDER + "gravitational_shield.png", "gravitationalShield");

}

void ComponentHUD::writeTextMiddleScreen(const char *text, bool bold) const
{
    int totalW = SETUP->screenWidth;
    int totalH = SETUP->screenHeight;

    int xPosition = (totalW / 2) - (int) (strlen(text) * CONCHARS_CHARACTER_W) / 2;
    this->writeText(xPosition, totalH / 2, text, bold);
}

void ComponentHUD::writeCenterHorizontal(int y, const char *text, bool bold) const
{
    int totalW = SETUP->screenWidth;

    int xPosition = (totalW / 2) - (int) (strlen(text) * CONCHARS_CHARACTER_W) / 2;
    this->writeText(xPosition, y, text, bold);
}

void ComponentHUD::writeText(int x, int y, const char *text, bool bold) const
{
    this->textureWriter->writeText(x, y, text, bold);
}

void ComponentHUD::drawHUD()
{
    auto componentManager = ComponentsManager::get();

    drawShaderBars();
    drawEnemySelectedShaderStamina();

    drawPlayerStamina(15);

    if (SETUP->DRAW_FPS) {
        writeCenterHorizontal(
            10,
            std::to_string(componentManager->getComponentRender()->fps).c_str(),
            false
        );
    }

    for (auto & button : buttons) {
        button->draw();
    }

    this->writeCenterHorizontal(25, componentManager->getComponentGame()->getLevelInfo()->getLevelName().c_str(), false);
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
    const int offsetX = 10;
    int offsetY = y;

    auto game = ComponentsManager::get()->getComponentGame();
    auto player = game->getPlayer();

    int availablesWeaponsCounter = 0;

    player->getWeapon()->getIcon()->drawFlat(15, 430);


    if (player->isAllowGravitationalShields()) {
        auto gravitationalShieldImage = HUDTextures->getTextureByLabel("gravitationalShield")->getImage();
        int gravitationalShieldsNumber = MAX_GRAVITATIONAL_SHIELDS - (int) player->getGravityShieldsNumber();
        for (int i = 0; i < gravitationalShieldsNumber; i++) {
            gravitationalShieldImage->drawFlat(60 + gravitationalShieldImage->width() * i, 400);
        }
    }
}

void ComponentHUD::drawSelectedWeaponEffect(int x, int y, int width, int height, Color c) {
    auto *buffer = EngineBuffers::getInstance();

    for (int i = 0; i < width; i++) {
        buffer->setVideoBuffer(x + i, y, c.getColor());
        buffer->setVideoBuffer(x + i, y + height - 1, c.getColor());
    }
    for (int i = 0; i < height; i++) {
        buffer->setVideoBuffer(x, y + i, c.getColor());
        buffer->setVideoBuffer(x + width - 1, y + i, c.getColor());
    }
}

void ComponentHUD::drawNotAvailableWeaponEffect(int xOrigin, int yOrigin, int width, int height, Color c) {
    auto *buffer = EngineBuffers::getInstance();

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            const int finalX = xOrigin + x;
            const int finalY = yOrigin + y;
            buffer->setVideoBuffer(finalX, finalY, c.getColor());
        }
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

            if (enemy->isAlphaEnabled()) return;

            enemy->updateBoundingBox();

            Point2D screenMinPoint;
            Point2D screenMaxPoint;
            this->getScreenCoordinatesForBoundingBox(screenMinPoint, screenMaxPoint, enemy, camera);

            const float sizeWidth = (float) (screenMaxPoint.x - screenMinPoint.x) * 0.5f;
            Point2D middlePoint(
                screenMinPoint.x + (int) sizeWidth,
                screenMinPoint.y
            );

            drawEnemyStats(
                middlePoint,
                sizeWidth,
                enemy->getStamina(),
                enemy->getStartStamina(),
                Color::red()
            );
        }
    }
}

void ComponentHUD::drawEnemyStats(Point2D screenPoint, float fixedWidth, float value, float startValue, Color c) {
    const int currentPercentage = (int) ((value * fixedWidth) / startValue);

    for (int i = 0; i < currentPercentage; i++) {
        int x = screenPoint.x + i - (fixedWidth*0.5);
        int y = screenPoint.y;
        if (!Tools::isPixelInWindow(x, y)) continue;
        EngineBuffers::getInstance()->setVideoBuffer(x, y, c.getColor());
        EngineBuffers::getInstance()->setVideoBuffer(x, y+1, c.getColor());
    }
}

void ComponentHUD::getScreenCoordinatesForBoundingBox(Point2D &min, Point2D &max, Mesh3D *mesh, Camera3D *camera)
{
    min.x = EngineSetup::get()->screenWidth;
    min.y = EngineSetup::get()->screenHeight;
    max.x = -1;
    max.y = -1;

    for (auto vertex : mesh->aabb.vertices) {
        Point2D screenPoint = Transforms::WorldToPoint(vertex, camera);
        min.x = std::min(min.x, screenPoint.x);
        min.y = std::min(min.y, screenPoint.y);
        max.x = std::max(max.x, screenPoint.x);
        max.y = std::max(max.y, screenPoint.y);
    }

    min.x = std::clamp(min.x, 0, EngineSetup::get()->screenWidth);
    min.y = std::clamp(min.y, 0, EngineSetup::get()->screenHeight);
    max.x = std::clamp(max.x, 0, EngineSetup::get()->screenWidth);
    max.y = std::clamp(max.y, 0, EngineSetup::get()->screenHeight);
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
