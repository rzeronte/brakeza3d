
#include "../../include/Brakeza.h"
#include "../../include/Components/Components.h"
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
#include "../../include/Render/Drawable.h"
#include "../../include/GUI/AddOns/GUIAddonDocumentation.h"
#include "../../include/GUI/Objects/StatusBarGUI.h"
#include "../../include/GUI/Objects/ThreadGUI.h"
#include "../../include/Loaders/SceneChecker.h"

#define ADD_WIN(title, type, icon, visible, internal, dockable, isObjectWindow, func, minSize, maxSize) \
windows.push_back({ title, type, icon, visible, internal, dockable, isObjectWindow, [&] { func; }, minSize, maxSize})

GUIManager::GUIManager()
:
    widgetConsole(new GuiAddonConsole()),
    textureAtlas(new TextureAtlas()),
    resourceHub(new GUIAddonResourceHub())
{
}

void GUIManager::OnStart()
{
    auto config = Config::get();

    // Load texture sheet icons
    textureAtlas->CreateFromSheet(
        config->ICONS_FOLDER + config->GUI_ICON_SHEET,
        config->GUI_ICON_SHEET_W,
        config->GUI_ICON_SHEET_H
    );
    IconsGUI::ImportIconsFromJSON(config->CONFIG_FOLDER + config->ICONS_CONFIG);
    Profiler::get()->CaptureGUIMemoryUsage();

    widgetConsole->setLua(&Components::get()->Scripting()->getLua());

    // Load splash
    splashImage = new Image(config->IMAGES_FOLDER + config->SPLASH_FILENAME);

    LoadBrowserFolders();
    RegisterAllowedItemsForViewer();
    RegisterMenu();
    RegisterWindows();
    LoadDocumentation();
    FileSystemGUI::LoadImagesFolder(browserImages, browserImagesTextures);
    GUI::ShowLoadTime("Time until GUIManager get ready", *Brakeza::get()->getTimer());
}

