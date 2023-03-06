#include "../../include/Particles/Particle.h"
#include "../../include/EngineSetup.h"
#include "../../include/Brakeza3D.h"

Particle::Particle(Object3D *parent, float force, float ttl, Color colorFrom, Color colorTo)
:
    parent(parent),
    active(true),
    force(force),
    colorFrom(colorFrom),
    colorTo(colorTo)
{
    this->setPosition(parent->getPosition());
    this->setRotation(parent->getRotation());

    this->timer.start();
    this->timeToLive.setStep(ttl);
    this->timeToLive.setEnabled(true);
}

void Particle::onUpdate()
{
    if (!isActive()) return;

    if (this->timeToLive.isFinished()) {
        setActive(false);
        return;
    };

    this->timeToLive.update();

    Vertex3D forward = rotation * EngineSetup::get()->forward;

    const auto dt = Brakeza3D::get()->getDeltaTime();

    velocity = forward.getScaled(force * dt);

    position = position + velocity;

    const float increase = timeToLive.getAcumulatedTime() / timeToLive.getStep();

    Drawable::drawVertex3D(getPosition(), Color::mixColor(colorFrom, colorTo, increase));
}

const Vertex3D &Particle::getPosition() const {
    return position;
}

void Particle::setPosition(const Vertex3D &position) {
    Particle::position = position;
}

const M3 &Particle::getRotation() const {
    return rotation;
}

void Particle::setRotation(const M3 &rotation) {
    Particle::rotation = rotation;
}

bool Particle::isActive() const {
    return active;
}

void Particle::setActive(bool active) {
    Particle::active = active;
}

Object3D *Particle::getParent() const {
    return parent;
}

void Particle::setParent(Object3D *parent) {
    Particle::parent = parent;
}
