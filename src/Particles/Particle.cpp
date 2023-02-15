#include "../../include/Particles/Particle.h"
#include "../../include/EngineSetup.h"
#include "../../include/Brakeza3D.h"

Particle::Particle(Object3D *parent, float force, float ttl, Color c, bool affectedByGravity)
    : force(force), t(0), color(c), affedByGravity(affectedByGravity)
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

    if (affedByGravity) {
        const float g = EngineSetup::get()->gravity.y;

        t += Brakeza3D::get()->getDeltaTime() / 1000;

        this->addToPosition(Vertex3D(
            EngineSetup::get()->gravity.x * t,
            EngineSetup::get()->gravity.y * t - (0.5f * g * t * t),
            EngineSetup::get()->gravity.z * t
        ));
    }

    Drawable::drawVertex3D(getPosition(), color);
}
