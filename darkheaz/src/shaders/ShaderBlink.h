
#ifndef BRAKEDA3D_SHADERBLINK_H
#define BRAKEDA3D_SHADERBLINK_H
#include "../../../include/Misc/Color.h"
#include "../../../include/Misc/Tools.h"
#include "../../../include/EngineBuffers.h"
#include "../../../include/Render/Transforms.h"
#include "../../../include/Render/Drawable.h"
#include "../../../include/Render/Logging.h"
#include "../../../include/Render/ShaderOpenCL.h"

class ShaderBlink : public ShaderOpenCL {
    bool isBlinking;
    int screenWidth;
    int screenHeight;
    Mesh3D* object;
    Color color;
    Counter counter;
public:
    ShaderBlink(bool active, Mesh3D *o, float step, Color c);

    void update() override;

    void executeKernelOpenCL();

    void setColor(Color color);
};
#endif //BRAKEDA3D_SHADERBLINK_H
