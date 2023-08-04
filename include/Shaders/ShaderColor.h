//
// Created by eduardo on 19/11/22.
//

#ifndef BRAKEZA3D_SHADERCOLOR_H
#define BRAKEZA3D_SHADERCOLOR_H


#include "../Render/ShaderOpenCL.h"
#include "../Misc/Image.h"

class ShaderColor: public ShaderOpenCL {
private:
    Color color;
    float progress;
public:
    ShaderColor(bool active, Color color, float progress);

    void update() override;

    void executeKernelOpenCL();

    void setProgress(float progress);
};


#endif //BRAKEZA3D_SHADERCOLOR_H
