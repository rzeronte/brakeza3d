//
// Created by Eduardo on 12/01/2025.
//

#include "../../include/2D/Image2D.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Render/Logging.h"
#include "../../include/Brakeza3D.h"

Image2D::Image2D(int x, int y, Image *image)
:
    x(x),
    y(y),
    image(image)
{
}

void Image2D::onUpdate()
{
    Object3D::onUpdate();

    if (!image->isLoaded()) return;

    const auto w = image->width();
    const auto h = image->height();

    image->drawFlatAlpha(
        x - w/2,
        y - h/2,
        alpha,
        ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer()
    );
}

void Image2D::updatePosition(int x, int y)
{
    this->x = x;
    this->y = y;
}

const char *Image2D::getTypeObject()
{
    return "Image2D";
}

const char *Image2D::getTypeIcon() {
    return "Image2DIcon";
}

void Image2D::drawImGuiProperties()
{
    Object3D::drawImGuiProperties();

    if (ImGui::TreeNode("Image2D")) {

        const int range_min_int = 1;
        const int range_max_int = 1280;

        if (ImGui::TreeNode("Screen Position")) {

            if (ImGui::DragScalar("Offset X", ImGuiDataType_S32, &x,1.f, &range_min_int, &range_max_int, "%d", 1.0f)) {
                updatePosition(x, y);
            }

            if (ImGui::DragScalar("Offset Y", ImGuiDataType_S32, &y,1.f, &range_min_int, &range_max_int, "%d", 1.0f)) {
                updatePosition(x, y);
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Image")) {
            if (image->isLoaded()) {
                ImGui::Image((ImTextureID) image->getOGLTextureID(),ImVec2(32, 32));
            } else {
                ImGui::Text("No image selected. Drag a texture here!");
            }

            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("IMAGE_ITEM")) {
                    Logging::Message("Dropping image (%s) in emitter %s", payload->Data, getLabel().c_str());
                    IM_ASSERT(payload->DataSize == sizeof(int));
                    auto selection = (char*) payload->Data;
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

        ImGui::TreePop();
    }
}

cJSON *Image2D::getJSON()
{
    auto root =  Object3D::getJSON();

    cJSON_AddNumberToObject(root, "x", x);
    cJSON_AddNumberToObject(root, "y", y);

    cJSON_AddStringToObject(root, "image", image->getFileName().c_str());

    return root;
}

void Image2D::createFromJSON(cJSON *object)
{
    auto name = cJSON_GetObjectItemCaseSensitive(object, "name")->valuestring;
    auto x = (float) cJSON_GetObjectItemCaseSensitive(object, "x")->valueint;
    auto y = (float) cJSON_GetObjectItemCaseSensitive(object, "y")->valueint;
    auto image = cJSON_GetObjectItemCaseSensitive(object, "image")->valuestring;

    auto o = new Image2D(x, y,new Image(image));

    Image2D::setPropertiesFromJSON(object, o);

    Brakeza3D::get()->addObject3D(o, name);
}

void Image2D::setPropertiesFromJSON(cJSON *object, Image2D *o)
{
    o->setBelongToScene(true);

    Object3D::setPropertiesFromJSON(object, o);
}
