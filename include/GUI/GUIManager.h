
#ifndef SDL2_3D_ENGINE_GUI_ENGINE_H
#define SDL2_3D_ENGINE_GUI_ENGINE_H

#include <string>
#include <vector>
#include "../Misc/TextureAtlas.h"
#include "../Misc/TexturePackage.h"
#include "AddOns/GUIConsole.h"

class Object3DGUI;
class ScriptLuaGUI;
class Mesh3DGUI;
class ShadersGUI;
class FileSystemGUI;
class IconsGUI;
class GUIAddonObjects3D;
class GUIAddonObject3DProperties;
class Object3D;

class GUIManager
{
    std::vector<GUIType::WindowData> windows;

    std::vector<GUIType::LayoutWindowConfig> defaultLayoutWindowsConfig;
    std::vector<GUIType::LayoutWindowConfig> devsLayoutWindowsConfig;
    std::vector<GUIType::LayoutWindowConfig> designLayoutWindowsConfig;

    int selectedObjectIndex = -1;
    bool showEditShaderWindow = false;
    bool showEditScriptWindow = false;
    bool showBoneMappingsEditorWindow = false;

    float splashAlpha = 1.0f;

    std::vector<Object3D *> &gameObjects;

    GUIType::ScriptEditableManager scriptEditableManager;
    GUIType::ShaderEditableManager shaderEditableManager;

    std::vector<GUIType::AddonAllowedObjects> visibleTypeObjects;
    GUIType::AddonObjectsViewerMode viewerMode = GUIType::AddonObjectsViewerMode::TREE;

    std::vector<GUIType::MenuItem> menus;

    GuiAddonConsole *widgetConsole;

    TexturePackage imagesFolder;

    std::string selectedFile;
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
    void DrawSplashWindow();

public:

    explicit GUIManager(std::vector<Object3D *> &gameObjects);
    virtual ~GUIManager() = default;

    void setSelectedObjectIndex(int selectedObjectIndex);
    void setSelectedObject(const Object3D *s);
    void RegisterWindows();
    void RegisterDefaultLayoutWindows();
    void RegisterAllowedItemsForViewer();
    void RegisterMenu();
    void OpenBoneInfoDialog();
    void setObjectsViewerMode(GUIType::AddonObjectsViewerMode value);
    void setLayoutToDefault(Config::ImGUIConfigs config);
    virtual void DrawGUI();
    GUIType::WindowData *getWindowStatus(GUIType::Window window);
    [[nodiscard]] GuiAddonConsole *getConsole() const;
    [[nodiscard]] bool isLightDepthMapsViewerWindowOpen();
    [[nodiscard]] TextureAtlas * getTextureAtlas() const;
    [[nodiscard]] GUIType::FolderBrowserCache getBrowserScripts() const;
    [[nodiscard]] GUIType::FolderBrowserCache getBrowserScenes() const;
    [[nodiscard]] GUIType::FolderBrowserCache getBrowserProjects() const;
    [[nodiscard]] GUIType::FolderBrowserCache getBrowserShaders() const;
    [[nodiscard]] GUIType::AddonObjectsViewerMode getObjectsViewerMode() const;
    [[nodiscard]] int& selectedObjectIndexPointer();
    static void SetNextWindowSize(int w, int h);
    static void UpdateImGuiDocking();

    friend class Object3DGUI;
    friend class ScriptLuaGUI;
    friend class Mesh3DGUI;
    friend class ShadersGUI;
    friend class FileSystemGUI;
    friend class IconsGUI;
    friend class Mesh3DAnimationDrawerGUI;
    friend class GUIAddonObjects3D;
    friend class GUIAddonObject3DProperties;
    friend class GUIAddonProjectSetup;
    friend class GUIAddonMenu;
};

#endif //SDL2_3D_ENGINE_GUI_ENGINE_H
