#include "../../include/FXEffect/FXOffsetImage.h"
#include "../../include/EngineBuffers.h"
#include "../../include/Render/Logging.h"
#include "../../include/Brakeza3D.h"


FXOffsetImage::FXOffsetImage(const std::string& filename)
:
    FXEffectOpenGL(true),
    image(Image(filename)),
    useOffset(true),
    offsetX(0),
    offsetY(0),
    alpha(1.0f)
{
    setLabel("FXOffsetImage");
}

void FXOffsetImage::preUpdate()
{
    if (!this->enabled) return;
    update();
    executeKernelOpenCL(0, 0);
}

void FXOffsetImage::postUpdate()
{
}

void FXOffsetImage::update()
{
    FXEffectBase::update();
}

void FXOffsetImage::update(float increaseOffsetX, float increaseOffsetY)
{
    FXEffectBase::update();

    if (!this->enabled) return;
    executeKernelOpenCL(increaseOffsetX, increaseOffsetY);
}

void FXOffsetImage::executeKernelOpenCL(float increaseOffsetX, float increaseOffsetY)
{
    ComponentsManager::get()->getComponentWindow()->getShaderOGLImage()->renderTexture(
        this->image.getOGLTextureID(),
        0, 0,
        this->image.width(),
        this->image.height(),
        alpha,
        false,
        ComponentsManager::get()->getComponentWindow()->getBackgroundFramebuffer()
    );
}

void FXOffsetImage::limitOffset()
{
    const float maxOffset = 100.0f;
    const float minOffset = -100.0f;

    offsetX = fminf(maxOffset, fmaxf(minOffset, offsetX));
    offsetY = fminf(maxOffset, fmaxf(minOffset, offsetY));
}

void FXOffsetImage::resetOffsets()
{
    offsetX = 0;
    offsetY = 0;
}

bool FXOffsetImage::isUseOffset() const {
    return useOffset;
}

void FXOffsetImage::setUseOffset(bool value) {
    FXOffsetImage::useOffset = value;
}

Image &FXOffsetImage::getImage() {
    return image;
}

void FXOffsetImage::drawImGuiProperties()
{
    if (ImGui::TreeNode("Image")) {
        ImGui::Image((ImTextureID)image.getTexture(), ImVec2(128, 128));
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("IMAGE_ITEM")) {
                Logging::Message("Dropping IMAGE (%s) in %s", payload->Data, getLabel().c_str());
                IM_ASSERT(payload->DataSize == sizeof(int));
                std::string fileName = (const char *) payload->Data;
                image.setImage(EngineSetup::get()->IMAGES_FOLDER + fileName);
            }
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Offset")) {
        ImGui::Checkbox("Use offsets", reinterpret_cast<bool *>(&useOffset));
        if (useOffset) {
            const float range_min = -0;
            const float range_max = 1;
            const float range_sensibility = 0.001;

            ImGui::DragScalar("OffsetX", ImGuiDataType_Float, &offsetX, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("OffsetY", ImGuiDataType_Float, &offsetY, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
        }

        ImGui::TreePop();
    }
}

cJSON *FXOffsetImage::getJSON()
{
    cJSON *root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "type", this->getLabel().c_str());
    cJSON_AddStringToObject(root, "image", image.getFileName().c_str());
    cJSON_AddBoolToObject(root, "usingOffset", (bool) useOffset);
    cJSON_AddNumberToObject(root, "offsetX", offsetX);
    cJSON_AddNumberToObject(root, "offsetY", offsetY);

    return root;
}

void FXOffsetImage::setOffsets(float x, float y) {
    offsetX = x;
    offsetY = y;
}
