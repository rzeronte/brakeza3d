
#include <SDL_image.h>
#include "../../headers/Components/ComponentMenu.h"
#include "../../headers/Misc/Tools.h"
#include "../../headers/Render/Logging.h"
#include "../../headers/Brakeza3D.h"

ComponentMenu::ComponentMenu()
{
    this->currentOption = 0;
    this->numOptions = 0;

    const char *file = std::string(SETUP->IMAGES_FOLDER + "menu_background.png").c_str();
    menu_background = IMG_Load(file);
}

void ComponentMenu::onStart()
{
    loadMenuOptions();
}

void ComponentMenu::preUpdate() {

}

void ComponentMenu::onUpdate()
{
    if (SETUP->MENU_ACTIVE) {
        drawOptions( ComponentsManager::get()->getComponentWindow()->screenSurface);
    }
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
    const char *mapsFile;
    mapsFile = Tools::readFile(SETUP->CONFIG_FOLDER + SETUP->CFG_MENU, file_size);
    cJSON *myDataJSON = cJSON_Parse(mapsFile);
    if (myDataJSON == NULL) {
        Logging::getInstance()->Log("menu.json can't be loaded", "ERROR");
        return;
    }

    cJSON *currentOption = NULL;
    optionsJSON = cJSON_GetObjectItemCaseSensitive(myDataJSON, "options" );
    int sizeOptions = cJSON_GetArraySize(optionsJSON);

    Logging::getInstance()->Log("menu.json have " + std::to_string(sizeOptions) + " optionsJSON");

    cJSON_ArrayForEach(currentOption, optionsJSON) {
        cJSON *nameOption   = cJSON_GetObjectItemCaseSensitive(currentOption, "name");
        cJSON *actionOption = cJSON_GetObjectItemCaseSensitive(currentOption, "action");
        cJSON *altOption    = cJSON_GetObjectItemCaseSensitive(currentOption, "alt");

        if (cJSON_IsString(nameOption)) {
            Logging::getInstance()->Log("Adding menu option " + std::string(nameOption->valuestring) + "/" + std::to_string(actionOption->valueint));
            this->options[numOptions] = new MenuOption( nameOption->valuestring, actionOption->valueint );
            this->options[numOptions]->setAlt(altOption->valuestring);
            numOptions++;
        }
    }
}

void ComponentMenu::drawOptions(SDL_Surface *dst)
{
    // Draw back
    SDL_BlitSurface(menu_background, NULL, dst, NULL);

    int offsetY = 50;
    int stepY   = 10;

    int xPos = 100;

    for( int i = 0 ; i < numOptions ; i++) {

        std::string text = this->options[ i ]->getLabel();
        bool bold = false;

        if (i == ComponentMenu::MNU_NEW_GAME && ComponentsManager::get()->getComponentGame()->getPlayer()->state != PlayerState::GAMEOVER ) {
            text = this->options[ ComponentMenu::MNU_NEW_GAME ]->getAlt();
        }

        if (i == currentOption) {
            bold = true;
            text = char(13) + text;
        }

        ComponentsManager::get()->getComponentHUD()->writeText(
            xPos,
            stepY + offsetY,
            text.c_str(),
            bold
        );

        offsetY += stepY;
    }
}