//
// Created by eduardo on 31/05/23.
//

#include <sstream>
#include "EnemyDialog.h"

#include "../../../include/ComponentsManager.h"

EnemyDialog::EnemyDialog(int x, int y, float staminaPercentage, const char *message, float ttl, TTF_Font *font, Color color)
:
    x(x),
    y(y),
    staminaPercentage(staminaPercentage),
    message(message),
    counter(Counter(ttl)),
    background(new Image(EngineSetup::get()->IMAGES_FOLDER + "dialogBubble.png")),
    followEnemy(false),
    color(color)
{
    auto componentWindow = ComponentsManager::get()->getComponentWindow();
    writer = new TextWriter(componentWindow->getRenderer(), font);
    counter.setEnabled(false);

    radioWave = ComponentsManager::get()->getComponentGame()->getRadioWave();
    radioWave->setPaused(false);
}

void EnemyDialog::onUpdate(float staminaPercentage)
{
    if (!isEnabled()) return;

    auto gameState = ComponentsManager::get()->getComponentGame()->getGameState();

    if (gameState != EngineSetup::GAMING) return;

    if (!counter.isEnabled() && staminaPercentage <= this->staminaPercentage && counter.getAcumulatedTime() <= counter.getStep() * 0.25f) {
        counter.setEnabled(true);

        int radioMessagesCounter = ComponentsManager::get()->getComponentHUD()->getRadioMessagesCounter();

        y = 90 * radioMessagesCounter + 30  ;

        ComponentsManager::get()->getComponentHUD()->setRadioMessagesCounter(radioMessagesCounter + 1);
        ComponentsManager::get()->getComponentSound()->sound("radioBeep", EngineSetup::SoundChannels::SND_GLOBAL, 0);
    }
}


void EnemyDialog::onDraw(Image *avatar, Vertex3D position)
{
    Object3D::onDraw();
    auto gameState = ComponentsManager::get()->getComponentGame()->getGameState();

    if (gameState != EngineSetup::GAMING) return;

    if (!counter.isEnabled()) return;

    if (counter.getAcumulatedTime() >= counter.getStep()) {
        this->setRemoved(true);
        int radioMessagesCounter = ComponentsManager::get()->getComponentHUD()->getRadioMessagesCounter();
        ComponentsManager::get()->getComponentHUD()->setRadioMessagesCounter(radioMessagesCounter - 1);
    }

    if (followEnemy) {
        auto camera = ComponentsManager::get()->getComponentCamera()->getCamera();
        Point2D screenPoint = Transforms::WorldToPoint(position, camera);

        x = screenPoint.x;
        y = screenPoint.y;
    }

    float alpha = 0.0f;
    if (counter.getAcumulatedTime() <= counter.getStep() * 0.25f) {
        alpha = 255.0f * (counter.getAcumulatedTime() / (counter.getStep() * 0.25f));
    } else if (counter.getAcumulatedTime() >= counter.getStep() * 0.75f) {
        alpha = 255.0f * (1.0f - ((counter.getAcumulatedTime() - (counter.getStep() * 0.75f)) / (counter.getStep() * 0.25f)));
    } else {
        alpha = 255.0f;
    }

    drawDialog(alpha, avatar);

    counter.update();
}

void EnemyDialog::drawDialog(float alpha, Image *avatar)
{
    if (this->isRemoved()) return;

    int backgroundX = x;
    int backgroundY = y;
    int avatarX = x + 24;
    int avatarY = y + 20;
    int textX = x + 58;
    int textY = y + 20;
    int waveX = x + 26;
    int waveY = y + 56;

    background->drawFlatAlpha(backgroundX, backgroundY, alpha);
    avatar->drawFlatAlpha(avatarX, avatarY, alpha);

    std::istringstream stream(message);
    std::string line;
    int lineHeight = 15;

    writer->setAlpha(alpha);
    while (std::getline(stream, line)) {
        writer->writeTextTTFAutoSize(textX, textY, line.c_str(), color, 0.20f);
        textY += lineHeight;
    }

    radioWave->update();
    radioWave->getCurrentFrame()->getImage()->drawFlatAlpha(waveX, waveY, alpha);
}