//
// Created by Eduardo on 23/11/2025.
//

#ifndef BRAKEZA3D_IMAGE3DGUI_H
#define BRAKEZA3D_IMAGE3DGUI_H

#include "../Objects/Image3D.h"
#include "../Render/Logging.h"

inline void Image3D::drawImGuiProperties()
{
    Object3D::drawImGuiProperties();

    if (ImGui::CollapsingHeader("Image3D")) {

        const float range_min_int = 1.0;
        const float range_max_int = 1000;

        if (ImGui::TreeNode("Size")) {
            ImGui::DragScalar("Width", ImGuiDataType_Float, &width,1.f, &range_min_int, &range_max_int, "%f", 1.0f);
            ImGui::DragScalar("Height", ImGuiDataType_Float, &height,1.f, &range_min_int, &range_max_int, "%f", 1.0f);

            if (ImGui::Button(std::string("Update size").c_str())) {
                setSize(width, height);
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Image")) {
            if (image->isLoaded()) {
                ImGui::Image((ImTextureID) image->getOGLTextureID(),ImVec2(64, 64));
            } else {
                ImGui::Text("No image selected. Drag a texture here!");
            }

            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("IMAGE_ITEM")) {
                    Logging::Message("Dropping image (%s) in emitter %s", payload->Data, getLabel().c_str());
                    IM_ASSERT(payload->DataSize == sizeof(int));
                    auto selection = static_cast<char *>(payload->Data);
                    auto fullPath = EngineSetup::get()->IMAGES_FOLDER + selection;
                    if (image == nullptr) {
                        image = new Image(fullPath);
                    } else {
                        image->setImage(fullPath);
                    }
                    Logging::Message("File %s", selection);
                }
                ImGui::EndDragDropTarget();
            }
            ImGui::TreePop();
        }
    }
}
#endif //BRAKEZA3D_IMAGE3DGUI_H