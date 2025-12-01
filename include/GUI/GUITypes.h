//
// Created by darkhead on 15/9/18.
//

#ifndef SDL2_3D_ENGINE_GUI_H
#define SDL2_3D_ENGINE_GUI_H

#include <string>
#include "imgui.h"

namespace GUITypes {
    enum GUIWindows {
        PROFILER,
        DEPTH_LIGTHS_MAPS,
        LOGGING,
        SCENE_OBJECTS
    };

    struct GUIWindowsStatus {
        GUIWindows window;
        std::string label;
        bool isOpen = false;
        void (*functionCallBack)();
    };

    struct GUIConstants {
        static constexpr ImVec2 ICON_SIZE_SMALL = ImVec2(14, 14);
        static constexpr ImVec2 ICON_SIZE_MEDIUM = ImVec2(16, 16);
        static constexpr ImVec2 ICON_SIZE_LARGE = ImVec2(96, 96);
        static constexpr float WINDOW_ALPHA = 0.9f;
        static constexpr int DEFAULT_WINDOW_WIDTH = 600;
        static constexpr int DEFAULT_WINDOW_HEIGHT = 600;
    };
}


#endif //SDL2_3D_ENGINE_GUI_H
