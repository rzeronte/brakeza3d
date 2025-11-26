//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_IMAGE2DGUI_H
#define BRAKEZA3D_IMAGE2DGUI_H

#include "../../2D/Image2D.h"
#include "../../Misc/Logging.h"

class Image2DGUI
{
public:
    static void drawImGuiProperties(Image2D *o)
    {
        if (ImGui::CollapsingHeader("Image2D")) {

            const int range_min_int = 1;
            const int range_max_int = 1280;

            if (ImGui::TreeNode("Screen Position")) {
                if (ImGui::DragScalar("Offset X", ImGuiDataType_S32, &o->x,1.f, &range_min_int, &range_max_int, "%d", 1.0f)) {
                    o->updatePosition(o->x, o->y);
                }
                if (ImGui::DragScalar("Offset Y", ImGuiDataType_S32, &o->y,1.f, &range_min_int, &range_max_int, "%d", 1.0f)) {
                    o->updatePosition(o->x, o->y);
                }
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Image")) {
                if (o->image->isLoaded()) {
                    ImGui::Image(o->image->getOGLImTexture(),ImVec2(32, 32));
                } else {
                    ImGui::Text("No image selected. Drag a texture here!");
                }

                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("IMAGE_ITEM")) {
                        Logging::Message("Dropping image (%s) in emitter %s", payload->Data, o->getLabel().c_str());
                        IM_ASSERT(payload->DataSize == sizeof(int));
                        auto selection = static_cast<char *>(payload->Data);
                        auto fullPath = BrakezaSetup::get()->IMAGES_FOLDER + selection;
                        if (o->image == nullptr) {
                            o->image = new Image(fullPath);
                        } else {
                            o->image->setImage(fullPath);
                        }
                        Logging::Message("File %s", selection);
                    }
                    ImGui::EndDragDropTarget();
                }
                ImGui::TreePop();
            }
        }
    }
};

#endif //BRAKEZA3D_IMAGE2DGUI_H