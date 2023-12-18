
#include "../../include/Shaders/ShaderDepthOfField.h"
#include "../../include/EngineSetup.h"
#include "../../include/Brakeza3D.h"

ShaderDepthOfField::ShaderDepthOfField(bool active)
:
    ShaderOpenCL(active),
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