
#ifndef SDL2_3D_ENGINE_TRIANGLE_H
#define SDL2_3D_ENGINE_TRIANGLE_H

#include "../3D/Vertex3D.h"
#include "../3D/Vector3D.h"
#include "../Components/Camera3D.h"
#include "Point2D.h"
#include "../3D/Object3D.h"
#include <list>
#include <vector>
#include "../Misc/Color.h"
#include "Image.h"

class Triangle {

public:
    Vertex3D A, B, C;
    Vertex3D Ao, Bo, Co;
    Vertex3D An, Bn, Cn;
    Point2D As, Bs, Cs;

    bool enableLights;

    Object3D *parent;
    Vertex3D normal;

    bool clipped;

    Triangle();

    Triangle(Vertex3D A, Vertex3D B, Vertex3D C, Object3D *parent);

    void updateObjectSpace();

    void updateNormal();

    void setEnableLights(bool enableLights);

    [[nodiscard]] bool isPointInside(Vertex3D) const;

    [[nodiscard]] Vertex3D getNormal() const;

    [[nodiscard]] bool isEnableLights() const;
};

#endif //SDL2_3D_ENGINE_TRIANGLE_H
