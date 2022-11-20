//
// Created by eduardo on 11/11/22.
//

#ifndef BRAKEZA3D_SHADERHORIZONTALBAR_H
#define BRAKEZA3D_SHADERHORIZONTALBAR_H


#include "../../../include/Render/ShaderOpenCL.h"

class ShaderHorizontalBar: public ShaderOpenCL
{
public:
    ShaderHorizontalBar(Color color, float verticalPosition);

private:
    Color color;
    float verticalPosition;
    float value;
    void executeKernelOpenCL();

public:
    float getValue() const;

    void setValue(float value);

public:
    void update() override;
};


#endif //BRAKEZA3D_SHADERHORIZONTALBAR_H
