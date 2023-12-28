//
// Created by eduardo on 19/11/22.
//

#include "../../include/FXEffect/FXColorTint.h"

FXColorTint::FXColorTint(bool active, Color color, float progress)
:
        FXEffectOpenGL(active),
        color(color),
        progress(progress)
{
}

void FXColorTint::update()
{
    FXEffectBase::update();

    if(!isEnabled()) return;

    executeKernelOpenCL();
}

void FXColorTint::executeKernelOpenCL()
{
}

void FXColorTint::setProgress(float value) {
    FXColorTint::progress = value;
}