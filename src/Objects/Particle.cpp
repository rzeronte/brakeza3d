#include "../../include/Objects/Particle.h"
#include "../../include/EngineSetup.h"
#include "../../include/Brakeza3D.h"

Particle::Particle(Object3D *parent, float force, float ttl, Color c) {
    setParent(parent);
    this->setPosition(parent->getPosition());
    this->setRotation(parent->getRotation());

    this->t = 0;
    this->timer.start();
    this->force = force;
    this->timeToLive.setStep(ttl);
    this->timeToLive.setEnabled(true);
    this->color = c;
}

void Particle::onUpdate() {
    Object3D::onUpdate();

    if (this->timeToLive.isFinished()) {
        this->timeToLive.setEnabled(true);
        this->removed = true;
    }

    this->timeToLive.update();

    float g = EngineSetup::get()->gravity.y;

    velocity = this->AxisForward().getScaled(force * Brakeza3D::get()->getDeltaTime());

    t += Brakeza3D::get()->getDeltaTime() / 1000;

    Vertex3D gravity(
        EngineSetup::get()->gravity.x * t,
        EngineSetup::get()->gravity.y * t - (0.5f * g * t * t),
        EngineSetup::get()->gravity.z * t
    );

    this->setPosition(getPosition() + gravity + velocity);

    Drawable::drawVertex(this->getPosition(), ComponentsManager::get()->getComponentCamera()->getCamera(), this->color);
}

const Vertex3D &Particle::getVelocity() const {
    return velocity;
}
