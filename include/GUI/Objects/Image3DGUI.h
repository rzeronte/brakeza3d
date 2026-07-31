//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_IMAGE3DDRAWERGUI_H
#define BRAKEZA3D_IMAGE3DDRAWERGUI_H

#include "../../3D/Image3D.h"
#include "../../Misc/Logging.h"
#include "../../Config.h"
#include "../AddOns/GUIAddonFilePicker.h"

class Image3DGUI
{
    static inline GUIFilePicker picker;

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
                    ImGui::Text("No image selected. Drag a texture here or Browse.");
                }

                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GUIType::DragDropTarget::IMAGE_ITEM)) {
                        auto selection = static_cast<char *>(payload->Data);
                        if (o->image == nullptr) {
                            o->image = new Image(FilePath::ImageFile(selection));
                            o->image->MakeAutoOGLImage();
                        } else {
                            o->image->setImage(FilePath::ImageFile(selection));
                            o->image->MakeAutoOGLImage();
                        }
                        LOG_MESSAGE("File %s", selection);
                    }
                    ImGui::EndDragDropTarget();
                }

                if (picker.drawTrigger("##browse_img3d")) {
                    std::string current = o->image ? o->image->getFileName().path : "";
                    picker.open(current, Config::get()->IMAGES_FOLDER,
                                "../assets/images/", {".png", ".jpg", ".jpeg", ".bmp", ".tga"});
                }

                if (picker.draw("Image3D_FilePicker")) {
                    if (o->image == nullptr) {
                        o->image = new Image(FilePath::ImageFile(picker.result));
                        o->image->MakeAutoOGLImage();
                    } else {
                        o->image->setImage(FilePath::ImageFile(picker.result));
                        o->image->MakeAutoOGLImage();
                    }
                }

                ImGui::TreePop();
            }
            ImGui::Separator();
            ImGui::Checkbox("Towards camera", &o->towardsCamera);
        }
    }
};

#endif //BRAKEZA3D_IMAGE3DDRAWERGUI_H
