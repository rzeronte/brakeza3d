//
// Created by eduardo on 9/03/23.
//

#ifndef BRAKEZA3D_SHOCKWAVE_H
#define BRAKEZA3D_SHOCKWAVE_H


#include "../../include/Objects/Object3D.h"
#include "../../include/Misc/Counter.h"

class ShockWave : public Object3D {
private:
    float speed;
    bool deleteWhenEnds;
    float ttl;
public:
    ShockWave(Vertex3D position, float speed, float ttl, bool deleteWhenEnds);

    void onUpdate() override;

    void postUpdate() override;

    [[nodiscard]] float getSpeed() const;
};


#endif //BRAKEZA3D_SHOCKWAVE_H
