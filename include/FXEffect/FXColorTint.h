//
// Created by eduardo on 19/11/22.
//

#ifndef BRAKEZA3D_FXCOLORTINT_H
#define BRAKEZA3D_FXCOLORTINT_H


#include "../OpenGL/FXEffectOpenGL.h"
#include "../Misc/Image.h"

class FXColorTint: public FXEffectOpenGL {
private:
    Color color;
    float progress;
public:
    FXColorTint(bool active, Color color, float progress);

    void update() override;

    void executeKernelOpenCL();

    void setProgress(float progress);
};


#endif //BRAKEZA3D_FXCOLORTINT_H
