//
// Created by eduardo on 13/08/23.
//

#include "../../include/2D/Sprite2D.h"

Sprite2D::Sprite2D(int x, int y, bool removeWhenEnds, TextureAnimated *animation)
:
    x(x),
    y(y),
    removeWhenEnds(removeWhenEnds),
    animation(animation),
    ttl(nullptr)
{
}

Sprite2D::Sprite2D(int x, int y, float ttl, TextureAnimated *animation)
    :
    x(x),
    y(y),
    removeWhenEnds(false),
    animation(animation),
    ttl(new Counter(ttl))
{
    this->ttl->setEnabled(true);
}

void Sprite2D::onUpdate()
{
    animation->update();

    Object3D::onUpdate();

    float alpha = 255;

    if (ttl != nullptr) {
        ttl->update();
        const float ttlPercentage = ttl->currentPercentage();

        if (ttlPercentage <= 15.0f) {
            alpha = (ttlPercentage / 15.0f) * 255;
        } else if (ttlPercentage >= 80.0f) {
            alpha = 255 - ((ttlPercentage - 80.0f) / 20.0f * 255);
        }

        if (ttl->isFinished()) {
            setRemoved(true);
        }
    }
    const auto w = animation->getCurrentFrame()->width();
    const auto h = animation->getCurrentFrame()->height();


    animation->getCurrentFrame()->drawFlatAlpha(x - w/2, y - h/2, alpha);

    if (removeWhenEnds && animation->isEndAnimation()) {
        this->setRemoved(true);
    }
}

TextureAnimated *Sprite2D::getAnimation() const
{
    return animation;
}

void Sprite2D::updatePosition(int x, int y)
{
    this->x = x;
    this->y = y;
}

const char *Sprite2D::getTypeObject() {
    return "Sprite2D";
}

const char *Sprite2D::getTypeIcon() {
    return "spriteIcon";
}
