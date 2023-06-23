
#include <SDL2/SDL_image.h>
#include "../../include/Components/ComponentMenu.h"
#include "../../include/Brakeza3D.h"
#include "../../darkheaz/src/enemies/behaviors/EnemyBehaviorPatrol.h"

ComponentMenu::ComponentMenu()
:
        shaderBackgroundImage(nullptr),
        planet(nullptr),
        light(nullptr),
        pendulum(nullptr),
        currentOption(0)
{
}

ComponentMenu::~ComponentMenu()
{
    delete shaderBackgroundImage;
    delete planet;
    delete light;
    delete pendulum;
}

void ComponentMenu::onStart()
{
    Logging::Log("ComponentMenu onStart");

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

    shaderBackgroundImage = new ShaderImage(SETUP->IMAGES_FOLDER + "menuBackground.png");
    shaderBackgroundImage->setEnabled(true);
    shaderBackgroundImage->setUseOffset(false);

    shaderMenuTitle = new ShaderImageMask(true, SETUP->IMAGES_FOLDER + "menuTitle.png", SETUP->IMAGES_FOLDER + "menuTitleMask.png");
    border = new Image(SETUP->IMAGES_FOLDER + "hud_background.png");

}

void ComponentMenu::loadDecorative3DMesh()
{
    planet = new Mesh3D();
    planet->setEnabled(false);
    planet->setAlpha(255);
    planet->setEnableLights(false);
    planet->setPosition(Vertex3D(-1540, 17000, 30000));
    planet->setRotationFrameEnabled(true);
    planet->setRotationFrame(Vertex3D(0.1f, 0.0, 0));
    planet->setRotation(0, 0, 0);
    planet->setScale(61.9);
    planet->setStencilBufferEnabled(true);
    planet->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "red_planet.fbx"));
    planet->updateBoundingBox();
    Brakeza3D::get()->addObject3D(planet, "planetMenu");

    pendulum = new SimplePendulum(0, 1, 1000, 0.0);
    pendulum->setRotation(295, 30, -20);
}

void ComponentMenu::preUpdate()
{
    if (!isEnabled()) {
        return;
    }

    shaderBackgroundImage->update();
}

void ComponentMenu::onUpdate()
{
    if (!isEnabled()) {
        return;
    }

    pendulum->onUpdate();
    //spaceship->setRotation(pendulum->pendulumRotation);

    drawOptions();
    drawVersion();
    const float alpha = 255 - ComponentsManager::get()->getComponentGame()->getFadeToGameState()->getProgress() * 255;

    ComponentsManager::get()->getComponentGame()->shaderTutorialMask->setMaxAlpha(alpha);
    ComponentsManager::get()->getComponentGame()->shaderTutorialMask->update();
    ComponentsManager::get()->getComponentGame()->boxTutorial->drawFlatAlpha(0, 0, alpha * 0.90f);
    shaderMenuTitle->update();
    border->drawFlatAlpha(0, 0, alpha);

}

void ComponentMenu::postUpdate()
{
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
        Logging::Log("menu.json can't be loaded");
        return;
    }

    cJSON *currentLoadingOption;
    cJSON_ArrayForEach(currentLoadingOption, cJSON_GetObjectItemCaseSensitive(myDataJSON, "options")) {
        cJSON *nameOption = cJSON_GetObjectItemCaseSensitive(currentLoadingOption, "name");
        cJSON *actionOption = cJSON_GetObjectItemCaseSensitive(currentLoadingOption, "action");
        cJSON *altOption = cJSON_GetObjectItemCaseSensitive(currentLoadingOption, "alt");

        if (cJSON_IsString(nameOption)) {
            Logging::Log("Adding menu option %s", std::string(std::string(nameOption->valuestring) + "/" + std::to_string(actionOption->valueint)).c_str());
            options.emplace_back(nameOption->valuestring, altOption->valuestring, actionOption->valueint);
        }
    }

    free(contentFile);
    cJSON_Delete(myDataJSON);
    cJSON_Delete(currentLoadingOption);
}

void ComponentMenu::drawOptions()
{
    auto componentGame = ComponentsManager::get()->getComponentGame();
    auto levelInfo = componentGame->getLevelLoader();

    int offsetY = 140;
    int stepY = 40;

    for (int i = 0; i < (int) options.size() ; i++) {
        std::string text = this->options[i].getLabel();

        if (i == ComponentMenu::MNU_NEW_GAME && (levelInfo->getCurrentLevelIndex() > 0 || levelInfo->isLevelStartedToPlay())) {
            text = this->options[ComponentMenu::MNU_NEW_GAME].getAlt();
        }

        auto color = ComponentsManager::get()->getComponentGame()->getPrimaryColor();
        float currentAlpha = componentGame->getTextWriter()->getAlpha();

        if (i == currentOption) {
            float currentSelectionAlpha = 0.5f * (float) (1+sin(5 * PI * Brakeza3D::get()->getExecutionTime()));
            componentGame->getTextWriter()->setAlpha(currentAlpha * currentSelectionAlpha);
        }

        componentGame->getTextWriter()->writeTTFCenterHorizontal(
            stepY + offsetY,
            text.c_str(),
            color,
            0.5
        );

        if (i == currentOption) {
            componentGame->getTextWriter()->setAlpha(currentAlpha);
        }

        offsetY += stepY;
    }
}

void ComponentMenu::setEnabled(bool value)
{
    Component::setEnabled(value);

    light->setEnabled(value);
    planet->setEnabled(value);
}

void ComponentMenu::drawVersion()
{
    ComponentsManager::get()->getComponentGame()->getTextWriter()->writeTTFCenterHorizontal(
        455,
        "brakeza.com",
        ComponentsManager::get()->getComponentGame()->getPrimaryColor(),
        0.3
    );
}

int ComponentMenu::getCurrentOption() const {
    return currentOption;
}

int ComponentMenu::getNumOptions() const {
    return (int) options.size();
}

void ComponentMenu::increaseMenuOption()
{
    currentOption++;
}

void ComponentMenu::decreaseMenuOption()
{
    currentOption--;
}

std::vector<MenuOption> &ComponentMenu::getOptions()
{
    return options;
}