void GUIManager::RegisterWindows()
{
    ADD_WIN("Project Setup",       GUIType::PROJECT_SETTINGS,    IconGUI::WIN_PROJECT_SETTINGS,  true,  false, true,  false,  GUIAddonProjectSetup::DrawWinProjectSettings(),                            ImVec2(300, 300), ImVec2(FLT_MAX, FLT_MAX));
    ADD_WIN("File Browser",        GUIType::BROWSER,             IconGUI::WIN_BROWSER,           true,  false, true,  false,  FileSystemGUI::DrawMainBrowser(),                                          ImVec2(450, 300), ImVec2(FLT_MAX, FLT_MAX));
    ADD_WIN("Scene Objects",       GUIType::SCENE_OBJECTS,       IconGUI::WIN_SCENE_OBJECTS,     true,  false, true,  false,  GUIAddonObjects3D::DrawWinSceneObjects(this),                          ImVec2(340, 500), ImVec2(FLT_MAX, FLT_MAX));
    ADD_WIN("Object Properties",   GUIType::OBJECT_PROPS,        IconGUI::WIN_OBJECT_PROPS,      true,  false, true,  true,   GUIAddonObject3DProperties::DrawWinObjectProps(this),                  ImVec2(340, 400), ImVec2(FLT_MAX, FLT_MAX));
    ADD_WIN("Object Shaders",      GUIType::OBJECT_SHADERS,      IconGUI::WIN_OBJECT_SHADERS,    true, false, true,  true,    ShadersGUI::DrawWinObjectShaders(),                                        ImVec2(350, 275), ImVec2(FLT_MAX, FLT_MAX));
    ADD_WIN("Object Scripts",      GUIType::OBJECT_SCRIPTS,      IconGUI::WIN_OBJECT_SCRIPTS,    true, false, true,  true,    ScriptLuaGUI::DrawWinObjectScripts(),                                      ImVec2(350, 275), ImVec2(FLT_MAX, FLT_MAX));
    ADD_WIN("Object Variables",    GUIType::OBJECT_VARS,         IconGUI::WIN_OBJECT_VARS,       false, false, true,  true,   ScriptLuaGUI::DrawWinObjectVars(this),                                 ImVec2(350, 275), ImVec2(FLT_MAX, FLT_MAX));
    ADD_WIN("Global Variables",    GUIType::GLOBAL_VARS,         IconGUI::WIN_GLOBAL_VARS,       false, false, true,  false,  ScriptLuaGUI::DrawWinGlobalVars(this),                                 ImVec2(350, 275), ImVec2(FLT_MAX, FLT_MAX));
    ADD_WIN("Keyboard/Mouse",      GUIType::KEYBOARD_MOUSE,      IconGUI::WIN_KEYBOARD_MOUSE,    false, false, true,  false,  DrawWinKeyboardMouse(),                                                    ImVec2(350, 400), ImVec2(FLT_MAX, FLT_MAX));
    ADD_WIN("Images",              GUIType::IMAGES,              IconGUI::WIN_IMAGES,            false, false, true,  false,  FileSystemGUI::DrawWinImages(browserImages, browserImagesTextures),  ImVec2(600, 550), ImVec2(FLT_MAX, FLT_MAX));
    ADD_WIN("Logs/Console",        GUIType::LOGGING,             IconGUI::WIN_LOGGING,           true,  false, true,  false,  widgetConsole->DrawWinLogging(),                                           ImVec2(400, 300), ImVec2(FLT_MAX, FLT_MAX));
    ADD_WIN("Lights DepthMaps",    GUIType::DEPTH_LIGHTS_MAPS,   IconGUI::WIN_DEPTH_LIGHTS_MAPS, false, false, true,  false,  DrawWinDepthLightsMap(),                                                   ImVec2(400, 500), ImVec2(FLT_MAX, FLT_MAX));
    ADD_WIN("Profiler",            GUIType::PROFILER,            IconGUI::WIN_PROFILER,          false, false, false,  false, Profiler::get()->DrawWinProfiler(),                                        ImVec2(800, 600), ImVec2(FLT_MAX, FLT_MAX));
    ADD_WIN("Code/Nodes editor",   GUIType::CODE_EDITOR,         IconGUI::WIN_CODE_EDITOR,       false, false, true,  false,  DrawWinEditableOpenResources(),                                            ImVec2(650, 400), ImVec2(FLT_MAX, FLT_MAX));
    ADD_WIN("GUI Icons",           GUIType::DEBUG_ICONS,         IconGUI::WIN_DEBUG_ICONS,       false, false, false, false,  IconsGUI::DrawWinDebugIcons(this),                                     ImVec2(650, 500), ImVec2(FLT_MAX, FLT_MAX));
    ADD_WIN("Documentation",       GUIType::DOCUMENTATION,       IconGUI::WIN_DOCUMENTATION,     false, true,  false, false,  GUIAddonDocumentation::DrawWinDocumentation(documentationTree),            ImVec2(400, 400), ImVec2(FLT_MAX, FLT_MAX));
    ADD_WIN("Scene Detail",        GUIType::SCENE_INFO,          IconGUI::SCENE_INFO,            false, true,  false, false,  sceneChecker.DrawWinSceneInfo(),                                           ImVec2(600, 450), ImVec2(FLT_MAX, FLT_MAX));
    ADD_WIN("Project Detail",      GUIType::PROJECT_INFO,        IconGUI::PROJECT_INFO,          false, true,  false, false,  projectChecker.DrawWinProjectInfo(),                                       ImVec2(600, 450), ImVec2(FLT_MAX, FLT_MAX));
    ADD_WIN("Threads",             GUIType::THREADS,             IconGUI::WIN_THREADS,           false, false, false, false,  ThreadGUI::MenuWorkers(),                                                  ImVec2(550, 650), ImVec2(FLT_MAX, FLT_MAX));

    RegisterDefaultLayoutWindows();
}

