
#include "../include/GUI/GUIManager.h"
#include "../../include/GUI/Objects/Object3DGUI.h"
#include "../../include/GUI/Objects/ShadersGUI.h"
#include "../../include/GUI/Objects/ScriptLuaGUI.h"
#include "../../include/GUI/Objects/Mesh3DGUI.h"
#include "../../include/GUI/Objects/FileSystemGUI.h"
#include "../../include/GUI/AddOns/GUIConsole.h"
#include "../../include/Brakeza.h"

GUIManager::GUIManager(std::vector<Object3D *> &gameObjects)
:
    gameObjects(gameObjects),
    widgetConsole(new GuiAddonConsole(ComponentsManager::get()->getComponentScripting()->getLua())),
    widgetObjects3D(new GUIAddonObjects3D(gameObjects)),
    widgetObject3DProperties(new GUIAddonObject3DProperties(gameObjects, scriptEditableManager)),
    widgetProjectSettings(new GUIAddonProjectSetup(scriptEditableManager)),
    widgetMenu(new GUIAddonMenu(windows)),
    widgetToolbar(new GUIAddonToolbar()),
    currentScriptsFolderWidget(BrakezaSetup::get()->SCRIPTS_FOLDER),
    currentScenesFolderWidget(BrakezaSetup::get()->SCENES_FOLDER),
    currentProjectsFolderWidget(BrakezaSetup::get()->PROJECTS_FOLDER),
    currentShadersFolderWidget(BrakezaSetup::get()->CUSTOM_SHADERS_FOLDER)
{
    FileSystemGUI::LoadIcons(icons);
    FileSystemGUI::LoadImagesFolder(this);
    Profiler::get()->CaptureGUIMemoryUsage();

    currentScriptsFolders = Tools::getFolderFolders(currentScriptsFolderWidget);
    currentScriptsFolderFiles = Tools::getFolderFiles(currentScriptsFolderWidget, "lua");

    currentScenesFolders = Tools::getFolderFolders(currentScenesFolderWidget);
    currentScenesFolderFiles = Tools::getFolderFiles(currentScenesFolderWidget, "json");

    currentProjectsFolders = Tools::getFolderFolders(currentProjectsFolderWidget);
    currentProjectsFolderFiles = Tools::getFolderFiles(currentProjectsFolderWidget, "json");

    currentShadersFolders = Tools::getFolderFolders(currentShadersFolderWidget);
    currentShadersFolderFiles = Tools::getFolderFiles(currentShadersFolderWidget, "json");

    RegisterWindows();
}

void GUIManager::RegisterWindows()
{
    windows.push_back({ GUITypes::GUIWindows::PROFILER, "Profiler", false, []() { Profiler::get()->DrawPropertiesGUI(); }} );
    windows.push_back({ GUITypes::GUIWindows::DEPTH_LIGTHS_MAPS, "Lights Depth Maps Viewer", false, []() { Brakeza::get()->GUI()->DrawLightsDepthMapsViewerWindow(); }} );
    windows.push_back({ GUITypes::GUIWindows::LOGGING, "Logging/Console", false, []() { Brakeza::get()->GUI()->DrawLoggingWindow(); }} );
    windows.push_back({ GUITypes::GUIWindows::SCENE_OBJECTS, "Scene Objects", false, []() { Brakeza::get()->GUI()->DrawSceneObjectsWindow(); }} );
}

void GUIManager::DrawSceneObjectsWindow()
{
    auto windowStatus = GetWindowStatus(GUITypes::GUIWindows::SCENE_OBJECTS);
    if (!windowStatus->isOpen) return;
    widgetObjects3D->Draw(selectedObjectIndex);
}

void GUIManager::DrawLoggingWindow()
{
    auto windowStatus = GetWindowStatus(GUITypes::GUIWindows::LOGGING);
    if (!windowStatus->isOpen) return;
    widgetConsole->Draw(windowStatus->label.c_str(), &windowStatus->isOpen);
}

