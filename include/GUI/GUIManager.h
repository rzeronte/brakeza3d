
#ifndef SDL2_3D_ENGINE_GUI_ENGINE_H
#define SDL2_3D_ENGINE_GUI_ENGINE_H

#include <string>
#include <vector>

#include "GUITypes.h"
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

class GUIManager
{
    std::vector<GUITypes::GUIWindowData> windows;

    int selectedObjectIndex = -1;
    bool showEditShaderWindow = false;
    bool showEditScriptWindow = false;
    bool showBoneMappingsEditorWindow = false;
    float splashAlpha = 1.0f;

    std::vector<Object3D *> &gameObjects;

    ScriptEditableManager scriptEditableManager;
    ShaderEditableManager shaderEditableManager;

    GuiAddonConsole *widgetConsole = new GuiAddonConsole(ComponentsManager::get()->getComponentScripting()->getLua());
    GUIAddonObjects3D *widgetObjects3D;
    GUIAddonObject3DProperties *widgetObjectProperties;
    GUIAddonProjectSetup *widgetProjectSettings;
    GUIAddonMenu *menu;
    GUIAddonToolbar *toolbar;

    TexturePackage imagesFolder;

    std::string selected_file;
    std::string currentVariableToAddName;
    std::string currentVariableToCreateCustomShader;

public:
    [[nodiscard]] GUITypes::FolderBrowserCache getBrowserScripts() const;
    [[nodiscard]] GUITypes::FolderBrowserCache getBrowserScenes() const;
    [[nodiscard]] GUITypes::FolderBrowserCache getBrowserProjects() const;
    [[nodiscard]] GUITypes::FolderBrowserCache getBrowserShaders() const;

private:
    GUITypes::FolderBrowserCache browserScenes = GUI::CreateBrowserCache(Config::get()->SCENES_FOLDER, Config::get()->SCENES_EXT);
    GUITypes::FolderBrowserCache browserProjects = GUI::CreateBrowserCache(Config::get()->CUSTOM_SHADERS_FOLDER, Config::get()->PROJECTS_EXT);
    GUITypes::FolderBrowserCache browserShaders = GUI::CreateBrowserCache(Config::get()->PROJECTS_FOLDER, Config::get()->SHADERS_EXT);
    GUITypes::FolderBrowserCache browserScripts = GUI::CreateBrowserCache(Config::get()->SCRIPTS_FOLDER, Config::get()->SCRIPTS_EXT);

    Color lineSelectorObjectColor = Color::green();

    TextureAtlas *textureAtlas;
    Image *splashImage = new Image(Config::get()->IMAGES_FOLDER + Config::get()->SPLASH_FILENAME);

    void WindowImages();
    void WindowLightsDepthMapsViewer();
    void DrawRegisteredWindows() const;
    void WindowKeyboardMouseSetup();
    void DrawSplash();

    GUITypes::GUIWindowData *GetWindowStatus(GUITypes::GUIWindow window);

public:
    explicit GUIManager(std::vector<Object3D *> &gameObjects);
    virtual ~GUIManager() = default;

    void RegisterWindows();
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
    static void ShowDeletePopup(const char* title, const char *message, const std::function<void()>& onConfirm);
    static void SetNextWindowSize(int w, int h);
    static void UpdateImGuiDocking();
    static void WelcomeMessage();

    friend class Object3DGUI;
    friend class ScriptLuaGUI;
    friend class Mesh3DGUI;
    friend class ShadersGUI;
    friend class FileSystemGUI;
};

#endif //SDL2_3D_ENGINE_GUI_ENGINE_H
