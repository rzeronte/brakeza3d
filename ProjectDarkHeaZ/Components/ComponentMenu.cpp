
#include <SDL2/SDL_image.h>
#include "ComponentMenu.h"
#include "../../include/Brakeza3D.h"

ComponentMenu::ComponentMenu()
:
    imageLogoBox(Image(SETUP->IMAGES_FOLDER + "logo_box.png")),
    boxTutorial(Image(SETUP->IMAGES_FOLDER + "tutorial_box.png")),
    background(Image(SETUP->IMAGES_FOLDER + "backgroundMenu.png")),
    border(Image(SETUP->IMAGES_FOLDER + "hud_background.png")),
    glassEffect(Image(SETUP->IMAGES_FOLDER + "menuBackground.png")),
    sceneMenu("menu.json"),
    currentOption(0),
    menuEnabled(false)
{
    loadMenuOptions();
}

void ComponentMenu::LoadScene()
{
    ComponentsManager::get()->getComponentRender()->getSceneLoader().loadScene(sceneMenu);
    ComponentsManager::get()->getComponentRender()->playLUAScripts();
}

void ComponentMenu::onStart()
{
    Logging::Message("ComponentMenu onStart");
}

void ComponentMenu::preUpdate()
{
    if (!isEnabled()) {
        return;
    }

    const float alpha = 1 - ComponentsManager::get()->getComponentGame()->getFadeToGameState()->getProgress();

    auto window = ComponentsManager::get()->getComponentWindow();
    auto fb = window->getForegroundFramebuffer();
    auto bb = window->getBackgroundFramebuffer();

    background.drawFlatAlpha(0, 0, alpha, bb);
    //boxTutorial.drawFlatAlpha(0, 0, alpha, fb);
    border.drawFlatAlpha(0, 0, alpha, fb);
    //glassEffect.drawFlatAlpha(0, 0, alpha, fb);
    imageLogoBox.drawFlatAlpha(0, 0, alpha, fb);
}

void ComponentMenu::onUpdate()
{
    if (!isEnabled()|| !isMenuEnabled()) return;

    drawOptions();
    drawVersion();
}

void ComponentMenu::postUpdate(){
}

void ComponentMenu::onEnd(){
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

    int offsetY = 150;
    int stepY = 70;

    auto color = PaletteColors::getMenuOptions();

    for (int i = 0; i < (int) options.size() ; i++) {
        std::string text = this->options[i].getLabel();

        if (i == ComponentMenu::MNU_NEW_GAME && (levelInfo->getCurrentLevelIndex() > 0 || levelInfo->isLevelStartedToPlay())) {
            text = this->options[ComponentMenu::MNU_NEW_GAME].getAlt();
        }

        float currentAlpha = componentGame->getTextWriter()->getAlpha();

        if (i == currentOption) {
            float currentSelectionAlpha = 0.5f * (float) (1+sin(2.5f * M_PI * Brakeza3D::get()->getExecutionTime()));
            componentGame->getTextWriter()->setAlpha(currentAlpha * currentSelectionAlpha);
        }

        componentGame->getTextWriter()->writeTTFCenterHorizontal(
            stepY + offsetY,
            text.c_str(),
            color,
            1.5
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
    setMenuEnabled(value);
}

void ComponentMenu::drawVersion()
{
    auto writer = ComponentsManager::get()->getComponentGame()->getTextWriter();

    writer->setFont(ComponentsManager::get()->getComponentWindow()->getFontDefault());

    writer->writeTTFCenterHorizontal(
        756,
        "www.brakeza.com",
        Color::white(),
        0.35
    );
    writer->writeTextTTFAutoSize(1160, 690, "v.1.21.7", PaletteColors::getMenuOptions(), 0.5);

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
