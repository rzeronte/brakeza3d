//
// Created by eduardo on 11/11/22.
//

#ifndef BRAKEZA3D_SHADERHORIZONTALBAR_H
#define BRAKEZA3D_SHADERHORIZONTALBAR_H


#include "../../../include/Render/ShaderOpenCL.h"

class ShaderHorizontalBar: public ShaderOpenCL {
private:
    Color color;
    float verticalPosition;
    float value;

public:
    ShaderHorizontalBar(Color color, float verticalPosition);

    [[nodiscard]] float getValue() const;

    void executeKernelOpenCL();

    void setValue(float value);

    void update() override;
};


#endif //BRAKEZA3D_SHADERHORIZONTALBAR_H
