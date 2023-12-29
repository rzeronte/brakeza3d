//
// Created by edu on 29/12/23.
//

#include "../../include/FXEffect/FXShockWave.h"
#include "../../include/Render/Transforms.h"
#include "../../include/ComponentsManager.h"

FXShockWave::FXShockWave(bool active, Object3D *object, float ttl, ShockWaveParams params)
:
    params(params),
    speed(1.0),
    deleteWhenEnds(false),
    ttlWave(Counter(ttl)),
    FXEffectOpenGLObject(active, object)
{
    setLabel("FXShockWave");
    ttlWave.setEnabled(true);
}

void FXShockWave::update()
{
    FXEffectBase::update();
    ttlWave.update();

    if (ttlWave.isFinished()) {
        ttlWave.setEnabled(true);
        if (deleteWhenEnds) {
            //object->setRemoved(true);
        }
    }

    auto window = ComponentsManager::get()->getComponentWindow();

    window->getShaderOGLShockWave()->render(
        Transforms::WorldToPoint(object->getPosition()),
        ttlWave.getAcumulatedTime(),
        speed,
        params,
        window->sceneTexture,
        window->getPostProcessingFramebuffer()
    );
}

void FXShockWave::preUpdate() {
    FXEffectOpenGL::preUpdate();
}

void FXShockWave::postUpdate() {
    FXEffectOpenGL::postUpdate();
    update();
}

void FXShockWave::drawImGuiProperties() {
    FXEffectOpenGL::drawImGuiProperties();

    const float rangeMin = 0;
    const float rangeMax = 1;
    const float rangeSens = 0.01f;

    ImGui::DragScalar("Step", ImGuiDataType_Float, &speed, rangeSens, &rangeMin, &rangeMax, "%f", 1.0f);

    const float rangeParamMin = 0;
    const float rangeParamMax = 25;
    const float rangeParamSens = 0.01f;

    ImGui::DragScalar("x", ImGuiDataType_Float, &params.param1, rangeParamSens, &rangeParamMin, &rangeParamMax, "%f", 1.0f);
    ImGui::DragScalar("y", ImGuiDataType_Float, &params.param2, rangeParamSens, &rangeParamMin, &rangeParamMax, "%f", 1.0f);
    ImGui::DragScalar("z", ImGuiDataType_Float, &params.param3, rangeParamSens, &rangeParamMin, &rangeParamMax, "%f", 1.0f);
}

cJSON *FXShockWave::getJSON() {
    return FXEffectOpenGL::getJSON();
}