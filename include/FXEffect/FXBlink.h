
#ifndef BRAKEDA3D_SHADERBLINK_H
#define BRAKEDA3D_SHADERBLINK_H
#include "../Misc/Color.h"
#include "../Misc/Tools.h"
#include "../Render/Transforms.h"
#include "../Render/Drawable.h"
#include "../Render/Logging.h"
#include "../OpenGL/FXOpenGL.h"

class FXBlink : public FXEffectOpenGLObject {
    bool isBlinking;
    float step;
    Mesh3D* object;
    Color color;
    Counter counter;
public:
    FXBlink(bool active, Mesh3D *o, float step, Color c);

    void update() override;

    void setColor(Color color);

    void preUpdate() override;

    void postUpdate() override;

    void drawImGuiProperties() override;

    void setCounter(float step);

    cJSON *getJSON() override;
};
#endif //BRAKEDA3D_SHADERBLINK_H
