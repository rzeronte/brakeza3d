
#ifndef SDL2_3D_ENGINE_GUI_ENGINE_H
#define SDL2_3D_ENGINE_GUI_ENGINE_H

#include <string>
#include <vector>

#include "../../src/GUI/ShaderNodeEditorManager.h"
#include "../Loaders/SceneChecker.h"
#include "../Misc/TextureAtlas.h"
#include "../Misc/TexturePackage.h"
#include "AddOns/GUIConsole.h"
#include "Editable/EditableOpenBaseResource.h"

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

    bool showBoneMappingsEditorWindow = false;

    float splashAlpha = 1.0f;

    std::vector<GUIType::ViewerObjectType> visibleTypeObjects;
    GUIType::ViewerObjectsMode viewerMode = GUIType::ViewerObjectsMode::TREE;

    std::vector<GUIType::MenuItem> menus;

    GuiAddonConsole *widgetConsole;

    std::vector<EditableOpenBaseResource *> openFiles;

    cJSON *documentationTree = nullptr;
    TextEditor documentationEditor;

    TexturePackage browserImagesTextures;

    GUIType::BrowserCache browserScenes;
    GUIType::BrowserCache browserProjects;
    GUIType::BrowserCache browserShaders;
    GUIType::BrowserCache browserScripts;
    GUIType::BrowserCache browserImages;

    SceneChecker checker;

    Color selectionColor = Color::green();

    int indexCodeEditorTab = 0;

    TextureAtlas *textureAtlas;
    Image *splashImage = nullptr;

    void DrawWinDepthLightsMap();
    void DrawRegisteredWindows();
    void DrawWinKeyboardMouse();
    void DrawSplashWindow();

public:

    explicit GUIManager();

    void LoadBrowserFolders();

    virtual ~GUIManager() = default;

    bool isWindowOpen(GUIType::Window w) const;
    bool isEditableFileAlreadyOpen(const std::string &label) const;
    void LoadDocumentation();
    void OnStart();
    void RegisterWindows();
    void RegisterDefaultLayoutWindows();
    void RegisterAllowedItemsForViewer();
    void RegisterMenu();
    void OpenBoneInfoDialog();
    void setObjectsViewerMode(GUIType::ViewerObjectsMode value);
    void setLayoutToDefault(Config::ImGUIConfigs config);
    void setIndexCodeEditorTab(const std::string &label);
    void CloseRemovedEditableOpenFiles();
    void DrawWinEditableOpenResources();
    void OpenEditableFile(EditableOpenBaseResource *openFile);
    void CloseEditableFile(EditableOpenBaseResource *openFile) const;
    void ResetIndexCodeEditor();

    [[nodiscard]] TextureAtlas * getTextureAtlas() const                                { return textureAtlas; }
    [[nodiscard]] GUIType::BrowserCache getBrowserScripts() const                       { return browserScripts; }
    [[nodiscard]] GuiAddonConsole *getConsole() const                                   { return widgetConsole; }
    [[nodiscard]] GUIType::BrowserCache getBrowserScenes() const                        { return browserScenes; }
    [[nodiscard]] GUIType::BrowserCache getBrowserProjects() const                      { return browserProjects; }
    [[nodiscard]] GUIType::BrowserCache getBrowserShaders() const                       { return browserShaders; }
    [[nodiscard]] GUIType::ViewerObjectsMode getObjectsViewerMode() const               { return viewerMode; }
    [[nodiscard]] std::vector<EditableOpenBaseResource *> getEditableOpenFiles() const  { return openFiles;}
    [[nodiscard]] int getIndexCodeEditorTab() const                                     { return indexCodeEditorTab; }
    [[nodiscard]] SceneChecker &getSceneChecker()                                       { return checker; }
    [[nodiscard]] std::vector<GUIType::ViewerObjectType> &getVisibleObjectTypes()       { return visibleTypeObjects; }

    GUIType::WindowData *getWindowStatus(GUIType::Window window);

    virtual void DrawGUI();
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
