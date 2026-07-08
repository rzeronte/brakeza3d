#ifndef BRAKEZA3D_UIMANAGERGUI_H
#define BRAKEZA3D_UIMANAGERGUI_H

#include <string>
#include <algorithm>
#include <vector>
#include <GL/glew.h>
#include "../../Render/UI/UIManager.h"

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

public:
    static void DrawWinUIManager();
    static void setSelectedWidget(const std::string& name);
};

#endif
