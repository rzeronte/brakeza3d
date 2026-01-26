// ============= StatusBarGUI.h =============
#pragma once
#include <imgui.h>
#include "../GUI.h"
#include "../AddOns/GUIAddonResourceHub.h"

class StatusBarGUI {
public:
    static void Render(GUIAddonResourceHub* resourceHub);

private:
    // Window setup
    static void SetupWindow(ImGuiViewport* viewport);
    static void ApplyStyle();
    static void RestoreStyle();
    static ImGuiWindowFlags GetWindowFlags();

    // Content rendering
    static void RenderContent(GUIAddonResourceHub* resourceHub);
    static void DrawSeparator();

    // Individual items
    static void DrawVersionInfo();
    static void DrawFPSCounter();
    static void DrawSessionStatus(GUIAddonResourceHub* resourceHub);
    static void DrawObjectCount();
    static void DrawLightsStatus();
    static void DrawScriptsStatus();
    static void DrawProjectStatus();
    static void DrawSceneStatus();

    // Helpers
    static void DrawIconWithText(GUIType::Sheet icon, const char* text);
    static void DrawIconWithFormattedText(GUIType::Sheet icon, const char* format, ...);

    static void DrawGradientLine(ImGuiViewport *viewport, float yPosition, float lineHeight);
    static float CalculateItemWidth(GUIType::Sheet icon, const char* text);
    static void DrawRightAlignedItems(GUIAddonResourceHub* resourceHub);
};
