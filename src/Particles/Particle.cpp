#include "../../include/Particles/Particle.h"
#include "../../include/EngineSetup.h"
#include "../../include/Brakeza3D.h"

Particle::Particle(Object3D *parent, float force, float ttl, Color c)
    : parent(parent), active(true), force(force), color(c)
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

    velocity = forward.getScaled(force * Brakeza3D::get()->getDeltaTime());

    position = position + velocity;
    color = (Color::red() * timeToLive.getAcumulatedTime()) + color;

    Drawable::drawVertex3D(getPosition(), color);
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