void GUIManager::DrawLightsDepthMapsViewerWindow()
{
    auto windowStatus = GetWindowStatus(GUITypes::DEPTH_LIGTHS_MAPS);
    if (!windowStatus->isOpen) return;
    //if (!showLightsDepthMapsViewerWindow) return;

    SetNextWindowSize(350, 400);
    ImGui::SetNextWindowBgAlpha(GUITypes::GUIConstants::WINDOW_ALPHA);

    auto title = std::string("Lights Depth Maps Viewer: ");
    if (ImGui::Begin(title.c_str(), &windowStatus->isOpen, ImGuiWindowFlags_NoDocking)) {

        auto render = ComponentsManager::get()->getComponentRender();
        auto shaderShadowPassDebugLight = render->getShaderOGLShadowPassDebugLight();
        auto lights = render->getShaderOGLRenderForward()->getShadowMappingSpotLights();

        const int columns = 1;

        // Añadir padding a las celdas
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10.0f, 10.0f));

        if (ImGui::BeginTable("DepthMapsTable", columns, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchSame)) {
            float imageSize = 200.0f;

            // Añadir DirectionalLight primero
            ImGui::TableNextColumn();

            auto directionalLightText = std::string("DirectionalLight");
            float directionalLightTextAvailWidth = ImGui::GetContentRegionAvail().x;
            float directionalLightTextTextWidth = ImGui::CalcTextSize(directionalLightText.c_str()).x;
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (directionalLightTextAvailWidth - directionalLightTextTextWidth) * 0.5f);
            ImGui::Text("%s", directionalLightText.c_str());

            // Centrar imagen
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (directionalLightTextAvailWidth - imageSize) * 0.5f);
            ImGui::Image(reinterpret_cast<ImTextureID>(shaderShadowPassDebugLight->getSceneTexture()), ImVec2(imageSize, imageSize));

            // Luego añadir los SpotLights
            int i = 0;
            for (const auto l: lights) {
                ImGui::TableNextColumn();

                // Centrar texto combinado
                auto combinedText = l->getLabel() + " / Layer: " + std::to_string(i);
                float availWidth = ImGui::GetContentRegionAvail().x;
                float textWidth = ImGui::CalcTextSize(combinedText.c_str()).x;
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (availWidth - textWidth) * 0.5f);
                ImGui::Text("%s", combinedText.c_str());

                // Centrar imagen
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (availWidth - imageSize) * 0.5f);
                ImGui::Image(reinterpret_cast<ImTextureID>(shaderShadowPassDebugLight->getInternalTexture(i)), ImVec2(imageSize, imageSize));

                i++;
            }
            ImGui::EndTable();
        }
        ImGui::PopStyleVar();
    }
    ImGui::End();
}

void GUIManager::DrawRegisteredWindows() {
    for (auto window : windows) {
        if (!window.isOpen) continue;
        if (ImGui::Begin(window.label.c_str())) {
            window.functionCallBack();
        }
        ImGui::End();
    }
}

void GUIManager::DrawWidgets()
{
    if (ImGui::Begin("Object shaders")) {
        ShadersGUI::DrawShadersBySelectedObject(this);
    }
    ImGui::End();

    if (ImGui::Begin("Object Scripts")) {
        ScriptLuaGUI::DrawScriptsBySelectedObject(this);
    }
    ImGui::End();

    if (ImGui::Begin("Scripts")) {
        ScriptLuaGUI::DrawScriptsLuaFolderFiles(this, currentScriptsFolderWidget);
    }
    ImGui::End();

    if (ImGui::Begin("Object variables")) {
        ScriptLuaGUI::DrawObjectVariables(this);
    }
    ImGui::End();

    if (ImGui::Begin("Global variables")) {
        ScriptLuaGUI::drawGlobalVariables();
    }
    ImGui::End();

    widgetProjectSettings->DrawProjectSetupGUI();

    if (ImGui::Begin("Keyboard/Mouse")) {
        DrawKeyboardMouseSettings();
    }
    ImGui::End();

    if (ImGui::Begin("Shaders")) {
        ShadersGUI::DrawCustomShadersFolder(this, currentShadersFolderWidget);
    }
    ImGui::End();

    if (ImGui::Begin("Projects")) {
        FileSystemGUI::DrawProjectFiles(this, currentProjectsFolderWidget);
    }
    ImGui::End();

    if (ImGui::Begin("Scenes")) {
        FileSystemGUI::DrawScenesFolder(this, currentScenesFolderWidget);
    }
    ImGui::End();

    if (ImGui::Begin("Images")) {
        DrawImages();
    }
    ImGui::End();
}


void GUIManager::UpdateImGuiDocking()
{
    //bool show_demo_window = true;
    //ImGui::ShowDemoWindow(&show_demo_window);
    static bool opt_fullscreen_persistant = true;
    bool opt_fullscreen = opt_fullscreen_persistant;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each other.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen) {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &opt_fullscreen, window_flags);
    ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // DockSpace
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
}

void GUIManager::DrawGUIPlugins(bool &finish)
{
    widgetObjects3D->Draw(selectedObjectIndex);
    widgetObject3DProperties->Draw(selectedObjectIndex);
    widgetMenu->Draw(finish, showAboutWindow, showLightsDepthMapsViewerWindow);
    widgetToolbar->Draw();
}

