//
// Created by darkhead on 15/9/18.
//

#ifndef SDL2_3D_ENGINE_GUI_H
#define SDL2_3D_ENGINE_GUI_H

#include <functional>
#include <string>
#include "imgui.h"

namespace GUITypes
{
    enum GUIWindowsGroups {
        OBJECT_WINDOWS,
        FILE_SYSTEM,
        PROFILING,
        SCRIPTING
    };
    enum GUIWindows {
        PROFILER,
        DEPTH_LIGHTS_MAPS,
        LOGGING,
        SCENE_OBJECTS,
        OBJECT_SHADERS,
        OBJECT_SCRIPTS,
        OBJECT_VARIABLES,
        GLOBAL_VARIABLES,
        KEYBOARD_MOUSE,
        IMAGES,
        PROJECT_SETTINGS,
        OBJECT_PROPERTIES,
        FILES_SCENES,
        FILES_PROJECTS,
        FILES_SHADERS,
        FILES_SCRIPTS
    };

    struct GUIWindowsStatus {
        GUIWindows window;
        std::string label;
        bool isOpen = false;
        std::function<void()> functionCallBack;
    };

    struct GUIMenuItem {
        std::string label;
        std::function<void()> functionCallBack;
    };

    struct GUIConstants {
        static constexpr ImVec2 ICON_SIZE_SMALL = ImVec2(14, 14);
        static constexpr ImVec2 ICON_SIZE_MEDIUM = ImVec2(16, 16);
        static constexpr ImVec2 ICON_SIZE_LARGE = ImVec2(96, 96);
        static constexpr float WINDOW_ALPHA = 0.9f;
        static constexpr int DEFAULT_WINDOW_WIDTH = 600;
        static constexpr int DEFAULT_WINDOW_HEIGHT = 600;
        static constexpr int DRAG_IMGUI_SENSIBILITY_LOW = 0.001f;
        static constexpr int DRAG_IMGUI_SENSIBILITY_MEDIUM = 0.01f;
        static constexpr int DRAG_IMGUI_SENSIBILITY_TOP = 1.0f;
        static constexpr int DRAG_FOV_MIN = 1.0f;
        static constexpr int DRAG_FOV_MAX = 160.0f;
        static constexpr int DRAG_FRAMERATE_MIN = 1.0f;
        static constexpr int DRAG_FRAMERATE_MAX = 1024.0f;
        static constexpr int DRAG_FAR_PLANE_MIN = 1.0f;
        static constexpr int DRAG_FAR_PLANE_MAX = 1024.0f;
        static constexpr int DRAG_UNIT_MIN = 0.f;
        static constexpr int DRAG_UNIT_MAX = 1.f;
        static constexpr int DRAG_VOLUME_MIN = 0.f;
        static constexpr int DRAG_VOLUME_MAX = 1.f;
    };
}


#endif //SDL2_3D_ENGINE_GUI_H
