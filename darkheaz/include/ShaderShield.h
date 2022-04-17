//
// Created by eduardo on 27/3/22.
//

#ifndef BRAKEDA3D_SHADERSHIELD_H
#define BRAKEDA3D_SHADERSHIELD_H


#include "../../include/Render/Shader.h"
#include "../../include/Misc/Color.h"
#include "../../include/Objects/Vertex3D.h"
#include "../../include/Objects/Point2D.h"

class ShaderShield: public Shader {
    Color c;
    float radius;
    float offsetX;
    float offsetY;
    float speed;
public:
    ShaderShield(float radius, float speed);

    void onUpdate(Vertex3D position);
    void drawCircle(int x0, int y0, int radius);

    Color paintCircle(Point2D uv, Point2D center, float rad, float width);

    float smoothstep(float edge0, float edge1, float x);

    float variation(Point2D v1, Point2D v2, float strength, float speed);
};


#endif //BRAKEDA3D_SHADERSHIELD_H
