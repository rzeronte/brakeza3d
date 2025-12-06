
#include "../../include/Brakeza.h"
#include "../../include/Components/ComponentsManager.h"
#include "../../include/Render/Profiler.h"
#include "../../include/GUI/GUIManager.h"
#include "../../include/GUI/Objects/Object3DGUI.h"
#include "../../include/GUI/Objects/ShadersGUI.h"
#include "../../include/GUI/Objects/ScriptLuaGUI.h"
#include "../../include/GUI/Objects/Mesh3DAnimationGUI.h"
#include "../../include/GUI/Objects/FileSystemGUI.h"
#include "../../include/GUI/Objects/IconsGUI.h"
#include "../../include/GUI/AddOns/GUIConsole.h"
#include "../../include/GUI/AddOns/GUIAddonObject3DProperties.h"
#include "../../include/GUI/AddOns/GUIAddonObjects3D.h"
#include "../../include/GUI/AddOns/GUIAddonMenu.h"
#include "../../include/GUI/AddOns/GUIAddonProjectSetup.h"
#include "../../include/GUI/AddOns/GUIAddonToolbar.h"

#define ADD_WIN(title, type, icon, visible, func) \
windows.push_back({ title, type, icon, visible, [&] { func; }})

GUIManager::GUIManager(std::vector<Object3D *> &gameObjects)
:
    gameObjects(gameObjects),
    widgetConsole(new GuiAddonConsole(ComponentsManager::get()->getComponentScripting()->getLua())),
    browserScenes(GUI::CreateBrowserCache(Config::get()->SCENES_FOLDER, Config::get()->SCENES_EXT)),
    browserProjects(GUI::CreateBrowserCache(Config::get()->PROJECTS_FOLDER, Config::get()->PROJECTS_EXT)),
    browserShaders(GUI::CreateBrowserCache(Config::get()->CUSTOM_SHADERS_FOLDER, Config::get()->SHADERS_EXT)),
    browserScripts(GUI::CreateBrowserCache(Config::get()->SCRIPTS_FOLDER, Config::get()->SCRIPTS_EXT)),
    textureAtlas(new TextureAtlas())
{
    FileSystemGUI::LoadImagesFolder(this);
    textureAtlas->CreateFromSheet(Config::get()->ICONS_FOLDER + Config::get()->GUI_ICON_SHEET, 32, 32);
    IconsGUI::ImportIconsFromJSON(Config::get()->CONFIG_FOLDER + Config::get()->ICONS_CONFIG);

    Profiler::get()->CaptureGUIMemoryUsage();
    RegisterAllowedItemsForViewer();
    RegisterMenu();
    RegisterWindows();
}

