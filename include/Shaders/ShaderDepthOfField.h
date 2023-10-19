//
// Created by eduardo on 28/05/23.
//

#ifndef BRAKEZA3D_SHADERDEPTHOFFIELD_H
#define BRAKEZA3D_SHADERDEPTHOFFIELD_H


#include "../Render/ShaderOpenCL.h"

class ShaderDepthOfField : public ShaderOpenCL {
public:
    ShaderDepthOfField(bool activ);

    void update() override;
    void executeKernelOpenCL();

    float focusPlaneDepth;
    float focusRange;
    float blurSize;
    float intensity;

    void preUpdate();

    void postUpdate();

    cJSON *getJSON();

    void drawImGuiProperties();

    void setup(float focusPlaneDepth, float focusRange, float blurSize, float intensity);
};


#endif //BRAKEZA3D_SHADERDEPTHOFFIELD_H
