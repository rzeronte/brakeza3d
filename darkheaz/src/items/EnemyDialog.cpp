//
// Created by eduardo on 31/05/23.
//

#include "EnemyDialog.h"

#include <utility>
#include "../../../include/ComponentsManager.h"

EnemyDialog::EnemyDialog(int x, int y, float staminaPercentage, const char *message, float ttl, EnemyGhost *enemy)
:
    x(x),
    y(y),
    staminaPercentage(staminaPercentage),
    message(message),
    counter(Counter(ttl)),
    enemy(enemy),
    background(new Image(EngineSetup::get()->IMAGES_FOLDER + "dialogBubble.png")),
    followEnemy(false)
{
    counter.setEnabled(false);
}

void EnemyDialog::onUpdate()
{
    Object3D::onUpdate();

    if (!isEnabled()) return;

    const float staminaPercentage = (enemy->getStamina() * 100) / enemy->getStartStamina();

    if (!counter.isEnabled() && staminaPercentage <= this->staminaPercentage) {
        counter.setEnabled(true);

        ComponentSound::playSound(
            ComponentsManager::get()->getComponentSound()->getSoundPackage().getByLabel("radioBeep"),
            EngineSetup::SoundChannels::SND_GLOBAL,
            0
        );
    }
}

void EnemyDialog::postUpdate() {
    Object3D::postUpdate();
}

void EnemyDialog::drawCall() {
    Object3D::drawCall();

}

void EnemyDialog::onDraw()
{
    Object3D::onDraw();

    if (counter.isEnabled()) {
        if (counter.isFinished()) {
            this->setRemoved(true);
        }

        if (enemy == nullptr || enemy->isRemoved()) return;

        if (followEnemy) {
            auto camera = ComponentsManager::get()->getComponentCamera()->getCamera();
            Point2D screenPoint = Transforms::WorldToPoint(enemy->getPosition(), camera);

            x = screenPoint.x;
            y = screenPoint.y;
        }

        drawDialog();

        counter.update();
    };
}

void EnemyDialog::drawDialog()
{
    int backgroundX = x;
    int backgroundY = y;
    int avatarX = x + 18;
    int avatarY = y + 16;
    int textX = x + 18;
    int textY = y + 42;

    background->drawFlat(backgroundX, backgroundY);

    enemy->getAvatar()->drawFlat(avatarX, avatarY);

    auto textWriter = ComponentsManager::get()->getComponentGame()->getTextWriter();
    textWriter->writeTextTTFAutoSize(textX, textY, message.c_str(), Color::green(), 0.15f);
}
