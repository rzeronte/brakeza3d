
#include <SDL2/SDL_image.h>
#include "../../include/Components/ComponentMenu.h"
#include "../../include/Brakeza3D.h"

ComponentMenu::ComponentMenu() {
    this->currentOption = 0;
    this->numOptions = 0;

    imageBackground = new Image(SETUP->IMAGES_FOLDER + "menu_background.png");
}

void ComponentMenu::onStart() {
    Logging::Log("ComponentMenu onStart", "ComponentMenu");
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

    auto player = ComponentsManager::get()->getComponentGame()->getPlayer();
    auto levelInfo = ComponentsManager::get()->getComponentGame()->getLevelInfo();

    int offsetY = 50;
    int stepY = 10;

    for (int i = 0; i < numOptions; i++) {
        std::string text = this->options[i]->getLabel();
        bool bold = false;

        if (i == ComponentMenu::MNU_NEW_GAME && levelInfo->isLevelStartedToPlay()) {
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