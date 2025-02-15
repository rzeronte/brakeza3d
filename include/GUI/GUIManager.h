
#ifndef SDL2_3D_ENGINE_GUI_ENGINE_H
#define SDL2_3D_ENGINE_GUI_ENGINE_H

#include <string>
#include <vector>
#include "../Objects/Object3D.h"
#include "imgui.h"
#include "../ComponentsManager.h"
#include "GUIConsole.h"
#include "../Misc/TexturePackage.h"
#include "../Render/Logging.h"
#include "../FXEffect/FXOutliner.h"
#include "../FXEffect/FXBlink.h"
#include "GUIWidgetObjects3D.h"
#include "GUIWidgetObject3DProperties.h"
#include "GUIWidgetProjectSettings.h"
#include "GUIWidgetMenu.h"
#include "GUIWidgetToolbar.h"

class GUIManager {
private:
    std::vector<Object3D *> &gameObjects;

    int selectedObjectIndex = -1;
    ScriptEditableManager scriptEditableManager;
    ShaderEditableManager shaderEditableManager;

    ImGuiConsoleApp *widgetConsole;
    GUIWidgetObjects3D *widgetObjects3D;
    GUIWidgetObject3DProperties *widgetObject3DProperties;
    GUIWidgetProjectSettings *widgetProjectSettings;
    GUIWidgetMenu *widgetMenu;
    GUIWidgetToolbar *widgetToolbar;

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

    bool showAboutWindow = false;
    bool showEditShaderWindow = false;
    bool showEditScriptWindow = false;

    const char *availableFX[4] = {"Edge", "Blink", "ShockWave", "Tint"};
public:

    void loadImagesFolder();

    explicit GUIManager(std::vector<Object3D *> &gameObjects);

    void LoadUIIcons();

    void LoadScriptDialog(const std::string& filename);

    void drawEditScriptWindow();

    void updateFolderFiles();

    void drawBrowserFolders(
        const std::string& folder,
        const std::string& baseFolder,
        std::string& destiny,
        std::vector<std::string> &folders,
        std::vector<std::string> &files,
        const std::string& extension
    );

    void LoadShaderDialog(std::string &folder, std::string &file);

    void drawSelectedObjectShaders();
    void drawSelectedObjectScripts();
    void drawEditShaderWindow();
    void drawScriptsLuaFolderFiles(const std::string& folder);
    void drawScriptVariables();
    void drawShaderVariables();
    void drawCustomShadersFolder(std::string folder);
    void drawWidgets();
    void drawProjectsFiles(const std::string& folder);
    void drawScenesFolder(const std::string& folder);
    void drawObjectVariables();
    void drawGlobalVariables();
    void drawKeyboardMouseSettings();
    void drawImages();

    virtual void draw(float timedelta, bool &finish);

    ImGuiConsoleApp *getConsole();

    TexturePackage *getImGuiTextures();

    void setSelectedObjectIndex(int selectedObjectIndex);

    void setSelectedObject(Object3D *s);

    static void ShowDeletePopup(const char* title, const std::function<void()>& onConfirm);

    static void setNextWindowSize(int w, int h);
};

#endif //SDL2_3D_ENGINE_GUI_ENGINE_H
