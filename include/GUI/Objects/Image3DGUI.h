//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_IMAGE3DDRAWERGUI_H
#define BRAKEZA3D_IMAGE3DDRAWERGUI_H

#include "../../3D/Image3D.h"
#include "../../Misc/Logging.h"

class Image3DGUI
{
public:
    static void DrawPropertiesGUI(Image3D *o)
    {
        if (ImGui::CollapsingHeader("Image3D")) {

            const float range_min_int = 1.0;
            const float range_max_int = 1000;

            if (ImGui::TreeNode("Size")) {
                ImGui::DragScalar("Width", ImGuiDataType_Float, &o->width,1.f, &range_min_int, &range_max_int, "%f", 1.0f);
                ImGui::DragScalar("Height", ImGuiDataType_Float, &o->height,1.f, &range_min_int, &range_max_int, "%f", 1.0f);

                if (ImGui::Button(std::string("Update size").c_str())) {
                    o->ResetBuffersToSize(o->width, o->height);
                }
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Image")) {
                if (o->image->isLoaded()) {
                    float fixedWidth = std::min((int) ImGui::GetContentRegionAvail().x, o->image->width());
                    float height = fixedWidth * ((float) o->image->height() / (float) o->image->width());
                    ImGui::Image(o->image->getOGLImTexture(),ImVec2(fixedWidth, height));
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
            ImGui::Separator();
            ImGui::Checkbox("Towards camera", &o->towardsCamera);
        }
    }
};

#endif //BRAKEZA3D_IMAGE3DDRAWERGUI_H