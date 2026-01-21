#ifndef BRAKEZA3D_GUIWIDGETTOOLBAR_H
#define BRAKEZA3D_GUIWIDGETTOOLBAR_H

struct GUIAddonToolbar
{
    explicit GUIAddonToolbar() = default;

    static void DrawBaseLine();

    static void Draw();
    static void LayoutIcons();
    static void TransformationsToolsIcons();
    static void StepSimulationOptionsIcons();
    static void MouseOptionsIcons() ;
    static void LightsOptions() ;
    static void LUAStatusIcons() ;
    static void RenderTriangleModes() ;
    static void Helpers() ;
    static void VerticalSeparator();
    static void DrawSeparatorLine();
};

#endif //BRAKEZA3D_GUIWIDGETTOOLBAR_H
