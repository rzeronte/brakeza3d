//
// Created by darkhead on 8/2/21.
//

#include "DoorGhost.h"
#include "../EngineBuffers.h"
#include "../Brakeza3D.h"

DoorGhost::DoorGhost() {
    moving = false;
    reverseMoving = false;
    waiting = false;
    offsetMoving = 0;
    timer = new Timer();
}

void DoorGhost::integrate() {
    if (this->isMoving() || this->isReverseMoving() || this->isWaiting()) {
        this->integrateMoving();
    }

    // Sync position
    btTransform t = ghostObject->getWorldTransform();
    btVector3 pos = t.getOrigin();

    Vertex3D worldPosition = Vertex3D(pos.getX(), pos.getY(), pos.getZ());
    this->setPosition(worldPosition);

    // Sync rotation
    btQuaternion quat = t.getRotation();
    float angle = quat.getAngle();
    btVector3 axis = quat.getAxis();
}

void DoorGhost::integrateMoving() {
    if (this->isWaiting()) {
        // Frame secuence control
        float deltatime = this->timer->getTicks() - this->last_ticks;
        this->last_ticks = this->timer->getTicks();
        this->timerCurrent += (deltatime / 1000.f);

        if (this->timerCurrent >= waitTime) {
            timer->stop();
            this->setWaiting(false);
            this->setReverseMoving(true);
            Tools::playMixedSound(EngineBuffers::getInstance()->soundPackage->getSoundByLabel("openDoor"),
                                  EngineSetup::SoundChannels::SND_ENVIRONMENT, 0);
        }

        return;
    }

    float sizeZ = this->aabb.max.x - this->aabb.min.x;
    float sizeX = this->aabb.max.y - this->aabb.min.y;
    float sizeY = this->aabb.max.z - this->aabb.min.z;

    // Sync position
    btTransform t = this->ghostObject->getWorldTransform();
    btVector3 bulletBodyPosition = t.getOrigin();

    // horizontal move
    if (angleMoving >= 0) {
        Vertex3D horizontalMovement = this->integrateHorizontalMovement(sizeX, sizeZ);
        bulletBodyPosition.setX( bulletBodyPosition.getX() + horizontalMovement.x);
        bulletBodyPosition.setY( bulletBodyPosition.getY() + horizontalMovement.y);
        bulletBodyPosition.setZ( bulletBodyPosition.getZ() + horizontalMovement.z);
    }

    // vertical move down
    if (angleMoving == VERTICAL_MOVEMENT_ANGLE) {
        Vertex3D verticalMovement = this->integrateVerticalMovement(sizeY);
        bulletBodyPosition.setX( bulletBodyPosition.getX() + verticalMovement.x);
        bulletBodyPosition.setY( bulletBodyPosition.getY() + verticalMovement.y);
        bulletBodyPosition.setZ( bulletBodyPosition.getZ() + verticalMovement.z);
    }

    t.setOrigin(bulletBodyPosition);
    this->ghostObject->setWorldTransform(t);
}

Vertex3D DoorGhost::integrateHorizontalMovement(float sizeX, float sizeZ) {
    Vertex3D move = Vertex3D::zero();

    M3 r = M3::getMatrixRotationForEulerAngles(0, -angleMoving, 0);
    Vertex3D right = r * EngineSetup::get()->right;

    if (reverseMoving) {
        right = right.getInverse();
    }

    float maxMovementSize = sizeZ;
    if (angleMoving == 90 || angleMoving == 270) {
        maxMovementSize = sizeZ;
    }
    if (angleMoving == 0 || angleMoving == 180) {
        maxMovementSize = sizeX;
    }

    // Si he superado el límite actualizo estado
    if (offsetMoving >= abs(maxMovementSize)) {

        if (reverseMoving) {
            reverseMoving = false;
        } else {
            timer->start();
            moving = false;
            reverseMoving = false;
            waiting = true;
        }

        offsetMoving = 0;

        return move;
    }

    move = right.getScaled(Brakeza3D::get()->getDeltaTime() * this->speedMoving);

    if ((offsetMoving + move.getModule()) > abs(maxMovementSize)) {
        move.setLength(abs(maxMovementSize) - offsetMoving);
    }

    offsetMoving += move.getModule();

    return move;
}

Vertex3D DoorGhost::integrateVerticalMovement(float sizeY) {
    Vertex3D move;
    Vertex3D down = EngineSetup::get()->down;

    if (reverseMoving) {
        down = down.getInverse();
    }

    if (offsetMoving >= abs(sizeY)) {
        if (reverseMoving) {
            reverseMoving = false;
        } else {
            timer->start();
            moving = false;
            reverseMoving = false;
            waiting = true;
        }

        offsetMoving = 0;
        return move;
    }

    move = down.getScaled(Brakeza3D::get()->getDeltaTime() * this->speedMoving);
    offsetMoving += move.getModule();

    return move;
}

bool DoorGhost::isMoving() const {
    return moving;
}

void DoorGhost::setMoving(bool m) {
    DoorGhost::moving = m;
}

bool DoorGhost::isReverseMoving() const {
    return reverseMoving;
}

void DoorGhost::setReverseMoving(bool m) {
    DoorGhost::reverseMoving = m;
}

void DoorGhost::setWaiting(bool w) {
    DoorGhost::waiting = w;
}

bool DoorGhost::isWaiting() const {
    return waiting;
}

void DoorGhost::setSpeedMoving(float speed) {
    this->speedMoving = speed;
}


float DoorGhost::getSpeedMoving() {
    return this->speedMoving;
}

float DoorGhost::getAngleMoving() const {
    return angleMoving;
}

void DoorGhost::setAngleMoving(float angle) {
    DoorGhost::angleMoving = angle;
}

