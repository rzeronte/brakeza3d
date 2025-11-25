//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_IMAGE3DDRAWERGUI_H
#define BRAKEZA3D_IMAGE3DDRAWERGUI_H

#include "../../3D/Image3D.h"
#include "../../Render/Logging.h"

class Image3DGUI
{
public:
    static void drawImGuiProperties(Image3D *o)
    {
        if (ImGui::CollapsingHeader("Image3D")) {

            const float range_min_int = 1.0;
            const float range_max_int = 1000;

            if (ImGui::TreeNode("Size")) {
                ImGui::DragScalar("Width", ImGuiDataType_Float, &o->width,1.f, &range_min_int, &range_max_int, "%f", 1.0f);
                ImGui::DragScalar("Height", ImGuiDataType_Float, &o->height,1.f, &range_min_int, &range_max_int, "%f", 1.0f);

                if (ImGui::Button(std::string("Update size").c_str())) {
                    o->setSize(o->width, o->height);
                }
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Image")) {
                if (o->image->isLoaded()) {
                    ImGui::Image(o->image->getOGLImTexture(),ImVec2(64, 64));
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

#endif //BRAKEZA3D_IMAGE3DDRAWERGUI_H