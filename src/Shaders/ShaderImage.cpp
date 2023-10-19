#include "../../include/Shaders/ShaderImage.h"
#include "../../include/EngineBuffers.h"
#include "../../include/Render/Logging.h"
#include "../../include/Brakeza3D.h"


ShaderImage::ShaderImage(const std::string& filename)
:
    ShaderOpenCL(true, "image.opencl"),
    image(Image(filename)),
    useOffset(true),
    offsetX(0),
    offsetY(0)
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
    offsetX += increaseOffsetX;
    offsetY += increaseOffsetY;

    limitOffset();

    clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::get()->screenWidth);
    clSetKernelArg(kernel, 1, sizeof(int), &EngineSetup::get()->screenHeight);
    clSetKernelArg(kernel, 2, sizeof(float), &Brakeza3D::get()->getExecutionTime());
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&EngineBuffers::get()->videoBufferOCL);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)image.getOpenClTexture());
    clSetKernelArg(kernel, 5, sizeof(int), &useOffset);
    clSetKernelArg(kernel, 6, sizeof(float), &offsetX);
    clSetKernelArg(kernel, 7, sizeof(float), &offsetY);

    size_t global_item_size[2] = {(size_t) EngineSetup::get()->screenWidth, (size_t) EngineSetup::get()->screenHeight};
    size_t local_item_size[2] = {16, 16};

    clEnqueueNDRangeKernel(clQueue, kernel, 2, NULL, global_item_size, local_item_size, 0, NULL, NULL);

    debugKernel("ShaderImage");
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