void GUIManager::RegisterWindows()
{
    ADD_WIN("Project setup",           GUIType::PROJECT_SETTINGS, IconGUI::WIN_DEPTH_LIGHTS_MAPS,    true,  GUIAddonProjectSetup::DrawProjectSetupGUI(this));
    ADD_WIN("Scene Objects",           GUIType::SCENE_OBJECTS,    IconGUI::WIN_DEPTH_LIGHTS_MAPS,    true,  GUIAddonObjects3D::DrawSceneObjects(this));
    ADD_WIN("Object Properties",       GUIType::OBJECT_PROPS,     IconGUI::WIN_OBJECT_PROPS,         true,  GUIAddonObject3DProperties::DrawPropertiesBySelectedObject(this));
    ADD_WIN("Object shaders",          GUIType::OBJECT_SHADERS,   IconGUI::WIN_OBJECT_SHADERS,       false, ShadersGUI::DrawShadersBySelectedObject(this));
    ADD_WIN("Object Scripts",          GUIType::OBJECT_SCRIPTS,   IconGUI::WIN_OBJECT_SCRIPTS,       false, ScriptLuaGUI::DrawScriptsBySelectedObject(this));
    ADD_WIN("Object variables",        GUIType::OBJECT_VARS,      IconGUI::WIN_OBJECT_VARS,          false, ScriptLuaGUI::DrawObjectVariables(this));
    ADD_WIN("Global variables",        GUIType::GLOBAL_VARS,      IconGUI::WIN_GLOBAL_VARS,          false, ScriptLuaGUI::DrawGlobalVariables(this));
    ADD_WIN("Keyboard/Mouse",          GUIType::KEYBOARD_MOUSE,   IconGUI::WIN_KEYBOARD_MOUSE,       false, WindowKeyboardMouseSetup());
    ADD_WIN("Images",                  GUIType::IMAGES,           IconGUI::WIN_IMAGES,               false, WindowImages());

    ADD_WIN("Projects",                GUIType::FILES_PROJECTS,   IconGUI::WIN_FILES_PROJECTS,       true,  FileSystemGUI::DrawProjectFiles(this, browserProjects));
    ADD_WIN("Scenes",                  GUIType::FILES_SCENES,     IconGUI::WIN_FILES_SCENES,         true,  FileSystemGUI::DrawScenesFolder(this, browserScenes));
    ADD_WIN("Scripts",                 GUIType::FILES_SCRIPTS,    IconGUI::WIN_FILES_SCRIPTS,        true,  ScriptLuaGUI::DrawScriptsLuaFolderFiles(this, browserScripts));
    ADD_WIN("Shaders",                 GUIType::FILES_SHADERS,    IconGUI::WIN_FILES_SHADERS,        true,  ShadersGUI::DrawCustomShadersFolder(this, browserShaders));

    ADD_WIN("Logging/Console",         GUIType::LOGGING,          IconGUI::WIN_LOGGING,              true,  widgetConsole->Draw());
    ADD_WIN("Lights DepthMaps Viewer", GUIType::DEPTH_LIGHTS_MAPS,IconGUI::WIN_DEPTH_LIGHTS_MAPS,    false, WindowLightsDepthMapsViewer());
    ADD_WIN("Profiler",                GUIType::PROFILER,         IconGUI::WIN_PROFILER,             false, Profiler::get()->DrawPropertiesGUI());
    ADD_WIN("Debug GUI Icons",         GUIType::DEBUG_ICONS,      IconGUI::WIN_DEBUG_ICONS,          false, IconsGUI::DrawDebugIconsWindow(this));
}

void GUIManager::RegisterAllowedItemsForViewer()
{
    visibleTypeObjects = {
        { "Object3D",               TypeObject::Object3D,             IconObject::OBJECT_3D, true },
        { "Image2D",                TypeObject::Image2D,              IconObject::IMAGE_2D, true },
        { "Image2DAnimation",       TypeObject::Image2DAnimation,     IconObject::IMAGE_2D_ANIMATION, true },
        { "Mesh3D",                 TypeObject::Mesh3D,               IconObject::MESH_3D, true  },
        { "Mesh3DAnimation",        TypeObject::Mesh3DAnimation,      IconObject::MESH_3D_ANIMATION, true  },
        { "Image3D",                TypeObject::Image3D,              IconObject::IMAGE_3D, true },
        { "Image3DAnimation",       TypeObject::Image3DAnimation,     IconObject::IMAGE_3D_ANIMATION, true },
        { "Image3DAnimation360",    TypeObject::Image3DAnimation360,  IconObject::IMAGE_3D_ANIMATION_360, true },
        { "LightPoint",             TypeObject::LightPoint,           IconObject::LIGHT_POINT, true },
        { "LightSpot",              TypeObject::LightSpot,            IconObject::LIGHT_SPOT, true },
        { "ParticleEmitter",        TypeObject::ParticleEmitter,      IconObject::PARTICLE_EMITTER, true }
    };
}

