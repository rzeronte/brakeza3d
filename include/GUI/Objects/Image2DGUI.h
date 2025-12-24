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
    static void DrawPropertiesGUI(Image2D *o)
    {
        if (ImGui::CollapsingHeader("Image2D")) {

            const int range_min_int = 1;
            const int range_max_int = Config::get()->screenWidth;
            if (ImGui::TreeNode("Screen Size")) {
                if (ImGui::DragScalar("Offset X", ImGuiDataType_S32, &o->width,1.f, &range_min_int, &range_max_int, "%d", 1.0f)) {
                    o->setScreenPosition(o->x, o->y);
                }
                if (ImGui::DragScalar("Offset Y", ImGuiDataType_S32, &o->height,1.f, &range_min_int, &range_max_int, "%d", 1.0f)) {
                    o->setScreenPosition(o->x, o->y);
                }
                ImGui::TreePop();
            }
            ImGui::Separator();
            if (ImGui::TreeNode("Screen Position")) {
                if (ImGui::DragScalar("Offset X", ImGuiDataType_S32, &o->x,1.f, &range_min_int, &range_max_int, "%d", 1.0f)) {
                    o->setScreenPosition(o->x, o->y);
                }
                if (ImGui::DragScalar("Offset Y", ImGuiDataType_S32, &o->y,1.f, &range_min_int, &range_max_int, "%d", 1.0f)) {
                    o->setScreenPosition(o->x, o->y);
                }
                ImGui::TreePop();
            }
            ImGui::Separator();
            if (ImGui::TreeNode("Image")) {
                if (o->image->isLoaded()) {
                    float fixedWidth = std::min((int) ImGui::GetContentRegionAvail().x, o->image->width());
                    float height = fixedWidth * ((float) o->image->height() / (float) o->image->width());
                    ImGui::Image(o->image->getOGLImTexture(), ImVec2(fixedWidth, height));
                } else {
                    ImGui::Text("No image selected. Drag a texture here!");
                }

                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GUIType::DragDropTarget::IMAGE_ITEM)) {
                        LOG_MESSAGE("Dropping image (%s) in emitter %s", payload->Data, o->getName().c_str());
                        IM_ASSERT(payload->DataSize == sizeof(int));
                        auto selection = static_cast<char *>(payload->Data);
                        auto fullPath = Config::get()->IMAGES_FOLDER + selection;
                        if (o->image == nullptr) {
                            o->image = new Image(fullPath);
                        } else {
                            o->image->setImage(fullPath);
                        }
                        LOG_MESSAGE("File %s", selection);
                    }
                    ImGui::EndDragDropTarget();
                }
                ImGui::TreePop();
            }
        }
    }
};

#endif //BRAKEZA3D_IMAGE2DGUI_H