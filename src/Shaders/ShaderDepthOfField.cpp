
#include "../../include/Shaders/ShaderDepthOfField.h"
#include "../../include/EngineSetup.h"
#include "../../include/Brakeza3D.h"

ShaderDepthOfField::ShaderDepthOfField(bool active)
:
    ShaderOpenCL(active, "dof.cl"),
    focusPlaneDepth(15000),
    focusRange(4500),
    blurSize(0.0000025f),
    intensity(1.15)
{
    setLabel("ShaderDepthOfField");
}

void ShaderDepthOfField::preUpdate()
{
}

void ShaderDepthOfField::postUpdate()
{
    if (!this->enabled) return;
    update();
    executeKernelOpenCL();
}

void ShaderDepthOfField::update()
{
    Shader::update();
}

void ShaderDepthOfField::executeKernelOpenCL()
{
    float dt = Brakeza3D::get()->getDeltaTime();

    clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::get()->screenWidth);
    clSetKernelArg(kernel, 1, sizeof(int), &EngineSetup::get()->screenHeight);
    clSetKernelArg(kernel, 2, sizeof(float), &dt);
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&EngineBuffers::get()->videoBufferOCL);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&EngineBuffers::get()->depthBufferOCL);
    clSetKernelArg(kernel, 5, sizeof(float), &focusPlaneDepth);
    clSetKernelArg(kernel, 6, sizeof(float), &focusRange);
    clSetKernelArg(kernel, 7, sizeof(float), &blurSize);
    clSetKernelArg(kernel, 8, sizeof(float), &intensity);

    size_t global_item_size[2] = {(size_t) EngineSetup::get()->screenWidth, (size_t) EngineSetup::get()->screenHeight};
    size_t local_item_size[2] = {16, 16};

    clRet = clEnqueueNDRangeKernel(clQueue, kernel, 2, NULL, global_item_size, local_item_size, 0, NULL, NULL);

    debugKernel("ShaderDepthOfField");
}

cJSON *ShaderDepthOfField::getJSON()
{
    cJSON *root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "type", this->getLabel().c_str());
    cJSON_AddNumberToObject(root, "focusPlaneDepth", focusPlaneDepth);
    cJSON_AddNumberToObject(root, "focusRange", focusRange);
    cJSON_AddNumberToObject(root, "blurSize", blurSize);
    cJSON_AddNumberToObject(root, "intensity", intensity);

    return root;
}

void ShaderDepthOfField::drawImGuiProperties()
{
    const float range_focus_min = 0;
    const float range_focus_max = 10000;
    const float range_focus_sensibility = 1;

    ImGui::DragScalar("Focus plane depth", ImGuiDataType_Float, &focusPlaneDepth, range_focus_sensibility ,&range_focus_min, &range_focus_max, "%f", 1.0f);
    ImGui::DragScalar("Focus range", ImGuiDataType_Float, &focusRange, range_focus_sensibility ,&range_focus_min, &range_focus_max, "%f", 1.0f);

    const float range_min = 0;
    const float range_max = 10000;
    const float range_sensibility = 0.01;

    ImGui::DragScalar("Blur size", ImGuiDataType_Float, &blurSize, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
    ImGui::DragScalar("Intensity", ImGuiDataType_Float, &intensity, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
}

void ShaderDepthOfField::setup(float focusPlaneDepth, float focusRange, float blurSize, float intensity)
{
    this->focusPlaneDepth = focusPlaneDepth;
    this->focusRange = focusRange;
    this->blurSize = blurSize;
    this->intensity = intensity;

}