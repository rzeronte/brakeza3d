#include "../../include/Components/ComponentGUI.h"
#include "../../include/Brakeza3D.h"

ComponentGUI::ComponentGUI(bool &finish) : finish(finish) {
}

void ComponentGUI::onStart() {
    Logging::Log("ComponentGUI onStart", "ComponentGUI");

}

void ComponentGUI::preUpdate() {
}

void ComponentGUI::onUpdate() {
}

void ComponentGUI::postUpdate() {

}

void ComponentGUI::onEnd() {

}

void ComponentGUI::onSDLPollEvent(SDL_Event *event, bool &finish) {
}

SDL_Window *ComponentGUI::getWindow() const {
    return window;
}

void ComponentGUI::setWindow(SDL_Window *window) {
    ComponentGUI::window = window;
}

SDL_Renderer *ComponentGUI::getRenderer() const {
    return renderer;
}

void ComponentGUI::setRenderer(SDL_Renderer *renderer) {
    ComponentGUI::renderer = renderer;
}

SDL_GLContext *ComponentGUI::getContextOpenGl() const {
    return contextOpenGL;
}

void ComponentGUI::setContextOpenGl(SDL_GLContext *contextOpenGl) {
    contextOpenGL = contextOpenGl;
}

void ComponentGUI::setFinish(bool &finish) {
    ComponentGUI::finish = finish;
}

