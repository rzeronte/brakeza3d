
#include "../../include/Particles/ParticleEmissorGravity.h"
#include "../../include/Render/Logging.h"

ParticleEmissorGravity::ParticleEmissorGravity(bool active, float force, float ttl, float step, const Color &c)
        : ParticleEmissor(active, force, ttl, step, c) {
}

void ParticleEmissorGravity::onUpdate()
{
    ParticleEmissor::onUpdate();

    for (auto it = particles.begin(); it != particles.end();) {
        Particle *p = *(it);

        if (p->isRemoved()) {
            particles.erase(it);
            delete p;
            continue;
        } else {
            it++;
        }

        p->onUpdate();
    }
}

