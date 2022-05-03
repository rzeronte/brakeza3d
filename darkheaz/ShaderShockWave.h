//
// Created by eduardo on 27/3/22.
//

#ifndef BRAKEDA3D_SHADERSHOCKWAVE_H
#define BRAKEDA3D_SHADERSHOCKWAVE_H


#include "../include/Render/Shader.h"
#include "../include/Misc/Color.h"
#include "../include/Objects/Vertex3D.h"
#include "../include/Objects/Point2D.h"

class ShaderShockWave: public Shader {
    float size;
    float waveSpeed;
public:
    ShaderShockWave(float size, float waveSpeed);

    void onUpdate(Vertex3D position);

    float smoothstep(float edge0, float edge1, float x);

    float fract(float x);
};


#endif //BRAKEDA3D_SHADERSHOCKWAVE_H
