
#include <SDL2/SDL_image.h>
#include "../../include/Components/ComponentMenu.h"
#include "../../include/Brakeza3D.h"
#include "../../darkheaz/src/enemies/behaviors/EnemyBehaviorPatrol.h"

ComponentMenu::ComponentMenu() {
    this->currentOption = 0;
    this->numOptions = 0;

    imageBackground = new Image(SETUP->IMAGES_FOLDER + "menu_background.png");
}

void ComponentMenu::onStart() {
    Logging::Log("ComponentMenu onStart", "ComponentMenu");
    loadDecorative3DMesh();
    loadMenuOptions();

    auto light = new LightPoint3D();
    Vertex3D lightPosition = ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition() + Vertex3D(0, 0, 6000);
    light->setPosition(lightPosition);
    light->setEnabled(true);
    light->setLabel("lp2");
    light->setPower(200);
    light->setColorSpecularity(220, 220, 30);
    light->setSpecularComponent(3);
    light->setColor(0, 255, 0);
    light->setRotation(270, 0, 0);
    light->setBehavior(new EnemyBehaviorPatrol(lightPosition + Vertex3D(-5000, 0, 0), lightPosition + Vertex3D(5000, 0, 0), 1));
    Brakeza3D::get()->addObject3D(light, "lightMenu");

    auto light2 = new LightPoint3D();
    light2->setPosition(lightPosition);
    light2->setEnabled(true);
    light2->setLabel("lp2");
    light2->setPower(200);
    light2->setColor(255, 0, 0);
    light2->setColorSpecularity(220, 220, 30);
    light2->setSpecularComponent(3);
    light2->setRotation(270, 0, 0);
    light2->setBehavior(new EnemyBehaviorPatrol(lightPosition + Vertex3D(5000, 0, 0), lightPosition + Vertex3D(-5000, 0, 0), 1));
    Brakeza3D::get()->addObject3D(light2, "light2Menu");
}

void ComponentMenu::loadDecorative3DMesh() {
    Vertex3D spaceshipBasePosition(0, 3100, 20000);

    Vertex3D speedRotation(1, 1, 0.5);

    title = new Mesh3D();
    title->setEnabled(true);
    title->setLabel("menuTitle");
    title->setEnableLights(true);
    //title->setRotation(90, 0, 0);
    title->setPosition(Vertex3D(-5700, -2600, 6200));
    title->setStencilBufferEnabled(false);
    title->setFlatTextureColor(true);
    title->setFlatColor(Color::blue());
    title->setRotationFrameEnabled(false);
    title->setRotationFrame(Vertex3D(0, 0, 0.1));
    title->setScale(1);
    title->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "meteorita.fbx"));
    Brakeza3D::get()->addObject3D(title, title->getLabel());
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

    drawOptions();
}

void ComponentMenu::postUpdate() {

}

void ComponentMenu::onEnd() {

}

void ComponentMenu::onSDLPollEvent(SDL_Event *event, bool &finish) {

}

void ComponentMenu::loadMenuOptions() {
    size_t file_size;
    auto contentFile = Tools::readFile(SETUP->CONFIG_FOLDER + SETUP->CFG_MENU, file_size);
    cJSON *myDataJSON = cJSON_Parse(contentFile);
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
    free(contentFile);
    cJSON_Delete(myDataJSON);
}

void ComponentMenu::drawOptions() {

    auto levelInfo = ComponentsManager::get()->getComponentGame()->getLevelInfo();

    int offsetY = 170;
    int stepY = 15;

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

void ComponentMenu::setEnabled(bool value)
{
    Component::setEnabled(value);
    title->setEnabled(value);
}