void GUIManager::RegisterDefaultLayoutWindows()
{
    defaultLayoutWindowsConfig =  {
        { GUIType::PROJECT_SETTINGS, true },
        { GUIType::SCENE_OBJECTS, true },
        { GUIType::OBJECT_PROPS, true },
        { GUIType::OBJECT_SHADERS, true },
        { GUIType::OBJECT_SCRIPTS, true },
        { GUIType::OBJECT_VARS, false },
        { GUIType::GLOBAL_VARS, false },
        { GUIType::KEYBOARD_MOUSE,  false },
        { GUIType::IMAGES, false },
        { GUIType::LOGGING, true },
        { GUIType::DEPTH_LIGHTS_MAPS, false },
        { GUIType::PROFILER, false },
        { GUIType::DEBUG_ICONS, false },
        { GUIType::CODE_EDITOR, false},
        { GUIType::SCENE_INFO, false},
        { GUIType::THREADS, false},
        { GUIType::BROWSER, true},
    };

    devsLayoutWindowsConfig =  {
        { GUIType::PROJECT_SETTINGS, true },
        { GUIType::SCENE_OBJECTS, true },
        { GUIType::OBJECT_PROPS, true },
        { GUIType::OBJECT_SHADERS, true },
        { GUIType::OBJECT_SCRIPTS, true },
        { GUIType::OBJECT_VARS, true },
        { GUIType::GLOBAL_VARS, false },
        { GUIType::KEYBOARD_MOUSE,  false },
        { GUIType::IMAGES, false },
        { GUIType::LOGGING, true },
        { GUIType::DEPTH_LIGHTS_MAPS, false },
        { GUIType::PROFILER, false },
        { GUIType::DEBUG_ICONS, false },
        { GUIType::CODE_EDITOR, true},
        { GUIType::SCENE_INFO, false},
        { GUIType::THREADS, false},
        { GUIType::BROWSER, true},
    };

    designLayoutWindowsConfig =  {
        { GUIType::PROJECT_SETTINGS, false },
        { GUIType::SCENE_OBJECTS, false },
        { GUIType::OBJECT_PROPS, false },
        { GUIType::OBJECT_SHADERS, false },
        { GUIType::OBJECT_SCRIPTS, false },
        { GUIType::OBJECT_VARS, false },
        { GUIType::GLOBAL_VARS, false },
        { GUIType::KEYBOARD_MOUSE,  false },
        { GUIType::IMAGES, false },
        { GUIType::LOGGING, false },
        { GUIType::DEPTH_LIGHTS_MAPS, false },
        { GUIType::PROFILER, false },
        { GUIType::DEBUG_ICONS, false },
        { GUIType::CODE_EDITOR, false },
        { GUIType::SCENE_INFO, false},
        { GUIType::THREADS, false},
        { GUIType::BROWSER, false},
    };
}

void GUIManager::RegisterAllowedItemsForViewer()
{
    visibleTypeObjects = {
        { "Object",                  ObjectType::Object3D,             IconObject::OBJECT_3D, true },
        { "Image 2D",                ObjectType::Image2D,              IconObject::IMAGE_2D, true },
        { "Image 2D animation",      ObjectType::Image2DAnimation,     IconObject::IMAGE_2D_ANIMATION, true },
        { "Mesh 3D",                 ObjectType::Mesh3D,               IconObject::MESH_3D, true  },
        { "Mesh 3D animation",       ObjectType::Mesh3DAnimation,      IconObject::MESH_3D_ANIMATION, true  },
        { "Image 3D",                ObjectType::Image3D,              IconObject::IMAGE_3D, true },
        { "Image 3D animation",      ObjectType::Image3DAnimation,     IconObject::IMAGE_3D_ANIMATION, true },
        { "Image 3D animation 360",  ObjectType::Image3DAnimation360,  IconObject::IMAGE_3D_ANIMATION_360, true },
        { "Light Point",             ObjectType::LightPoint,           IconObject::LIGHT_POINT, true },
        { "Light Spot",              ObjectType::LightSpot,            IconObject::LIGHT_SPOT, true },
        { "Particle Emitter",        ObjectType::ParticleEmitter,      IconObject::PARTICLE_EMITTER, true }
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
        {"Logs",            IconGUI::MNU_LOGGING,           [&] { GUIAddonMenu::MenuLogging(); }},
        {"Layouts",         IconGUI::MNU_LAYOUTS,           [&] { GUIAddonMenu::MenuLayout(); }},
        {"Assets Hub",      IconGUI::MNU_RESOURCESHUB,      [&] { GUIAddonMenu::MenuResourcesHub(); }},
        {"Windows",         IconGUI::MNU_WINDOWS,           [&] { GUIAddonMenu::MenuWindow(this); }},
    };
}

