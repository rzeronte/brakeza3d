//
// Created by eduardo on 19/11/22.
//

#include "../../include/FXEffect/FXColorTint.h"
#include "../../include/Components/ComponentsManager.h"

FXColorTint::FXColorTint(bool active, Color color, float progress)
:
        FXOpenGL(active),
        color(color),
        progress(progress)
{
    setLabel("FXColorTint");
}

void FXColorTint::update()
{
    FXBase::update();

    if (!isEnabled()) return;

    auto render = ComponentsManager::get()->getComponentRender();
    auto windows = ComponentsManager::get()->getComponentWindow();

    render->getShaderOGLTint()->render(
        color,
        progress,
        windows->getGlobalFramebuffer()
    );
}

void FXColorTint::setProgress(float value) {
    FXColorTint::progress = value;
}

void FXColorTint::drawImGuiProperties()
{
    const float rangeParamMin = 0;
    const float rangeParamMax = 1;
    const float rangeParamSens = 0.01f;

    ImGui::DragScalar("alpha", ImGuiDataType_Float, &progress, rangeParamSens, &rangeParamMin, &rangeParamMax, "%f", 1.0f);

    if (ImGui::TreeNode(("Edge color##" + getLabel()).c_str())) {
        ImVec4 imguiColor = color.toImVec4();

        if (ImGui::ColorEdit4("Color", (float *) &imguiColor, ImGuiColorEditFlags_NoOptions)) {
            setColor(Color(imguiColor.x,imguiColor.y,imguiColor.z));
        }
        ImGui::TreePop();
    }
}

void FXColorTint::setColor(const Color &color) {
    FXColorTint::color = color;
}

void FXColorTint::preUpdate()
{
    FXOpenGL::preUpdate();
}

void FXColorTint::postUpdate() {
    FXOpenGL::postUpdate();
    update();
}

cJSON *FXColorTint::getJSON()
{
    cJSON *root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "type", this->getLabel().c_str());
    cJSON_AddBoolToObject(root, "enabled", isEnabled());
    cJSON_AddNumberToObject(root, "alpha", progress);
    cJSON *colorJSON = cJSON_CreateObject();
    cJSON_AddNumberToObject(colorJSON, "r", color.r);
    cJSON_AddNumberToObject(colorJSON, "g", color.g);
    cJSON_AddNumberToObject(colorJSON, "b", color.b);
    cJSON_AddItemToObject(root, "color", colorJSON);

    return root;
}
