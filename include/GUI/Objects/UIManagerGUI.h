#ifndef BRAKEZA3D_UIMANAGERGUI_H
#define BRAKEZA3D_UIMANAGERGUI_H

#include <string>
#include <algorithm>
#include <vector>
#include <GL/glew.h>
#include "../../Render/UI/UIManager.h"

class GUIFilePicker;

class UIManagerGUI
{
    static int selectedWidget;
    static int selectedElement;
    static GLuint previewFBO;
    static GLuint previewColorTex;
    static GLuint previewDepthRB;
    static UIWidgetRenderData previewData;
    static float  previewZoom;
    static ImVec2 previewPan;

    static void DrawWidgetList(UIManager* ui);
    static void DrawElementList(UIWidget& w);
    static void DrawElementEditor(UIElement& el, UIWidget& w, UIManager* ui, const std::string& widgetName);
    static void DrawProgressBarColors(UIElement& el);
    static void DrawIconMappingEditor(UIElement& el);
    static void SaveWidget(UIManager* ui, const std::string& widgetName, UIWidget& w);
    static void DrawWidgetPreview(UIManager* ui, const std::string& wName);
    static void DrawElementPreviewFields(UIElement& el);

    // ── Reusable field helpers (extracted from repeated inline blocks) ────
    // All take a label with ##uid suffix for stable ImGui IDs; return true if edited.
    static bool Color4Field(const char* label, Color& c, bool alphaBar = false);
    static bool StringField(const char* label, std::string& str, float width, int bufSize = 256);
    static bool ImagePickerField(
        const char* label,
        std::string& path,
        GUIFilePicker& picker,
        const char* pickerTitle,
        float inputWidth = 260.0f);
    static void BorderEditorSection(UIElement& el);

public:
    static void DrawWinUIManager();
    static void setSelectedWidget(const std::string& name);
};

#endif