void GUIManager::RegisterMenu()
{
    menus = {
        {"Brakeza3D",       IconGUI::MNU_BRAKEZA,           [&] { GUIAddonMenu::MenuBrakeza3D(); }},
        {"Script Controls", IconGUI::MNU_SCRIPT_CONTROLS,   [&] { GUIAddonMenu::MenuScriptControls(); }},
        {"Add Object",      IconGUI::MNU_ADD_OBJECT,        [&] { GUIAddonMenu::MenuAddObject(); }},
        {"Video",           IconGUI::MNU_VIDEO,             [&] { GUIAddonMenu::MenuVideo(); }},
        {"Colliders",       IconGUI::MNU_COLLIDERS,         [&] { GUIAddonMenu::MenuColliders(); }},
        {"Illumination",    IconGUI::MNU_ILLUMINATION,      [&] { GUIAddonMenu::MenuIllumination(); }},
        {"Camera",          IconGUI::MNU_CAMERA,            [&] { GUIAddonMenu::MenuCamera(); }},
        {"Sound",           IconGUI::MNU_SOUND,             [&] { GUIAddonMenu::MenuSound(); }},
        {"Logging",         IconGUI::MNU_LOGGING,           [&] { GUIAddonMenu::MenuLogging(); }},
        {"Layouts",         IconGUI::MNU_LAYOUTS,           [&] { GUIAddonMenu::MenuLayout(); }},
        {"Windows",         IconGUI::MNU_WINDOWS,           [&] { GUIAddonMenu::MenuWindow(this); }},
    };
}

void GUIManager::DrawGUI()
{
    UpdateImGuiDocking();

    GUIAddonMenu::Draw(this);
    GUIAddonToolbar::Draw();

    Object3DGUI::DrawSelectedObjectGuizmo();
    ShadersGUI::DrawEditShaderWindow(this);
    ScriptLuaGUI::DrawEditScriptWindow(this);
    Mesh3DAnimationDrawerGUI::DrawEditBonesMappingWindow(this);
    IconsGUI::DrawDebugIconsWindow(this);
    DrawRegisteredWindows();
    DrawSplashWindow();

    ImGui::End();
}

void GUIManager::DrawRegisteredWindows()
{
    for (auto& window : windows) {
        if (!window.isOpen) continue;
        if (ImGui::Begin(window.label.c_str(), &window.isOpen, ImGuiWindowFlags_NoFocusOnAppearing)) {
            window.functionCallBack();
        }
        ImGui::End();
    }
}

void GUIManager::UpdateImGuiDocking()
{
    //bool show_demo_window = true;
    //ImGui::ShowDemoWindow(&show_demo_window);
    static bool opt_fullscreen_persistant = true;
    bool opt_fullscreen = opt_fullscreen_persistant;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

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

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &opt_fullscreen, window_flags);
    ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // DockSpace
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
}

GuiAddonConsole *GUIManager::getConsole() const
{
    return widgetConsole;
}

