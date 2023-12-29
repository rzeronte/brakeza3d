//
// Created by eduardo on 19/11/22.
//

#include "../../include/FXEffect/FXColorTint.h"
#include "../../include/ComponentsManager.h"

FXColorTint::FXColorTint(bool active, Color color, float progress)
:
    FXEffectOpenGL(active),
    color(color),
    progress(progress)
{
    setLabel("FXColorTint");
}

void FXColorTint::update()
{
    FXEffectBase::update();

    if(!isEnabled()) return;

    auto windows = ComponentsManager::get()->getComponentWindow();

    windows->getShaderOGLTint()->render(
        color,
        progress,
        windows->getPostProcessingFramebuffer()
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
    FXEffectOpenGL::preUpdate();

    if(!isEnabled()) return;

    auto windows = ComponentsManager::get()->getComponentWindow();

    windows->getShaderOGLTint()->render(
            color,
            progress,
            windows->getPostProcessingFramebuffer()
    );
}

void FXColorTint::postUpdate() {
    FXEffectOpenGL::postUpdate();
}

cJSON *FXColorTint::getJSON() {
    return FXEffectOpenGL::getJSON();
}
