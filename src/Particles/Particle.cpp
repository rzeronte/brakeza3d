#include "../../include/Particles/Particle.h"
#include "../../include/EngineSetup.h"
#include "../../include/Brakeza3D.h"

Particle::Particle(Object3D *parent, float force, float ttl, Color c)
    : force(force), color(c)
{
    setParent(parent);
    this->setPosition(parent->getPosition());
    this->setRotation(parent->getRotation());

    this->timer.start();
    this->timeToLive.setStep(ttl);
    this->timeToLive.setEnabled(true);
}

void Particle::onUpdate()
{

    if (this->timeToLive.isFinished()) {
        this->timeToLive.setEnabled(true);
        setRemoved(true);
    };

    this->timeToLive.update();

    velocity = this->AxisForward().getScaled(force * Brakeza3D::get()->getDeltaTime());

    addToPosition(velocity);
    color = (Color::red() * timeToLive.getAcumulatedTime()) + color;

    Drawable::drawVertex3D(getPosition(), color);
}

void Particle::postUpdate() {

}