bool GUIManager::isLightDepthMapsViewerWindowOpen()
{
    auto windowStatus = getWindowStatus(GUIType::DEPTH_LIGHTS_MAPS);

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

void GUIManager::WindowKeyboardMouseSetup()
{
    auto windowStatus = getWindowStatus(GUIType::KEYBOARD_MOUSE);
    if (!windowStatus->isOpen) return;

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

TextureAtlas * GUIManager::getTextureAtlas() const
{
    return textureAtlas;
}

GUIType::FolderBrowserCache GUIManager::getBrowserScripts() const
{
    return browserScripts;
}

GUIType::FolderBrowserCache GUIManager::getBrowserScenes() const
{
    return browserScenes;
}

GUIType::FolderBrowserCache GUIManager::getBrowserProjects() const
{
    return browserProjects;
}

GUIType::FolderBrowserCache GUIManager::getBrowserShaders() const
{
    return browserShaders;
}

int& GUIManager::selectedObjectIndexPointer()
{
    return selectedObjectIndex;
}

void GUIManager::WindowImages()
{
    auto windowStatus = getWindowStatus(GUIType::IMAGES);
    if (!windowStatus->isOpen) return;

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
    ImVec2 window_size = ImVec2((float) w, (float) h);

    ImVec2 center = viewport->GetCenter();
    ImVec2 pos = ImVec2(center.x - window_size.x * 0.5f, center.y - window_size.y * 0.5f);

    ImGui::SetNextWindowSize(window_size);
    ImGui::SetNextWindowPos(pos, ImGuiCond_Appearing);
}

void GUIManager::OpenBoneInfoDialog()
{
    showBoneMappingsEditorWindow = true;
}


void GUIManager::WindowLightsDepthMapsViewer()
{
    auto windowStatus = getWindowStatus(GUIType::DEPTH_LIGHTS_MAPS);
    if (!windowStatus->isOpen) return;

    SetNextWindowSize(350, 400);
    ImGui::SetNextWindowBgAlpha(GUIType::Levels::WINDOW_ALPHA);

    auto title = std::string("Lights Depth Maps Viewer: ");
    if (ImGui::Begin(title.c_str(), &windowStatus->isOpen, ImGuiWindowFlags_NoDocking)) {

        auto render = ComponentsManager::get()->getComponentRender();
        auto shaderShadowPassDebugLight = render->getShaderOGLShadowPassDebugLight();
        auto lights = render->getShaderOGLRenderForward()->getShadowMappingSpotLights();

        // Añadir padding a las celdas
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10.0f, 10.0f));

        if (ImGui::BeginTable("DepthMapsTable", 1, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchSame)) {
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
                auto combinedText = std::string(l->getName()) + " / Layer: " + std::to_string(i);
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

void GUIManager::DrawSplashWindow()
{
    if (!Config::get()->ENABLE_SPLASH) return;

    float currentTime = Brakeza::get()->getEngineTotalTime();
    float countdownTime = Config::get()->SPLASH_COUNTDOWN_TIME;
    float fadeDuration = 1.5f;

    if (currentTime >= countdownTime - fadeDuration) {
        splashAlpha = (countdownTime - currentTime) / fadeDuration;
        splashAlpha = glm::clamp(splashAlpha, 0.0f, 1.0f);
        Config::get()->ENABLE_SPLASH = false;
    }

    ImVec2 size((float) splashImage->width(), (float) splashImage->height());

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 tamañoPantalla = io.DisplaySize;

    ImVec2 positionCentered;
    positionCentered.x = (tamañoPantalla.x - size.x) * 0.5f;
    positionCentered.y = (tamañoPantalla.y - size.y) * 0.5f;

    ImGui::SetNextWindowPos(positionCentered);
    ImGui::SetNextWindowSize(size);
    ImGui::Image(
        splashImage->getOGLImTexture(),
        size,
        ImVec2(0, 0),
        ImVec2(1, 1),
        ImVec4(1.0f, 1.0f, 1.0f, splashAlpha)
    );
}

GUIType::WindowData* GUIManager::getWindowStatus(GUIType::Window window)
{
    for (auto& status : windows) {
        if (status.window == window) {
            return &status;
        }
    }
    return nullptr;
}

void GUIManager::WelcomeMessage()
{
    Logging::Error("ese %d", 1);
    Logging::Message("############################################################");
    Logging::Message("");
    Logging::Message("***********************");
    Logging::Message("*  B R A K E Z A 3 D  *");
    Logging::Message("***********************");
    Logging::Message("");
    Logging::Message("Open source game engine for developers");
    Logging::Message("############################################################");
    Logging::Message("");
    Logging::Message(Config::get()->ENGINE_WEBSITE.c_str());
    Logging::Message(Config::get()->ENGINE_SOURCE_WEBSITE.c_str());
    Logging::Message("");
    Logging::Message("%s", Config::get()->ENGINE_TITLE.c_str());
    Logging::Message("");
    Logging::Message("############################################################");
    Logging::Message("");
}