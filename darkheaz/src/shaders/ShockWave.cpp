//
// Created by eduardo on 9/03/23.
//

#include "ShockWave.h"
#include "../../../include/Brakeza3D.h"

ShockWave::ShockWave(Vertex3D position, float speed, float currentSize, float ttl, bool deleteWhenEnds)
:
    speed(speed),
    startSize(currentSize),
    currentSize(currentSize),
    ttlWave(Counter(ttl)),
    deleteWhenEnds(deleteWhenEnds)
{
    setPosition(position);
    ttlWave.setEnabled(true);
}

void ShockWave::onUpdate()
{
    ttlWave.update();

    currentSize -= (Brakeza3D::get()->getDeltaTime() * startSize) / ttlWave.getStep();

    if (ttlWave.isFinished()) {
        if (deleteWhenEnds) {
            setRemoved(true);
        } else {
            reset();
        }
    }
}

void ShockWave::postUpdate()
{
}

float ShockWave::getSpeed() const {
    return speed;
}

float ShockWave::getCurrentSize() const {
    return currentSize;
}

Counter &ShockWave::getTtlWave(){
    return ttlWave;
}

void ShockWave::reset()
{
    currentSize = startSize;
    ttlWave.setStep(ttlWave.getStep());
    ttlWave.setEnabled(true);
}
