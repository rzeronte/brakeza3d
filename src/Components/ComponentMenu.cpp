
#include <SDL2/SDL_image.h>
#include "../../include/Components/ComponentMenu.h"
#include "../../include/Brakeza3D.h"
#include "../../darkheaz/src/enemies/behaviors/EnemyBehaviorPatrol.h"
#include "../../include/Physics/Mesh3DAnimatedGhost.h"

ComponentMenu::ComponentMenu() {
    this->currentOption = 0;
    this->numOptions = 0;


}

void ComponentMenu::onStart()
{
    Logging::Log("ComponentMenu onStart", "ComponentMenu");
    loadDecorative3DMesh();
    loadMenuOptions();

    light = new LightPoint3D();
    Vertex3D lightPosition = ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition() + Vertex3D(0, 0, 3000);
    light->setPosition(lightPosition);
    light->setEnabled(true);
    light->setLabel("lp2");
    light->setPower(200);
    light->setColorSpecularity(255, 0, 0);
    light->setSpecularComponent(3);
    light->setColor(255, 0, 0);
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

    const float alpha = 255 - ComponentsManager::get()->getComponentGame()->getFadeToGameState()->getProgress() * 255;
    ComponentsManager::get()->getComponentHUD()->getTextWriter()->setAlpha(alpha);

    int offsetY = 140;
    int stepY = 50;

    for (int i = 0; i < numOptions; i++) {
        std::string text = this->options[i]->getLabel();

        if (i == ComponentMenu::MNU_NEW_GAME && (levelInfo->getCurrentLevelIndex() > 0 || levelInfo->isLevelStartedToPlay())) {
            text = this->options[ComponentMenu::MNU_NEW_GAME]->getAlt();
        }

        auto color = ComponentsManager::get()->getComponentGame()->primaryColor;
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
            ComponentsManager::get()->getComponentGame()->secondaryColor,
            0.3
    );
}

