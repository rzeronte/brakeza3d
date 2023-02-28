
#include <SDL2/SDL_image.h>
#include "../../include/Components/ComponentMenu.h"
#include "../../include/Brakeza3D.h"
#include "../../darkheaz/src/enemies/behaviors/EnemyBehaviorPatrol.h"
#include "../../include/Physics/Mesh3DAnimatedGhost.h"

ComponentMenu::ComponentMenu(): numOptions(0), currentOption(0)
{
}

ComponentMenu::~ComponentMenu()
{
    delete shaderBackgroundImage;
    delete title;
    delete spaceship;
    delete light;
    delete pendulum;

    for (int i = 0; i < numOptions; i++) {
        delete options[i];
    }
}

void ComponentMenu::onStart()
{
    Logging::Log("ComponentMenu onStart", "ComponentMenu");
    loadDecorative3DMesh();
    loadMenuOptions();

    Vertex3D lightPosition = ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition() + Vertex3D(0, 0, 3000);

    light = new LightPoint3D(200, 1, 0, 0, 10, Color(255, 0, 0), Color(255, 0, 0));
    light->setPosition(lightPosition);
    light->setEnabled(true);
    light->setLabel("lightMenu");
    light->setRotation(270, 0, 0);
    light->setBehavior(new EnemyBehaviorPatrol(lightPosition + Vertex3D(-5000, 0, 0), lightPosition + Vertex3D(5000, 0, 0), 1));
    Brakeza3D::get()->addObject3D(light, "lightMenu");

    shaderBackgroundImage = new ShaderImage();
    shaderBackgroundImage->setPhaseRender(EngineSetup::ShadersPhaseRender::PREUPDATE);
    shaderBackgroundImage->setEnabled(true);
    shaderBackgroundImage->setUseOffset(false);
    shaderBackgroundImage->setImage(new Image(SETUP->IMAGES_FOLDER + "menu_background.png"));
}

void ComponentMenu::loadDecorative3DMesh() {

    title = new Image(SETUP->IMAGES_FOLDER + "title.png");

    spaceship = new Mesh3D();
    spaceship->setLabel("spaceshipMenu");
    spaceship->setEnabled(true);
    spaceship->setAlpha(2555);
    spaceship->setEnableLights(true);
    spaceship->setPosition(Vertex3D(5000, -1000, 20000));
    spaceship->setRotationFrameEnabled(false);
    spaceship->setRotationFrame(Vertex3D(0, 0.5, 0));
    spaceship->setRotation(-30, 0, 0);
    spaceship->setScale(6);
    spaceship->setStencilBufferEnabled(true);
    spaceship->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "spaceships/player.fbx"));
    spaceship->updateBoundingBox();
    Brakeza3D::get()->addObject3D(spaceship, "spacheshipMenu");

    pendulum = new SimplePendulum(0, 1, 1000, 0.0);
    pendulum->setRotation(295, 30, -20);
}


void ComponentMenu::preUpdate()
{
    if (!isEnabled()) {
        return;
    }

    shaderBackgroundImage->update();

    const float alpha = 255 - ComponentsManager::get()->getComponentGame()->getFadeToGameState()->getProgress() * 255;
    title->drawFlatAlpha(0, 0, alpha);

    drawOptions();
    drawVersion();
}

void ComponentMenu::onUpdate()
{
    if (!isEnabled()) {
        return;
    }

    pendulum->onUpdate();
    spaceship->setRotation(pendulum->pendulumRotation);

}

void ComponentMenu::postUpdate() {

}

void ComponentMenu::onEnd() {

}

void ComponentMenu::onSDLPollEvent(SDL_Event *event, bool &finish) {

}

void ComponentMenu::loadMenuOptions()
{
    size_t file_size;
    auto contentFile = Tools::readFile(SETUP->CONFIG_FOLDER + SETUP->CFG_MENU, file_size);
    cJSON *myDataJSON = cJSON_Parse(contentFile);
    if (myDataJSON == nullptr) {
        Logging::Log("menu.json can't be loaded", "ERROR");
        return;
    }

    cJSON *currentLoadingOption;
    cJSON_ArrayForEach(currentLoadingOption, cJSON_GetObjectItemCaseSensitive(myDataJSON, "options")) {
        cJSON *nameOption = cJSON_GetObjectItemCaseSensitive(currentLoadingOption, "name");
        cJSON *actionOption = cJSON_GetObjectItemCaseSensitive(currentLoadingOption, "action");
        cJSON *altOption = cJSON_GetObjectItemCaseSensitive(currentLoadingOption, "alt");

        if (cJSON_IsString(nameOption)) {
            Logging::Log("Adding menu option " + std::string(nameOption->valuestring) + "/" + std::to_string(actionOption->valueint), "Menu");
            this->options[numOptions] = new MenuOption(nameOption->valuestring, actionOption->valueint);
            this->options[numOptions]->setAlt(altOption->valuestring);
            numOptions++;
        }
    }

    free(contentFile);

    cJSON_Delete(myDataJSON);
    cJSON_Delete(currentLoadingOption);
}

void ComponentMenu::drawOptions() {

    auto levelInfo = ComponentsManager::get()->getComponentGame()->getLevelInfo();

    const float alpha = 255 - ComponentsManager::get()->getComponentGame()->getFadeToGameState()->getProgress() * 255;
    ComponentsManager::get()->getComponentHUD()->getTextWriter()->setAlpha(alpha);

    int offsetY = 140;
    int stepY = 50;

    for (int i = 0; i < numOptions; i++) {
        std::string text = this->options[i]->getLabel();

        if (i == ComponentMenu::MNU_NEW_GAME && (levelInfo->getCurrentLevelIndex() > 0 || levelInfo->isLevelStartedToPlay())) {
            text = this->options[ComponentMenu::MNU_NEW_GAME]->getAlt();
        }

        auto color = ComponentsManager::get()->getComponentGame()->getPrimaryColor();
        if (i == currentOption) {
            color = Color::black();
        }

        ComponentsManager::get()->getComponentHUD()->getTextWriter()->writeTextTTFAutoSize(
            30,
            stepY + offsetY,
            text.c_str(),
            color,
            0.5
        );

        offsetY += stepY;
    }
}

void ComponentMenu::setEnabled(bool value)
{
    Component::setEnabled(value);

    light->setEnabled(value);
    spaceship->setEnabled(value);
}

void ComponentMenu::drawVersion()
{
    ComponentsManager::get()->getComponentHUD()->getTextWriter()->writeTextTTFAutoSize(
        520,
        455,
        "brakeza.com",
        ComponentsManager::get()->getComponentGame()->getSecondaryColor(),
        0.3
    );
}

int ComponentMenu::getCurrentOption() const {
    return currentOption;
}

int ComponentMenu::getNumOptions() const {
    return numOptions;
}

void ComponentMenu::increaseMenuOption()
{
    currentOption++;
}

void ComponentMenu::decreaseMenuOption()
{
    currentOption--;
}

MenuOption *const *ComponentMenu::getOptions() const {
    return options;
}

