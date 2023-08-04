//
// Created by eduardo on 31/05/23.
//

#include <sstream>
#include "EnemyDialog.h"

#include "../../include/ComponentsManager.h"

EnemyDialog::EnemyDialog(Object3D *parent, Image *avatar, Image * avatarSmall, float staminaPercentage, const char *message, const char *from, TTF_Font *font, Color color)
:
    showed(false),
    parent(parent),
    message(message),
    from(from),
    background(new Image(EngineSetup::get()->IMAGES_FOLDER + "radio_notification.png")),
    color(color),
    avatar(avatar),
    avatarSmall(avatarSmall),
    staminaPercentage(staminaPercentage)
{
    writer = new TextWriter(ComponentsManager::get()->getComponentWindow()->getRenderer(), font);
}

void EnemyDialog::update()
{
    drawDialog(255, avatar);
}

void EnemyDialog::drawDialog(float alpha, Image *avatar)
{
    Point2D positionAvatar(137, 142);
    Point2D positionAvatarSmall(208, 183);
    Point2D positionText(137, 240);
    Point2D positionFrom(208, 160);

    background->drawFlatAlpha(0, 0, alpha);
    avatar->drawFlatAlpha(positionAvatar.x, positionAvatar.y, alpha);
    avatarSmall->drawFlatAlpha(positionAvatarSmall.x, positionAvatarSmall.y, alpha);

    std::istringstream stream(message);
    std::string line;
    int lineHeight = 15;

    writer->setAlpha(alpha);
    writer->writeTextTTFAutoSize(positionFrom.x, positionFrom.y, from.c_str(), color, 0.20f);

    while (std::getline(stream, line)) {
        writer->writeTextTTFAutoSize(positionText.x, positionText.y, line.c_str(), color, 0.20f);
        positionText.y += lineHeight;
    }
}

bool EnemyDialog::isShowed() const {
    return showed;
}

void EnemyDialog::setShowed(bool showed) {
    EnemyDialog::showed = showed;
}
