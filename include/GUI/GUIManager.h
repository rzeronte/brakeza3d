
#ifndef SDL2_3D_ENGINE_GUI_ENGINE_H
#define SDL2_3D_ENGINE_GUI_ENGINE_H

#include <string>
#include <vector>

#include "GUITypes.h"
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
    std::vector<GUITypes::GUIWindowsStatus> windows;

    int selectedObjectIndex = -1;
    bool showEditShaderWindow = false;
    bool showEditScriptWindow = false;
    bool showBoneMappingsEditorWindow = false;

    std::vector<Object3D *> &gameObjects;

    ScriptEditableManager scriptEditableManager;
    ShaderEditableManager shaderEditableManager;

    GuiAddonConsole *widgetConsole;
    GUIAddonObjects3D *widgetObjects3D;
    GUIAddonObject3DProperties *widgetObjectProperties;
    GUIAddonProjectSetup *widgetProjectSettings;
    GUIAddonMenu *menu;
    GUIAddonToolbar *toolbar;

    TexturePackage icons;
    TexturePackage imagesFolder;

    std::string selected_file;
    std::string currentVariableToAddName;
    std::string currentVariableToCreateCustomShader;

    std::string currentScriptsFolderWidget;
    std::vector<std::string> currentScriptsFolderFiles;
    std::vector<std::string> currentScriptsFolders;

    std::string currentScenesFolderWidget;
    std::vector<std::string> currentScenesFolderFiles;
    std::vector<std::string> currentScenesFolders;

    std::string currentProjectsFolderWidget;
    std::vector<std::string> currentProjectsFolderFiles;
    std::vector<std::string> currentProjectsFolders;

    std::string currentShadersFolderWidget;
    std::vector<std::string> currentShadersFolderFiles;
    std::vector<std::string> currentShadersFolders;

    Color lineSelectorObjectColor = Color::green();

    void DrawImages();
    void DrawLightsDepthMapsViewerWindow();
    void DrawRegisteredWindows() const;
    void DrawKeyboardMouseSettings();
    static void DrawSplash();

    GUITypes::GUIWindowsStatus *GetWindowStatus(GUITypes::GUIWindows window);

public:
    explicit GUIManager(std::vector<Object3D *> &gameObjects);
    virtual ~GUIManager() = default;
    void RegisterWindows();
    void DrawFilesShaders();
    void DrawFilesScriptWindow();
    void DrawFilesScenesWindow();
    void DrawFilesProjectWindow();
    void DrawSelectedObjectPropertiesWindow();
    void DrawProjectSettingsWindow();
    void DrawGlobalVariablesWindow();
    void DrawSelectedObjectScriptsWindow();
    void DrawSelectedObjectVariablesWindow();
    void DrawSelectedObjectShadersWindow();
    void DrawSceneObjectsWindow();
    void DrawLoggingWindow();
    void setSelectedObjectIndex(int selectedObjectIndex);
    void setSelectedObject(const Object3D *s);
    void OpenBoneInfoDialog();
    virtual void DrawGUI();
    TexturePackage &getImGuiTextures();
    [[nodiscard]] GuiAddonConsole *getConsole() const;
    [[nodiscard]] bool isLightDepthMapsViewerWindowOpen();
    static void ShowDeletePopup(const char* title, const char *message, const std::function<void()>& onConfirm);
    static void SetNextWindowSize(int w, int h);
    static void UpdateImGuiDocking();

    friend class Object3DGUI;
    friend class ScriptLuaGUI;
    friend class Mesh3DGUI;
    friend class ShadersGUI;
    friend class FileSystemGUI;
};

#endif //SDL2_3D_ENGINE_GUI_ENGINE_H
