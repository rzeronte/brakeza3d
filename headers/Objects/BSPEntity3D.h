#ifndef SDL2_3D_ENGINE_LIGHTPOINT_H
#define SDL2_3D_ENGINE_LIGHTPOINT_H

#include "Object3D.h"
#include "../Render/Billboard.h"


class BSPEntity3D : public Object3D {
public:

    BSPEntity3D();

    Billboard *billboard;
};


#endif //SDL2_3D_ENGINE_LIGHTPOINT_H
