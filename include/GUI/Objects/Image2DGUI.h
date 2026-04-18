//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_IMAGE2DGUI_H
#define BRAKEZA3D_IMAGE2DGUI_H

#include "../../2D/Image2D.h"
#include "../../Misc/Logging.h"
#include "../../Misc/FilePaths.h"

class Image2DGUI
{
public:
    static void DrawPropertiesGUI(Image2D *o)
    {
        if (ImGui::CollapsingHeader("Image2D")) {

            const float scaleMin = 0.0f;
            const float scaleMax = 1.0f;
            if (ImGui::TreeNode("Screen Size")) {
                if (ImGui::DragScalar("Width", ImGuiDataType_Float, &o->widthScale, 0.005f, &scaleMin, &scaleMax, "%.3f", 1.0f)) {
                    o->setSize(o->widthScale, o->heightScale);
                }
                if (ImGui::DragScalar("Height", ImGuiDataType_Float, &o->heightScale, 0.005f, &scaleMin, &scaleMax, "%.3f", 1.0f)) {
                    o->setSize(o->widthScale, o->heightScale);
                }
                ImGui::TreePop();
            }
            ImGui::Separator();
            if (ImGui::TreeNode("Screen Position")) {
                if (ImGui::DragScalar("Offset X", ImGuiDataType_S32, &o->x, 1.f, nullptr, nullptr, "%d", 1.0f)) {
                    o->setScreenPosition(o->x, o->y);
                }
                if (ImGui::DragScalar("Offset Y", ImGuiDataType_S32, &o->y, 1.f, nullptr, nullptr, "%d", 1.0f)) {
                    o->setScreenPosition(o->x, o->y);
                }
                ImGui::TreePop();
            }
            ImGui::Separator();
            if (ImGui::TreeNode("Image / Video")) {
                bool hasVideo = o->getVideoPlayer() != nullptr;

                if (hasVideo) {
                    ImGui::TextColored(ImVec4(0.4f, 0.9f, 0.4f, 1.f), "[VIDEO]");
                    ImGui::SameLine();
                    if (ImGui::SmallButton("Stop")) {
                        o->setVideoPlayer(nullptr);
                    }
                }

                if (o->image && o->image->isLoaded()) {
                    float fixedWidth = std::min((int) ImGui::GetContentRegionAvail().x, o->image->width());
                    float h = fixedWidth * ((float) o->image->height() / (float) o->image->width());
                    ImGui::Image(o->image->getOGLImTexture(), ImVec2(fixedWidth, h));
                } else {
                    ImGui::TextDisabled("Drag an image or video here");
                }

                // Drop zone: accepts both IMAGE and VIDEO
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GUIType::DragDropTarget::IMAGE_ITEM)) {
                        auto selection = static_cast<char *>(payload->Data);
                        o->setVideoPlayer(nullptr);
                        if (o->image == nullptr) {
                            o->image = new Image(FilePath::ImageFile(selection));
                        } else {
                            o->image->setImage(FilePath::ImageFile(selection));
                            o->image->MakeAutoOGLImage();
                        }
                    }
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GUIType::DragDropTarget::VIDEO_ITEM)) {
                        auto selection = static_cast<char *>(payload->Data);
                        o->loadVideo(std::string(selection));
                    }
                    ImGui::EndDragDropTarget();
                }
                ImGui::TreePop();
            }
        }
    }
};

#endif //BRAKEZA3D_IMAGE2DGUI_H