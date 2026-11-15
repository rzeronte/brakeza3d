/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
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
    static void Helpers() ;
    static void VerticalSeparator();
    static void DrawSeparatorLine();
};

#endif //BRAKEZA3D_GUIWIDGETTOOLBAR_H