void GUIManager::DrawGUI()
{
    UpdateImGuiDocking();

    GUIAddonMenu::Draw(this);
    GUIAddonToolbar::Draw();

    Object3DGUI::DrawSelectedObjectGuizmo();
    Mesh3DAnimationDrawerGUI::DrawEditBonesMappingWindow(this);

    DrawRegisteredWindows();
    DrawSplashWindow();
    resourceHub->render();

    StatusBarGUI::Render(resourceHub);

    CloseRemovedEditableOpenFiles();

    ImGui::End();
}

void GUIManager::LoadBrowserFolders()
{
    browserScenes = GUI::CreateBrowserCache(
        Config::get()->SCENES_FOLDER,
        Config::get()->SCENES_EXT,
        [this, &cache = browserScenes]() {
            cache.folderFolders = Tools::getFolderFolders(cache.currentFolder);
            cache.folderFiles = Tools::getFolderFiles(cache.currentFolder, cache.ext);
        }
    );

    browserProjects = GUI::CreateBrowserCache(
        Config::get()->PROJECTS_FOLDER,
        Config::get()->PROJECTS_EXT,
        [this, &cache = browserProjects]() {
            cache.folderFolders = Tools::getFolderFolders(cache.currentFolder);
            cache.folderFiles = Tools::getFolderFiles(cache.currentFolder, cache.ext);
        }
    );

    browserShaders = GUI::CreateBrowserCache(
        Config::get()->CUSTOM_SHADERS_FOLDER,
        Config::get()->SHADERS_EXT,
        [this, &cache = browserShaders]() {
            cache.folderFolders = Tools::getFolderFolders(cache.currentFolder);
            cache.folderFiles = Tools::getFolderFiles(cache.currentFolder, cache.ext);
        }
    );

    browserScripts = GUI::CreateBrowserCache(
        Config::get()->SCRIPTS_FOLDER,
        Config::get()->SCRIPTS_EXT,
        [this, &cache = browserScripts]() {
            cache.folderFolders = Tools::getFolderFolders(cache.currentFolder);
            cache.folderFiles = Tools::getFolderFiles(cache.currentFolder, cache.ext);
        }
    );

    browserImages = GUI::CreateBrowserCache(
        Config::get()->IMAGES_FOLDER,
        Config::get()->IMAGES_EXT,
        [this, &cache = browserImages]() {
            cache.folderFolders = Tools::getFolderFolders(cache.currentFolder);
            cache.folderFiles = Tools::getFolderFiles(cache.currentFolder, cache.ext);
            FileSystemGUI::LoadImagesFolder(cache, browserImagesTextures);
        }
    );
}

void GUIManager::CloseRemovedEditableOpenFiles()
{
    for (auto it = openFiles.begin(); it != openFiles.end(); ) {
        if ((*it)->isRemoved()) {
            delete *it;
            it = openFiles.erase(it);
        } else {
            ++it;
        }
    }
}

void GUIManager::DrawWinEditableOpenResources() const
{
    if (ImGui::BeginTabBar("FileTabs")) {
        int i = 0;
        for (auto &f : openFiles) {
            f->DrawTabEdition(i);
            i++;
        }

        if (Brakeza::get()->GUI()->getIndexCodeEditorTab() != -1) {
            Brakeza::get()->GUI()->ResetIndexCodeEditor();
        }

        ImGui::EndTabBar();
    }

    if (openFiles.empty()) {
        Drawable::WarningMessage("Open a file to start...");
    }
}

