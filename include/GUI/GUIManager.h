
#ifndef SDL2_3D_ENGINE_GUI_ENGINE_H
#define SDL2_3D_ENGINE_GUI_ENGINE_H

#include <string>
#include <vector>
#include "../3D/Object3D.h"
#include "GUIConsole.h"
#include "../Misc/TexturePackage.h"
#include "GUIAddonObjects3D.h"
#include "GUIAddonObject3DProperties.h"
#include "GUIAddonProjectSettings.h"
#include "GUIAddonMenu.h"
#include "GUIAddonToolbar.h"

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

    ImGuiConsoleApp *widgetConsole;
    GUIAddonObjects3D *widgetObjects3D;
    GUIAddonObject3DProperties *widgetObject3DProperties;
    GUIAddonProjectSettings *widgetProjectSettings;
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

    void loadImagesFolder();
    void LoadScriptDialog(const std::string& filename);
    void DrawEditScriptWindow();
    void DrawLightsDepthMapsViewerWindow();
    void updateFolderFiles();
    void LoadShaderDialog(const std::string &folder, std::string &file);
    void drawSelectedObjectShaders();
    void drawSelectedObjectScripts();
    void DrawEditShaderWindow();
    void drawScriptsLuaFolderFiles(const std::string& folder);
    void drawScriptVariables();
    void drawShaderVariables();
    void drawCustomShadersFolder(std::string folder);
    void DrawWidgets();
    void DrawObjectSelectedGuizmoOperation();
    void updateImGuiDocking();
    void DrawGUIPlugins(bool &finish);
    void drawProjectsFiles(const std::string& folder);
    void drawScenesFolder(const std::string& folder);
    void DrawObjectVariables() const;

    void drawKeyboardMouseSettings();
    void drawImages();
    void setSelectedObjectIndex(int selectedObjectIndex);
    void setSelectedObject(Object3D *s);
    void DrawEditBonesMappingWindow();
    void openBoneInfoDialog();
    void openLightsDepthMapsViewerDialog();
    void DrawSplash();
    void drawBrowserFolders(
        const std::string& folder,
        const std::string& baseFolder,
        std::string& destiny,
        std::vector<std::string> &folders,
        std::vector<std::string> &files,
        const std::string& extension
    );
    virtual void draw(float timedelta, bool &finish);
    ImGuiConsoleApp *getConsole();
    TexturePackage *getImGuiTextures();
    [[nodiscard]] bool isShowLightsDepthMapsViewerWindow() const;
    static void drawGlobalVariables();
    static void ShowDeletePopup(const char* title, const std::function<void()>& onConfirm);
    static void LoadIcons(TexturePackage &icon);
    static void setNextWindowSize(int w, int h);
    static void RenderFPS();
};

#endif //SDL2_3D_ENGINE_GUI_ENGINE_H
