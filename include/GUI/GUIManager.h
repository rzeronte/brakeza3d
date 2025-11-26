
#ifndef SDL2_3D_ENGINE_GUI_ENGINE_H
#define SDL2_3D_ENGINE_GUI_ENGINE_H

#include <string>
#include <vector>
#include "AddOns/GUIConsole.h"
#include "../Misc/TexturePackage.h"
#include "AddOns/GUIAddonObjects3D.h"
#include "AddOns/GUIAddonObject3DProperties.h"
#include "AddOns/GUIAddonProjectSetup.h"
#include "AddOns/GUIAddonMenu.h"
#include "AddOns/GUIAddonToolbar.h"

struct GUIConstants {
    static constexpr ImVec2 ICON_SIZE_SMALL = ImVec2(14, 14);
    static constexpr ImVec2 ICON_SIZE_MEDIUM = ImVec2(16, 16);
    static constexpr ImVec2 ICON_SIZE_LARGE = ImVec2(96, 96);
    static constexpr float WINDOW_ALPHA = 0.9f;
    static constexpr int DEFAULT_WINDOW_WIDTH = 600;
    static constexpr int DEFAULT_WINDOW_HEIGHT = 600;
};

class Object3DGUI;
class ScriptLuaGUI;
class Mesh3DGUI;
class ShadersGUI;
class FileSystemGUI;

class GUIManager
{
    int selectedObjectIndex = -1;
    bool showAboutWindow = false;
    bool showEditShaderWindow = false;
    bool showEditScriptWindow = false;
    bool showBoneMappingsEditorWindow = false;
    bool showLightsDepthMapsViewerWindow = false;

    std::vector<Object3D *> &gameObjects;

    ScriptEditableManager scriptEditableManager;
    ShaderEditableManager shaderEditableManager;

    GuiAddonConsole *widgetConsole;
    GUIAddonObjects3D *widgetObjects3D;
    GUIAddonObject3DProperties *widgetObject3DProperties;
    GUIAddonProjectSetup *widgetProjectSettings;
    GUIAddonMenu *widgetMenu;
    GUIAddonToolbar *widgetToolbar;

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
public:
    virtual ~GUIManager() = default;
    explicit GUIManager(std::vector<Object3D *> &gameObjects);

    void setSelectedObjectIndex(int selectedObjectIndex);
    void setSelectedObject(const Object3D *s);
    void openBoneInfoDialog();

    virtual void DrawGUI(float timedelta, bool &finish);
    TexturePackage &getImGuiTextures();
    [[nodiscard]] GuiAddonConsole *getConsole() const;
    [[nodiscard]] bool isShowLightsDepthMapsViewerWindow() const;
    static void ShowDeletePopup(const char* title, const std::function<void()>& onConfirm);
    static void SetNextWindowSize(int w, int h);
    static void UpdateImGuiDocking();

    friend class Object3DGUI;
    friend class ScriptLuaGUI;
    friend class Mesh3DGUI;
    friend class ShadersGUI;
    friend class FileSystemGUI;
private:
    void DrawWidgets();
    void DrawGUIPlugins(bool &finish);
    void DrawImages();
    void DrawLightsDepthMapsViewerWindow();
    static void DrawKeyboardMouseSettings();
    static void RenderFPS();
    static void DrawSplash();
};

#endif //SDL2_3D_ENGINE_GUI_ENGINE_H
