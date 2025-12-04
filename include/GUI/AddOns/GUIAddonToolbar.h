#ifndef BRAKEZA3D_GUIWIDGETTOOLBAR_H
#define BRAKEZA3D_GUIWIDGETTOOLBAR_H

#include "imgui.h"
#include "../GUITypes.h"

struct GUIAddonToolbar
{
    explicit GUIAddonToolbar() = default;

    static void Draw();
    static void LayoutIcons();
    static void TransformationsToolsIcons();
    static void StepSimulationOptionsIcons();
    static void MouseOptionsIcons() ;
    static void LightsOptions() ;
    static void LUAStatusIcons() ;
    static void RenderTriangleModes() ;
    static void VerticalSeparator();
};

#endif //BRAKEZA3D_GUIWIDGETTOOLBAR_H