void GUIManager::DrawRegisteredWindows()
{
    bool selected = Components::get()->Render()->getSelectedObject() != nullptr;

    for (auto& window : windows) {
        if (!window.isOpen) continue;

        if (!selected && window.autoHideIfNotSelected) continue;

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoFocusOnAppearing;

        if (!window.isDockable) {
            flags |= ImGuiWindowFlags_NoDocking;
        }

        // Centrar ventanas no dockables al aparecer
        if (!window.isDockable) {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImVec2 center = viewport->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        }

        ImGui::SetNextWindowBgAlpha(0.99f);
        ImGui::SetNextWindowSizeConstraints(window.minSize, window.maxSize);

        if (ImGui::Begin(window.label.c_str(), &window.isOpen, flags)) {
            window.functionCallBack();

            if (window.isObjectWindow) {
                float statusBarHeight = GetObjectStatusBarHeight();

                float availableHeight = ImGui::GetContentRegionAvail().y - statusBarHeight + 8.0f;

                ImGui::BeginChild(
                    "WindowContent",
                    ImVec2(0, availableHeight),
                    false,
                    ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
                );
                ImGui::EndChild();

                DrawObjectWindowStatusBar(window);
            }
        }
        ImGui::End();
    }
}

void GUIManager::DrawObjectWindowStatusBar(GUIType::WindowGUI &window)
{
    ImGui::Separator();

    GUI::DrawButtonTransparent(
        "Auto-hide when no object is selected",
        window.autoHideIfNotSelected ? IconGUI::WIN_OBJECT_AUTOHIDE_OFF : IconGUI::WIN_OBJECT_AUTOHIDE_ON,
        GUIType::Sizes::ICONS_OBJECTS_ALLOWED,
        window.autoHideIfNotSelected,
        [&] {
            window.autoHideIfNotSelected = !window.autoHideIfNotSelected;
        }
    );
}

float GUIManager::GetObjectStatusBarHeight()
{
    float height = 0.0f;

    height += ImGui::GetStyle().ItemSpacing.y;
    height += ImGui::GetFrameHeight();
    height += ImGui::GetStyle().FramePadding.y * 2.0f;
    height += ImGui::GetStyle().ItemSpacing.y;
    height += ImGui::GetStyle().SeparatorTextPadding.y;

    return height;
}

void GUIManager::UpdateImGuiDocking()
{
    static bool opt_fullscreen_persistant = true;
    bool opt_fullscreen = opt_fullscreen_persistant;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

    const float toolbar_height = Config::get()->ENABLE_IMGUI_TOOLBAR ? Config::get()->TOOL_BAR_HEIGHT : 0;
    const float status_bar_height = Config::get()->ENABLE_IMGUI_STATUSBAR ? Config::get()->STATUS_BAR_HEIGHT : 0;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen) {
        ImGuiViewport* viewport = ImGui::GetMainViewport();

        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + toolbar_height));
        ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - toolbar_height - status_bar_height));

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

    ImGuiID dockSpaceId = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), dockspace_flags);
}

bool GUIManager::isWindowOpen(GUIType::Window window) const
{
    for (auto& w : windows) {
        if (w.window == window) {
            return w.isOpen;
        }
    }

    return false;
}

