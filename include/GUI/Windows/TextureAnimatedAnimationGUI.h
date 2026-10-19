//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_TEXTUREANIMATIONGUI_H
#define BRAKEZA3D_TEXTUREANIMATIONGUI_H

#include "../../Render/TextureAnimated.h"
#include "../Widgets/GUIFields.h"

class TextureAnimatedDrawerGUI
{
public:
    static void DrawPropertiesGUI(TextureAnimated *o)
    {
        if (ImGui::TreeNode("Current Animation Setup")) {
            static int width = o->currentSpriteWidth;
            static int height = o->currentspriteHeight;
            static int numFrames = o->numberFramesToLoad;
            static int fps = o->fps;

            constexpr int range_min_int = 1;
            constexpr int range_max_int = 1280;

            {
                GUIFields::Section setup("ANIMATION SETUP");
                if (setup) {
                    GUIFields::Int("Sprite Width", &width, 1.0f, range_min_int, range_max_int);
                    GUIFields::Int("Sprite Height", &height, 1.0f, range_min_int, range_max_int);
                    GUIFields::Int("Number frames", &numFrames, 1.0f, range_min_int, range_max_int);
                    GUIFields::Int("FPS", &fps, 1.0f, range_min_int, range_max_int);
                }
            }

            if (ImGui::Button("Apply")) {
                o->Apply(o->getBaseFilename(), width, height, numFrames, fps);
                o->UpdateStep();
            }
            ImGui::TreePop();
        }
    }
};

#endif //BRAKEZA3D_TEXTUREANIMATIONGUI_H