
#include <SDL2/SDL_image.h>
#include "ComponentMenu.h"
#include "../../include/Brakeza3D.h"

ComponentMenu::ComponentMenu()
:
    shaderBackgroundImage(nullptr),
    planet(nullptr),
    pendulum(nullptr),
    currentOption(0),
    menuEnabled(false)
{
}

ComponentMenu::~ComponentMenu()
{
    delete shaderBackgroundImage;
    delete planet;
    delete hexagonStation;
    delete pendulum;
}

void ComponentMenu::onStart()
{
    Logging::Log("ComponentMenu onStart");

    loadDecorative3DMesh();
    loadMenuOptions();

    Vertex3D lightPosition = ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition() + Vertex3D(0, 0, 3000);

    shaderBackgroundImage = new ShaderImage(SETUP->IMAGES_FOLDER + "menuBackground.png");
    shaderBackgroundImage->setEnabled(true);
    shaderBackgroundImage->setUseOffset(false);

    shaderMenuTitle = new ShaderImageMask(true, SETUP->IMAGES_FOLDER + "menuTitle.png", SETUP->IMAGES_FOLDER + "menuTitleMask.png");
    border = new Image(SETUP->IMAGES_FOLDER + "hud_background.png");
    imageLogoBox =  new Image(SETUP->IMAGES_FOLDER + "logo_box.png");
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

    hexagonStation = new Mesh3D();
    hexagonStation->setEnabled(false);
    hexagonStation->setAlpha(255);
    hexagonStation->setEnableLights(false);
    hexagonStation->setPosition(Vertex3D(-3800, 1000, 10000));
    hexagonStation->setRotationFrameEnabled(true);
    hexagonStation->setRotationFrame(Vertex3D(0.02f, 0.1f, 0.01));
    hexagonStation->setRotation(20, 40, 0);
    hexagonStation->setScale(0.2);
    hexagonStation->setStencilBufferEnabled(true);
    hexagonStation->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "space_hexagon.fbx"));
    hexagonStation->updateBoundingBox();
    Brakeza3D::get()->addObject3D(hexagonStation, "astronautMenu");

    pendulum = new SimplePendulum(0, 5, 5000, 0.0);
    pendulum->setRotation(-40, 0, 0);
}

void ComponentMenu::preUpdate()
{
    if (!isEnabled()) {
        return;
    }

    shaderBackgroundImage->update(0, 0);
}

void ComponentMenu::onUpdate()
{
    if (!isEnabled()|| !isMenuEnabled()) return;

    pendulum->onUpdate();
    //astronaut->setRotation(pendulum->pendulumRotation);

    const float alpha = 255 - ComponentsManager::get()->getComponentGame()->getFadeToGameState()->getProgress() * 255;

    ComponentsManager::get()->getComponentGame()->dialogBackground->setMaxAlpha((int) alpha);
    ComponentsManager::get()->getComponentGame()->dialogBackground->update();
    ComponentsManager::get()->getComponentGame()->boxTutorial->drawFlatAlpha(0, 0, alpha);

    border->drawFlatAlpha(0, 0, alpha);
    imageLogoBox->drawFlatAlpha(0, 0, alpha);
    shaderMenuTitle->update();
    drawOptions();
    drawVersion();
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

    auto color = componentGame->getPalette().getCrt();

    for (int i = 0; i < (int) options.size() ; i++) {
        std::string text = this->options[i].getLabel();

        if (i == ComponentMenu::MNU_NEW_GAME && (levelInfo->getCurrentLevelIndex() > 0 || levelInfo->isLevelStartedToPlay())) {
            text = this->options[ComponentMenu::MNU_NEW_GAME].getAlt();
        }

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

    planet->setEnabled(value);
    hexagonStation->setEnabled(value);

    setMenuEnabled(value);
}

void ComponentMenu::drawVersion()
{
    ComponentsManager::get()->getComponentGame()->getTextWriter()->writeTTFCenterHorizontal(
        447,
        "https://brakeza.com",
        ComponentsManager::get()->getComponentGame()->getPalette().getFive(),
        0.3
    );

    ComponentsManager::get()->getComponentGame()->getTextWriter()->writeTTFCenterHorizontal(362, "v.1.21.7", ComponentsManager::get()->getComponentGame()->getPalette().getFive(), 0.2);
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

void ComponentMenu::setMenuEnabled(bool menuEnabled) {
    ComponentMenu::menuEnabled = menuEnabled;
}

bool ComponentMenu::isMenuEnabled() const {
    return menuEnabled;
}

Image *ComponentMenu::getBorder() const {
    return border;
}
