#include "../../include/Shaders/ShaderImage.h"
#include "../../include/EngineBuffers.h"
#include "../../include/Render/Logging.h"
#include "../../include/Brakeza3D.h"


ShaderImage::ShaderImage(const std::string& filename)
:
    ShaderOpenCL(true),
    image(Image(filename)),
    useOffset(true),
    offsetX(0),
    offsetY(0),
    alpha(1.0f)
{
    setLabel("ShaderImage");
}

void ShaderImage::preUpdate()
{
    if (!this->enabled) return;
    update();
    executeKernelOpenCL(0, 0);
}

void ShaderImage::postUpdate()
{
}

void ShaderImage::update()
{
    Shader::update();
}

void ShaderImage::update(float increaseOffsetX, float increaseOffsetY)
{
    Shader::update();

    if (!this->enabled) return;
    executeKernelOpenCL(increaseOffsetX, increaseOffsetY);
}

void ShaderImage::executeKernelOpenCL(float increaseOffsetX, float increaseOffsetY)
{
    ComponentsManager::get()->getComponentWindow()->getShaderOGLImage()->renderTexture(
        this->image.getOGLTextureID(),
        0, 0,
        this->image.width(),
        this->image.height(),
        alpha,
        false,
        ComponentsManager::get()->getComponentWindow()->getBackgroundFramebuffer(),
        0
    );
    glClear(GL_DEPTH_BUFFER_BIT);
}

void ShaderImage::limitOffset()
{
    const float maxOffset = 100.0f;
    const float minOffset = -100.0f;

    offsetX = fminf(maxOffset, fmaxf(minOffset, offsetX));
    offsetY = fminf(maxOffset, fmaxf(minOffset, offsetY));
}

void ShaderImage::resetOffsets()
{
    offsetX = 0;
    offsetY = 0;
}

bool ShaderImage::isUseOffset() const {
    return useOffset;
}

void ShaderImage::setUseOffset(bool value) {
    ShaderImage::useOffset = value;
}

Image &ShaderImage::getImage() {
    return image;
}

void ShaderImage::drawImGuiProperties()
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

cJSON *ShaderImage::getJSON()
{
    cJSON *root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "type", this->getLabel().c_str());
    cJSON_AddStringToObject(root, "image", image.getFileName().c_str());
    cJSON_AddBoolToObject(root, "usingOffset", (bool) useOffset);
    cJSON_AddNumberToObject(root, "offsetX", offsetX);
    cJSON_AddNumberToObject(root, "offsetY", offsetY);

    return root;
}

void ShaderImage::setOffsets(float x, float y) {
    offsetX = x;
    offsetY = y;
}
