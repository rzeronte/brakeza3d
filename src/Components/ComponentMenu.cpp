
#include <SDL2/SDL_image.h>
#include "../../include/Components/ComponentMenu.h"
#include "../../include/Brakeza3D.h"
#include "../../include/Game/EnemyBehaviorPatrol.h"

ComponentMenu::ComponentMenu() {
    this->currentOption = 0;
    this->numOptions = 0;

    imageBackground = new Image(SETUP->IMAGES_FOLDER + "menu_background.png");
}

void ComponentMenu::onStart() {
    Logging::Log("ComponentMenu onStart", "ComponentMenu");

    spaceship = new Mesh3D();
    spaceship->setEnabled(true);
    spaceship->setLabel("spaceshipMenu");
    spaceship->setEnableLights(true);
    spaceship->setRotation(90, 0, 0);
    spaceship->setPosition(Vertex3D(1500, -1150, 2250));
    spaceship->setStencilBufferEnabled(true);
    spaceship->setRotationFrameEnabled(true);
    spaceship->setRotationFrame(Vertex3D(0, 0, 0.1));
    spaceship->setScale(1);
    spaceship->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "red_spaceship_03.fbx"));

    Brakeza3D::get()->addObject3D(spaceship, spaceship->getLabel());
    loadMenuOptions();
}

void ComponentMenu::preUpdate() {
    if (!isEnabled()) {
        return;
    }

    imageBackground->drawFlat(0, 0);
}

void ComponentMenu::onUpdate() {
    if (!isEnabled()) {
        return;
    }

    drawOptions(ComponentsManager::get()->getComponentWindow()->screenSurface);
}

void ComponentMenu::postUpdate() {

}

void ComponentMenu::onEnd() {

}

void ComponentMenu::onSDLPollEvent(SDL_Event *event, bool &finish) {

}

void ComponentMenu::loadMenuOptions() {
    size_t file_size;
    const char *mapsFile;
    mapsFile = Tools::readFile(SETUP->CONFIG_FOLDER + SETUP->CFG_MENU, file_size);
    cJSON *myDataJSON = cJSON_Parse(mapsFile);
    if (myDataJSON == nullptr) {
        Logging::Log("menu.json can't be loaded", "ERROR");
        return;
    }

    cJSON *currentLoadingOption;
    optionsJSON = cJSON_GetObjectItemCaseSensitive(myDataJSON, "options");
    int sizeOptions = cJSON_GetArraySize(optionsJSON);

    Logging::Log("menu.json have " + std::to_string(sizeOptions) + " optionsJSON", "Menu");

    cJSON_ArrayForEach(currentLoadingOption, optionsJSON) {
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
}

void ComponentMenu::drawOptions(SDL_Surface *dst) {

    auto levelInfo = ComponentsManager::get()->getComponentGame()->getLevelInfo();

    int offsetY = 50;
    int stepY = 10;

    for (int i = 0; i < numOptions; i++) {
        std::string text = this->options[i]->getLabel();
        bool bold = false;

        if (i == ComponentMenu::MNU_NEW_GAME && (levelInfo->getCurrentLevelIndex() > 0 || levelInfo->isLevelStartedToPlay())) {
            text = this->options[ComponentMenu::MNU_NEW_GAME]->getAlt();
        }

        if (i == currentOption) {
            bold = true;
            text = char(13) + text;
        }
        ComponentsManager::get()->getComponentHUD()->writeCenterHorizontal(
                stepY + offsetY,
                text.c_str(),
                bold
        );

        offsetY += stepY;
    }
}

void ComponentMenu::setEnabled(bool value) {
    Component::setEnabled(value);
    spaceship->setEnabled(value);
}

