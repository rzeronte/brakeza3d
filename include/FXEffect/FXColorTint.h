//
// Created by eduardo on 19/11/22.
//

#ifndef BRAKEZA3D_FXCOLORTINT_H
#define BRAKEZA3D_FXCOLORTINT_H


#include "../OpenGL/FXOpenGL.h"
#include "../Render/Image.h"

class FXColorTint: public FXOpenGL {
private:
    Color color;
    float progress;
public:
    FXColorTint(bool active, Color color, float progress);

    void update() override;

    void setProgress(float progress);

    void drawImGuiProperties() override;

    void setColor(const Color &color);

    void preUpdate() override;

    void postUpdate() override;

    cJSON *getJSON() override;
};


#endif //BRAKEZA3D_FXCOLORTINT_H
