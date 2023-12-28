
#ifndef BRAKEDA3D_SHADERBLINK_H
#define BRAKEDA3D_SHADERBLINK_H
#include "../Misc/Color.h"
#include "../Misc/Tools.h"
#include "../EngineBuffers.h"
#include "../Render/Transforms.h"
#include "../Render/Drawable.h"
#include "../Render/Logging.h"
#include "../OpenGL/FXEffectOpenGL.h"

class FXBlink : public FXEffectOpenGLObject {
    bool isBlinking;
    Mesh3D* object;
    Color color;
    Counter counter;
public:
    FXBlink(bool active, Mesh3D *o, float step, Color c);

    void update() override;

    void executeKernelOpenCL();

    void setColor(Color color);

    void preUpdate() override;

    void postUpdate() override;
};
#endif //BRAKEDA3D_SHADERBLINK_H
