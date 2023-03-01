//
// Created by eduardo on 19/11/22.
//

#ifndef BRAKEZA3D_SHADERCOLOR_H
#define BRAKEZA3D_SHADERCOLOR_H


#include "../../../include/Render/ShaderOpenCL.h"
#include "../../../include/Misc/Image.h"

class ShaderColor: public ShaderOpenCL {
    Color color;
    float progress;
public:
    ShaderColor(Color color, float progress);

    void update() override;

    ~ShaderColor() override;

    void executeKernelOpenCL();

    void setProgress(float progress);

    [[nodiscard]] float getProgress() const;
};


#endif //BRAKEZA3D_SHADERCOLOR_H
