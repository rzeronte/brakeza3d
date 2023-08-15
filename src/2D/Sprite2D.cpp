//
// Created by eduardo on 13/08/23.
//

#include <utility>

#include "../../include/2D/Sprite2D.h"

Sprite2D::Sprite2D(int x, int y, bool removeWhenEnds, TextureAnimated *animation)
:
    x(x),
    y(y),
    removeWhenEnds(removeWhenEnds),
    animation(animation)
{
}

void Sprite2D::onUpdate()
{
    animation->update();

    Object3D::onUpdate();

    const auto w = animation->getCurrentFrame()->width();
    const auto h = animation->getCurrentFrame()->height();

    animation->getCurrentFrame()->drawFlatAlpha(x - w/2, y - h/2, 255);

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