//
// Created by eduardo on 9/03/23.
//

#ifndef BRAKEZA3D_SHOCKWAVE_H
#define BRAKEZA3D_SHOCKWAVE_H


#include "../../../include/Objects/Object3D.h"
#include "../../../include/Misc/Counter.h"

class ShockWave : public Object3D {
private:
    float speed;
    float startSize;
    float currentSize;
    Counter ttlWave;
    bool deleteWhenEnds;
public:
    ShockWave(Vertex3D position, float speed, float currentSize, float ttl, bool deleteWhenEnds);

    void onUpdate() override;

    void postUpdate() override;

    [[nodiscard]] float getSpeed() const;

    [[nodiscard]] float getCurrentSize() const;

    [[nodiscard]] Counter &getTtlWave();

    void reset();
};


#endif //BRAKEZA3D_SHOCKWAVE_H
