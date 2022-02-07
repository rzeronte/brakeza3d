//
// Created by eduardo on 7/11/21.
//

#include <SDL_image.h>
#include "ComponentConsole.h"

void ComponentConsole::onStart() {
    this->enabled = false;
    this->backgroundCanvas = IMG_Load(std::string(EngineSetup::get()->TEXTURES_FOLDER + "example_texture.png").c_str());
}

void ComponentConsole::preUpdate() {
    //memcpy(&BUFFERS->videoBuffer, &this->backgroundCanvas, sizeof(backgroundCanvas->pixels));
}

void ComponentConsole::onUpdate() {
}

void ComponentConsole::postUpdate() {

    if (!this->enabled) {
        return;
    }


}

void ComponentConsole::onEnd() {

}

void ComponentConsole::onSDLPollEvent(SDL_Event *event, bool &finish) {

}

void ComponentConsole::setRenderer(SDL_Renderer *newRenderer) {
    this->renderer = newRenderer;
}

void ComponentConsole::setTextureRender(SDL_Texture *newTexture) {
    this->screenTexture = newTexture;
}
