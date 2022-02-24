#ifndef BRAKEDA3D_SHADER_H
#define BRAKEDA3D_SHADER_H


#include "../Misc/Timer.h"

class Shader {
public:
    Shader();

    uint32_t *videoBuffer;
    int w;
    int h;

    float deltaTime = 0;
    float last_ticks = 0;
    float current_ticks = 0;
    float executionTime = 0;

    Timer t;

    virtual void onUpdate();
};


#endif //BRAKEDA3D_SHADER_H
