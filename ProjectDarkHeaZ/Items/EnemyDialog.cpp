//
// Created by eduardo on 31/05/23.
//

#include <sstream>
#include "EnemyDialog.h"

#include "../../include/ComponentsManager.h"

EnemyDialog::EnemyDialog(Object3D *parent, Image *avatar, Image * avatarSmall, float staminaPercentage, const char *message, const char *sound, const char *from, TTF_Font *font)
:
    offsetX(0),
    offsetY(0),
    showed(false),
    parent(parent),
    message(message),
    sound(sound),
    from(from),
    background(new Image(EngineSetup::get()->IMAGES_FOLDER + "radio_notification.png")),
    degradateBottom(new Image(EngineSetup::get()->IMAGES_FOLDER + "degradate_bottom.png")),
    avatar(avatar),
    avatarSmall(avatarSmall),
    counterTTL(Counter(ComponentsManager::get()->getComponentSound()->soundDuration(sound))),
    staminaPercentage(staminaPercentage)
{
    writer = new TextWriter(ComponentsManager::get()->getComponentWindow()->getRenderer(), font);
}

EnemyDialog::~EnemyDialog()
{
    delete writer;
    delete background;
    delete degradateBottom;
}

void EnemyDialog::update()
{
    counterTTL.update();

    if (counterTTL.isFinished()) {
        ComponentsManager::get()->getComponentGame()->setCurrentEnemyDialog(nullptr);
        ComponentsManager::get()->getComponentSound()->sound("radioBeep", EngineSetup::SoundChannels::SND_GLOBAL, 0);
        return;
    }

    updateOffsets();

    drawDialog(255);
}

void EnemyDialog::drawDialog(float alpha)
{
    Point2D positionAvatar(0, -avatar->height() + (avatar->height() * smoothEffect));
    Point2D positionBackground(0, -background->height() + (background->height() * smoothEffect));

    degradateBottom->drawFlatAlpha(positionBackground.x, positionBackground.y, alpha);
    avatar->drawFlatAlpha(positionAvatar.x, positionAvatar.y, alpha);
    background->drawFlatAlpha(positionBackground.x, positionBackground.y, alpha);

    Point2D positionFrom(positionBackground.x + 275, positionBackground.y + 30);
    Point2D positionText(positionBackground.x + 275, positionBackground.y + 55);

    std::istringstream stream(message);
    std::string line;
    int lineHeight = 15;

    writer->setAlpha(alpha);
    writer->writeTextTTFAutoSize(positionFrom.x, positionFrom.y, from.c_str(), PaletteColors::getCrt(), 0.25f);

    while (std::getline(stream, line)) {
        writer->writeTextTTFAutoSize(positionText.x, positionText.y, line.c_str(), PaletteColors::getEnemyMessage(), 0.15f);
        positionText.y += lineHeight;
    }
}

bool EnemyDialog::isShowed() const {
    return showed;
}

void EnemyDialog::setShowed(bool showed) {
    EnemyDialog::showed = showed;
}

void EnemyDialog::start()
{
    ComponentsManager::get()->getComponentSound()->sound("radioBeep", EngineSetup::SoundChannels::SND_GLOBAL, 0);
    ComponentsManager::get()->getComponentSound()->sound(sound, EngineSetup::SoundChannels::SND_GLOBAL, 0);
    counterTTL.setEnabled(true);
}

void EnemyDialog::updateOffsets()
{
    float percentage = counterTTL.currentPercentage() / 100;

    const float percentageFadeIn = 0.20;
    const float percentageFadeOut = 0.80;

    if (percentage <= percentageFadeIn) {
        smoothEffect = sin(M_PI / 2.0 * (percentage / percentageFadeIn));
    } else if (percentage <= percentageFadeOut) {
        smoothEffect = 1.0;
    } else {
        smoothEffect = sin(M_PI / 2.0 * (1.0 - (percentage - percentageFadeOut) / percentageFadeIn));
    }
}