void GUIManager::DrawGUI(float timedelta, bool &finish)
{
    UpdateImGuiDocking();

    DrawWidgets();
    DrawGUIPlugins(finish);
    Object3DGUI::DrawObjectSelectedGuizmoOperation();
    ShadersGUI::DrawEditShaderWindow(this);
    ScriptLuaGUI::DrawEditScriptWindow(this);
    Mesh3DGUI::DrawEditBonesMappingWindow(this);

    DrawRegisteredWindows();

    DrawSplash();
    ImGui::End();
}

GuiAddonConsole *GUIManager::getConsole() const
{
    return widgetConsole;
}

TexturePackage &GUIManager::getImGuiTextures()
{
    return icons;
}

bool GUIManager::isShowLightsDepthMapsViewerWindow()
{
    auto windowStatus = GetWindowStatus(GUITypes::DEPTH_LIGTHS_MAPS);

    return windowStatus->isOpen;
}

void GUIManager::setSelectedObjectIndex(int selectedObjectIndex)
{
    GUIManager::selectedObjectIndex = selectedObjectIndex;
}

void GUIManager::setSelectedObject(const Object3D *s)
{
    int i = 0;
    for (auto o: gameObjects) {
        if (s == o) {
            setSelectedObjectIndex(i);
        }
        i++;
    }
}