void GUIManager::DrawWinKeyboardMouse()
{
    auto windowStatus = getWindowStatus(GUIType::KEYBOARD_MOUSE);
    if (!windowStatus->isOpen) return;

    auto input = Components::get()->Input();

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
    ImGui::Text(("Game Controller Enabled: " + std::to_string(input->isGameControllerAvailable())).c_str());
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

void GUIManager::setObjectsViewerMode(GUIType::ViewerObjectsMode value)
{
    LOG_MESSAGE("[GUIManager] Change viewer mode to %d", (int) value);
    viewerMode = value;
}

void GUIManager::setLayoutToDefault(Config::ImGUIConfigs currentConfig)
{
    std::vector<GUIType::LayoutWindowConfig> selectedConfig;

    switch (currentConfig) {
        case Config::ImGUIConfigs::DEFAULT:
            LOG_MESSAGE("[GUIManager] Reset current layout default to defaults windows...");
            selectedConfig = defaultLayoutWindowsConfig;
            break;
        case Config::ImGUIConfigs::CODING:
            LOG_MESSAGE("[GUIManager] Reset current layout devs to default windows...");
            selectedConfig = devsLayoutWindowsConfig;
            break;
        case Config::ImGUIConfigs::DESIGN:
            LOG_MESSAGE("[GUIManager] Reset current layout design to defaults windows...");
            selectedConfig = designLayoutWindowsConfig;
            break;
        default:
            LOG_ERROR("[GUIManager] Invalid config type!");
            return;
    }

    for (auto &w: windows) {
        w.isOpen = false;
        for (const auto &c : selectedConfig) {
            if (w.window == c.window && c.visible) {
                w.isOpen = true;
            }
        }
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

void GUIManager::DrawWinDepthLightsMap()
{
    auto windowStatus = getWindowStatus(GUIType::DEPTH_LIGHTS_MAPS);
    if (!windowStatus->isOpen) return;

    auto title = std::string("Lights Depth Maps Viewer: ");
    if (ImGui::Begin(title.c_str(), &windowStatus->isOpen, ImGuiWindowFlags_NoDocking)) {

        auto render = Components::get()->Render();
        auto shaderShadowPassDebugLight = render->getShaders()->shaderShadowPassDebugLight;
        auto lights = render->getShaders()->shaderOGLRender->getShadowMappingSpotLights();

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
            for (const auto &l: lights) {
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
        if (splashAlpha <= 0.0f) {
            Config::get()->ENABLE_SPLASH = false;
            return;
        }
    }

    ImVec2 size((float) splashImage->width(), (float) splashImage->height());
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 tamañoPantalla = io.DisplaySize;

    ImVec2 positionCentered;
    positionCentered.x = (tamañoPantalla.x - size.x) * 0.5f;
    positionCentered.y = (tamañoPantalla.y - size.y) * 0.5f;

    ImGui::SetNextWindowPos(positionCentered);
    ImGui::SetNextWindowSize(size);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);              // ← Borde = 0
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));  // Fondo transparente
    ImGui::Begin("##Splash", nullptr,
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoInputs |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollWithMouse
    );
    ImGui::Image(splashImage->getOGLImTexture(), size, ImVec2(0, 0), ImVec2(1, 1), ImVec4(1.0f, 1.0f, 1.0f, splashAlpha));
    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
}

GUIType::WindowGUI* GUIManager::getWindowStatus(GUIType::Window window)
{
    for (auto& status : windows) {
        if (status.window == window) {
            return &status;
        }
    }
    return nullptr;
}

bool GUIManager::isEditableFileAlreadyOpen(const std::string &uniqueId) const
{
    for (const auto &f : openFiles) {
        if (uniqueId == f->getUniqueId()) {
            return true;
        }
    }

    return false;
}

void GUIManager::LoadDocumentation()
{
    LOG_MESSAGE("[GUI] Reading documentation structure...");
    auto contentFile = Tools::ReadFile(Config::get()->CONFIG_FOLDER + Config::get()->DOCUMENTATION_CONFIG);

    documentationTree = cJSON_Parse(contentFile);
}

void GUIManager::OpenEditableFile(EditableOpenBaseResource *openFile)
{
    openFiles.push_back(openFile);
}

void GUIManager::CloseEditableFile(EditableOpenBaseResource *openFile) const
{
    for (auto &f : openFiles) {
        if (f->getUniqueId() == openFile->getUniqueId()) {
            f->setRemoved(true);
        }
    }
}

void GUIManager::ResetIndexCodeEditor()
{
    indexCodeEditorTab = -1;
}

void GUIManager::setIndexCodeEditorTab(const std::string &uniqueId)
{
    int i = 0;
    for (auto &o : openFiles) {
        if (uniqueId == o->getUniqueId()) {
            indexCodeEditorTab = i;
            return;
        }
        i++;
    }

    indexCodeEditorTab = 0;
}
