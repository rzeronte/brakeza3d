#define GL_GLEXT_PROTOTYPES

//
// Created by eduardo on 18/07/23.
//

#include "../../include/FXEffect/FXBlink.h"
#include "../../include/ComponentsManager.h"

FXBlink::FXBlink(bool active, Mesh3D *o, float step, Color c) :
    FXEffectOpenGLObject(active, o),
    step(step),
    isBlinking(false),
    object(o),
    color(c),
    counter(Counter(step))
{
    setLabel("FXBlink");
}

void FXBlink::preUpdate()
{
    FXEffectOpenGL::preUpdate();
}

void FXBlink::postUpdate()
{
    FXEffectOpenGL::postUpdate();
    update();
}

void FXBlink::update()
{
    if (!isEnabled() || this->object == nullptr) return;

    counter.update();

    if (isBlinking) {
        if (counter.isFinished()) {
            isBlinking = false;
            counter.setEnabled(true);
        }
    } else {
        if (counter.isFinished()) {
            isBlinking = true;
            counter.setEnabled(true);
        }
    }

    if (isBlinking) {
        auto mesh = dynamic_cast<Mesh3D*> (object);
        if (mesh != nullptr) {
            glDisable(GL_BLEND);

            glBindFramebuffer(GL_FRAMEBUFFER, ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer());
            ComponentsManager::get()->getComponentWindow()->getShaderOGLColor()->render(
                object,
                object->vertexbuffer,
                object->uvbuffer,
                object->normalbuffer,
                (int) object->vertices.size(),
                false,
                color
            );
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glEnable(GL_BLEND);
        }
    }
}

void FXBlink::setColor(Color color) {
    this->color = color;
}

void FXBlink::drawImGuiProperties()
{
    const float rangeMin = 0;
    const float rangeMax = 10;
    const float rangeSens = 0.01f;

    if (ImGui::DragScalar("Step", ImGuiDataType_Float, &step, rangeSens, &rangeMin, &rangeMax, "%f", 1.0f)){
        setCounter(step);
    }

    if (ImGui::TreeNode(("Blink color##" + getLabel()).c_str())) {
        ImVec4 imguiColor = color.toImVec4();

        if (ImGui::ColorEdit4("Color", (float *) &imguiColor, ImGuiColorEditFlags_NoOptions)) {
            setColor(Color(imguiColor.x,imguiColor.y,imguiColor.z));
        }
        ImGui::TreePop();
    }
}

void FXBlink::setCounter(float step) {
    counter.setStep(step);
    counter.setEnabled(true);
}