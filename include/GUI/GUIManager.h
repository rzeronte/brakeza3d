
#ifndef SDL2_3D_ENGINE_GUI_ENGINE_H
#define SDL2_3D_ENGINE_GUI_ENGINE_H

#include <string>
#include <vector>

#include "../Misc/TextureAtlas.h"
#include "AddOns/GUIConsole.h"
#include "../Misc/TexturePackage.h"
#include "AddOns/GUIAddonObjects3D.h"
#include "AddOns/GUIAddonObject3DProperties.h"
#include "AddOns/GUIAddonProjectSetup.h"
#include "AddOns/GUIAddonMenu.h"
#include "AddOns/GUIAddonToolbar.h"

class Object3DGUI;
class ScriptLuaGUI;
class Mesh3DGUI;
class ShadersGUI;
class FileSystemGUI;
class IconsGUI;

class GUIManager
{
    std::vector<GUIType::WindowData> windows;

    int selectedObjectIndex = -1;
    bool showEditShaderWindow = false;
    bool showEditScriptWindow = false;
    bool showBoneMappingsEditorWindow = false;
    bool showDebugIconsWindow = false;

    float splashAlpha = 1.0f;

    std::vector<Object3D *> &gameObjects;

    ScriptEditableManager scriptEditableManager;
    ShaderEditableManager shaderEditableManager;

    GuiAddonConsole *widgetConsole;
    GUIAddonObjects3D *widgetObjects3D;
    GUIAddonObject3DProperties *widgetObjectProperties;
    GUIAddonProjectSetup *widgetProjectSettings;
    GUIAddonMenu *menu;
    GUIAddonToolbar *toolbar;

    TexturePackage imagesFolder;

    std::string selected_file;
    std::string currentVariableToAddName;
    std::string currentVariableToCreateCustomShader;

    GUIType::FolderBrowserCache browserScenes;
    GUIType::FolderBrowserCache browserProjects;
    GUIType::FolderBrowserCache browserShaders;
    GUIType::FolderBrowserCache browserScripts;

    Color lineSelectorObjectColor = Color::green();

    TextureAtlas *textureAtlas;
    Image *splashImage = new Image(Config::get()->IMAGES_FOLDER + Config::get()->SPLASH_FILENAME);

    void WindowImages();
    void WindowLightsDepthMapsViewer();
    void DrawRegisteredWindows();
    void WindowKeyboardMouseSetup();
    void DrawSplash();

    GUIType::WindowData *GetWindowStatus(GUIType::Window window);

public:
    explicit GUIManager(std::vector<Object3D *> &gameObjects);
    virtual ~GUIManager() = default;

    void RegisterWindows();

    void WindowDebugIcons();

    void WindowShaderFiles();
    void WindowScriptFiles();
    void WindowSceneFiles();
    void WindowProjectFiles();
    void WindowSelectedObjectProperties();
    void WindowProjectSettings();
    void WindowGlobalVars();
    void WindowSelectedObjectScripts();
    void WindowSelectedObjectVariables();
    void WindowSelectedObjectShaders();
    void WindowSceneObjects();
    void WindowLogging();
    void setSelectedObjectIndex(int selectedObjectIndex);
    void setSelectedObject(const Object3D *s);
    void OpenBoneInfoDialog();
    virtual void DrawGUI();
    [[nodiscard]] GuiAddonConsole *getConsole() const;
    [[nodiscard]] bool isLightDepthMapsViewerWindowOpen();
    [[nodiscard]] TextureAtlas * getTextureAtlas() const;
    [[nodiscard]] GUIType::FolderBrowserCache getBrowserScripts() const;
    [[nodiscard]] GUIType::FolderBrowserCache getBrowserScenes() const;
    [[nodiscard]] GUIType::FolderBrowserCache getBrowserProjects() const;
    [[nodiscard]] GUIType::FolderBrowserCache getBrowserShaders() const;
    static void ShowDeletePopup(const char* title, const char *message, const std::function<void()>& onConfirm);
    static void SetNextWindowSize(int w, int h);
    static void UpdateImGuiDocking();
    static void WelcomeMessage();

    friend class Object3DGUI;
    friend class ScriptLuaGUI;
    friend class Mesh3DGUI;
    friend class ShadersGUI;
    friend class FileSystemGUI;
    friend class IconsGUI;
};

#endif //SDL2_3D_ENGINE_GUI_ENGINE_H
