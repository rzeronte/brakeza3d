//
// Created by darkhead on 13/1/20.
//

#include "ComponentGUI.h"
#include "../../imgui/imgui.h"
#include "../../imgui/examples/imgui_impl_opengl2.h"
#include "../../imgui/examples/imgui_impl_sdl.h"
#include "../Brakeza3D.h"

ComponentGUI::ComponentGUI()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
}

void ComponentGUI::onStart()
{
    ImGui_ImplSDL2_InitForOpenGL(window, contextOpenGL);
    ImGui_ImplOpenGL2_Init();

    ImGuiIO& io = ImGui::GetIO();
    io.WantCaptureMouse = false;
    io.WantCaptureKeyboard = false;

    // Setup style
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameBorderSize = 1.0f;
}

void ComponentGUI::preUpdate()
{
}

void ComponentGUI::onUpdate()
{
    return;
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplSDL2_NewFrame( window );

    ImGui::NewFrame();
    bool f = false;
    getManagerGui()->draw(
            Brakeza3D::get()->getDeltaTime(),
            f,
            Brakeza3D::get()->getSceneObjects(),
            Brakeza3D::get()->getComponentsManager()->getComponentCamera()->getCamera(),
            Brakeza3D::get()->getComponentsManager()->getComponentRender()->tiles, Brakeza3D::get()->getComponentsManager()->getComponentRender()->tilesWidth,
            10
    );
    ImGui::Render();
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
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

GUIManager *ComponentGUI::getManagerGui() const {
    return managerGUI;
}

void ComponentGUI::setManagerGui(GUIManager *managerGui) {
    managerGUI = managerGui;
}

