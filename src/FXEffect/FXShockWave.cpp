//
// Created by edu on 29/12/23.
//

#include "../../include/FXEffect/FXShockWave.h"
#include "../../include/Render/Transforms.h"
#include "../../include/ComponentsManager.h"

FXShockWave::FXShockWave(bool active, Object3D *object, float ttl)
:
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

    auto p = Transforms::WorldToPoint(object->getPosition());

    auto window = ComponentsManager::get()->getComponentWindow();
    window->getShaderOGLShockWave()->setPoint(p);
    window->getShaderOGLShockWave()->setTimeLive(ttlWave.getAcumulatedTime());
    window->getShaderOGLShockWave()->render(window->sceneTexture,window->getPostProcessingFramebuffer());
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
}

cJSON *FXShockWave::getJSON() {
    return FXEffectOpenGL::getJSON();
}