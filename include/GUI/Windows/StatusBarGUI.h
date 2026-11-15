/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
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
    static void DrawCullingStats();
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