void GUIManager::ShowDeletePopup(const char* title, const char *message, const std::function<void()>& onConfirm)
{
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal(title, nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text(message);
        ImGui::Separator();

        if (ImGui::Button("OK", ImVec2(120, 0))) {
            onConfirm();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void GUIManager::DrawKeyboardMouseSettings()
{
    auto input = ComponentsManager::get()->getComponentInput();

    ImGui::Text(("Mouse motion: " + std::to_string(input->isMouseMotion())).c_str());
    ImGui::Text(("Mouse motion RelX: " + std::to_string(input->getMouseMotionXRel())).c_str());
    ImGui::Text(("Mouse motion RelY: " + std::to_string(input->getMouseMotionYRel())).c_str());
    ImGui::Separator();

    ImGui::Text(("Click Left: " + std::to_string(input->isClickLeft())).c_str());
    ImGui::Text(("Click Right: " + std::to_string(input->isClickRight())).c_str());
    ImGui::Text(("Click Left pressed: " + std::to_string(input->isLeftMouseButtonPressed())).c_str());
    ImGui::Text(("Click Right pressed: " + std::to_string(input->isRightMouseButtonPressed())).c_str());
    ImGui::Text(("Mouse Up: " + std::to_string(input->isMouseButtonUp())).c_str());
    ImGui::Text(("Mouse Down: " + std::to_string(input->isMouseButtonDown())).c_str());
    ImGui::Text(("Mouse Drag: " + std::to_string(input->isDrag())).c_str());

    ImGui::Separator();
    ImGui::Text(("Key event Down: " + std::to_string(input->isKeyEventDown())).c_str());
    ImGui::Text(("Key event Up: " + std::to_string(input->isKeyEventUp())).c_str());
    ImGui::Separator();
    ImGui::Text(("Enabled: " + std::to_string(input->isEnabled())).c_str());
    ImGui::Separator();
    ImGui::Text(("Game Controller Enabled: " + std::to_string(input->isGameControllerEnabled())).c_str());
    ImGui::Text(("Controller Button A: " + std::to_string(input->getControllerButtonA())).c_str());
    ImGui::Text(("Controller Button B: " + std::to_string(input->getControllerButtonB())).c_str());
    ImGui::Text(("Controller Button X: " + std::to_string(input->getControllerButtonX())).c_str());
    ImGui::Text(("Controller Button Y: " + std::to_string(input->getControllerButtonY())).c_str());
    ImGui::Text(("Controller AxisLeft X: " + std::to_string(input->getControllerAxisLeftX())).c_str());
    ImGui::Text(("Controller AxisLeft Y: " + std::to_string(input->getControllerAxisLeftY())).c_str());
    ImGui::Text(("Controller AxisRight X: " + std::to_string(input->getControllerAxisRightX())).c_str());
    ImGui::Text(("Controller AxisRight Y: " + std::to_string(input->getControllerAxisRightY())).c_str());
    ImGui::Text(("Controller TriggerLeft: " + std::to_string(input->getControllerAxisTriggerLeft())).c_str());
    ImGui::Text(("Controller TriggerRight: " + std::to_string(input->getControllerAxisTriggerRight())).c_str());
    ImGui::Text(("Controller Pad Up: " + std::to_string(input->getControllerPadUp())).c_str());
    ImGui::Text(("Controller Pad Down: " + std::to_string(input->getControllerPadDown())).c_str());
    ImGui::Text(("Controller Pad Left: " + std::to_string(input->getControllerPadLeft())).c_str());
    ImGui::Text(("Controller Pad Right: " + std::to_string(input->getControllerPadRight())).c_str());
    ImGui::Text(("Controller Shoulder Left: " + std::to_string(input->getControllerShoulderLeft())).c_str());
    ImGui::Text(("Controller Shoulder Right: " + std::to_string(input->getControllerShoulderRight())).c_str());
    ImGui::Text(("Controller Button Back: " + std::to_string(input->getControllerButtonBack())).c_str());
    ImGui::Text(("Controller Button Guide: " + std::to_string(input->getControllerButtonGuide())).c_str());
    ImGui::Text(("Controller Button Start: " + std::to_string(input->getControllerButtonStart())).c_str());
}

void GUIManager::DrawImages()
{
    static ImGuiTableFlags flags = ImGuiTableFlags_SizingStretchSame;

    auto imageFiles = imagesFolder.getItems();
    int columns = 8; // Máximo de 6 imágenes por fila

    if (ImGui::BeginTable("ImagesTable", columns, flags)) {
        int count = 0;
        for (auto image : imageFiles) {
            if (count % columns == 0) {
                ImGui::TableNextRow();
            }
            ImGui::TableNextColumn();

            float columnWidth = ImGui::GetColumnWidth();
            float cursorX = ImGui::GetCursorPosX() + (columnWidth - 96) * 0.5f;
            ImGui::SetCursorPosX(cursorX);

            ImGui::BeginGroup();

            ImGui::PushID(image->label.c_str());
            ImGui::ImageButton(reinterpret_cast<ImTextureID>(image->texture->getOGLTextureID()), ImVec2(96, 96));

            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                ImGui::SetDragDropPayload("IMAGE_ITEM", image->label.c_str(), image->label.size() + 1);
                ImGui::Text("%s", image->label.c_str());
                ImGui::EndDragDropSource();
            }
            ImGui::PopID();

            ImVec2 textSize = ImGui::CalcTextSize(image->label.c_str());
            ImGui::SetCursorPosX(cursorX + (96 - textSize.x) * 0.5f);
            ImGui::Text("%s", image->label.c_str());

            char sizeText[32];
            snprintf(sizeText, sizeof(sizeText), "%d x %d", image->texture->width(), image->texture->height());
            ImVec2 sizeTextSize = ImGui::CalcTextSize(sizeText);
            ImGui::SetCursorPosX(cursorX + (96 - sizeTextSize.x) * 0.5f);
            ImGui::Text("%s", sizeText);

            ImGui::EndGroup();
            ImGui::Dummy(ImVec2(0.0f, 10.0f));

            count++;
        }

        int remaining = columns - (count % columns);
        if (remaining != columns) {
            for (int i = 0; i < remaining; i++) {
                ImGui::TableNextColumn();
            }
        }

        ImGui::EndTable();
    }
}

void GUIManager::SetNextWindowSize(int w, int h)
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 window_size = ImVec2(w, h);

    ImVec2 center = viewport->GetCenter();
    ImVec2 pos = ImVec2(center.x - window_size.x * 0.5f, center.y - window_size.y * 0.5f);

    ImGui::SetNextWindowSize(window_size);
    ImGui::SetNextWindowPos(pos, ImGuiCond_Appearing);
}

void GUIManager::openBoneInfoDialog()
{
    showBoneMappingsEditorWindow = true;
}

void GUIManager::DrawSplash()
{
    if (BrakezaSetup::get()->ENABLE_SPLASH) {
        ImGui::OpenPopup("brakeza_splash");
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        BrakezaSetup::get()->ENABLE_SPLASH = false;
    }

    if (ImGui::BeginPopup("brakeza_splash")) {
        ImGui::SeparatorText("Welcome to Brakeza3D!");
        ImGui::Image(FileSystemGUI::IconTag(IconsByGUI::SPLASH), ImVec2(640, 350));
        ImGui::SeparatorText(
            std::string("Brakeza3D (" + BrakezaSetup::get()->ENGINE_VERSION + ") | " + BrakezaSetup::get()->ENGINE_TITLE).c_str()
        );

        if (Brakeza::get()->getEngineTotalTime() > BrakezaSetup::get()->SPLASH_COUNTDOWN_TIME) {
            ImGui::CloseCurrentPopup();
            ComponentsManager::get()->getComponentInput()->setEnabled(true);
        }

        ImGui::EndPopup();
    }
}

GUITypes::GUIWindowsStatus* GUIManager::GetWindowStatus(GUITypes::GUIWindows window)
{
    for (auto& status : windows) {
        if (status.window == window) {
            return &status;
        }
    }
    return nullptr;
}