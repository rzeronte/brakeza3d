//
// Created by eduardo on 7/08/23.
//

#include "SalvageSpaceship.h"
#include "../Player.h"
#include "../../include/ComponentsManager.h"

SalvageSpaceship::SalvageSpaceship()
:
    RotatableToTarget(ComponentsManager::get()->getComponentGame()->getPlayer(), this, 1.0f),
    getting(false),
    finished(false),
    timeGetting(Counter(3)),
    timeToRemove(Counter(3)),
    blink(new ShaderBlink(true, this, 0.05, ComponentsManager::get()->getComponentGame()->getPalette().getFirst()))
{
    timeGetting.setEnabled(false);
}

void SalvageSpaceship::postUpdate()
{
    Mesh3D::postUpdate();
}

void SalvageSpaceship::onUpdate()
{
    Mesh3D::onUpdate();

    timeGetting.update();
    timeToRemove.update();

    if (finished && timeToRemove.isFinished()) {
        removeCollisionObject();
        remove();
    }

    if (getting) {

        auto game = ComponentsManager::get()->getComponentGame();
        auto textWriter = game->getTextWriter();
        textWriter->writeTTFCenterHorizontal(390, "Boarding...", game->getPalette().getStamina(), 0.3);
        blink->update();

        const int width = 80.0f;
        const float stroke = 0.15f;

        const int startPositionX = (EngineSetup::get()->screenWidth / 2) - width/2;
        const int startPositionY = 410;
        const float topBarMargin = 5;
        float progress = timeGetting.currentPercentage() / 100;

        ComponentsManager::get()->getComponentHUD()->getShaderLasers()->addLaser(
            startPositionX, startPositionY + topBarMargin,
            startPositionX + (int) (width * progress), startPositionY + topBarMargin,
            ComponentsManager::get()->getComponentGame()->getPalette().getStamina(),
            stroke,
            false,
            false
        );

        ComponentsManager::get()->getComponentHUD()->getShaderLasers()->addLaser(
            startPositionX, startPositionY + topBarMargin,
            startPositionX + (int) (width * 1), startPositionY + topBarMargin,
            ComponentsManager::get()->getComponentGame()->getPalette().getFive(),
            stroke,
            false,
            false
        );

    }

    //finish
    if (getting && timeGetting.isFinished()) {
        ComponentsManager::get()->getComponentSound()->sound("salvageFinish", EngineSetup::SoundChannels::SND_GLOBAL, 0);
        finished = true;
        getting = false;
        timeGetting.setEnabled(false);
        timeToRemove.setEnabled(true);
        return;
    }

    // go out
    if (!getting && timeGetting.isEnabled()) {
        getting = false;
        timeGetting.setEnabled(false);
        blink->setEnabled(false);
        ComponentsManager::get()->getComponentSound()->sound("energyOff", EngineSetup::SoundChannels::SND_GLOBAL, 0);
        return;
    }

    getting = false;

    if (!finished) {
        makeRotationToTarget();
    } else {
        velocity = velocity + Vertex3D(0.5, 0, 0);
        addToPosition(AxisForward().getInverse().getScaled(50) + velocity);
    }
}

void SalvageSpaceship::resolveCollision(Collisionable *collisionable)
{
    if (finished) return;

    Mesh3DGhost::resolveCollision(collisionable);

    auto *player = dynamic_cast<Player*> (collisionable);
    if (player != nullptr) {

        // Start
        if (!getting && !timeGetting.isEnabled()) {
            blink->setEnabled(true);
            timeGetting.setEnabled(true);
            ComponentsManager::get()->getComponentSound()->sound("salvageConnection", EngineSetup::SoundChannels::SND_GLOBAL, 0);
        }

        getting = true;

        return;
    }
}
