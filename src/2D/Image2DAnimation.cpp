//
// Created by eduardo on 13/08/23.
//

#include "../../include/2D/Image2DAnimation.h"
#include "../../include/Components/ComponentsManager.h"
#include "../../include/Brakeza.h"
#include "../../include/GUI/Objects/Image2DAnimationGUI.h"

Image2DAnimation::Image2DAnimation(int x, int y, bool removeWhenEnds, TextureAnimated *animation)
:
    x(x),
    y(y),
    removeWhenEnds(removeWhenEnds),
    animation(animation),
    ttl(nullptr)
{
    featuresGUI.position = false;
    featuresGUI.rotation = false;
    featuresGUI.scale = false;
    featuresGUI.collider = false;
}

Image2DAnimation::Image2DAnimation(int x, int y, float ttl, TextureAnimated *animation)
    :
    x(x),
    y(y),
    removeWhenEnds(false),
    animation(animation),
    ttl(new Counter(ttl))
{
    this->ttl->setEnabled(true);
}

void Image2DAnimation::onUpdate()
{
    return;
    animation->update();

    Object3D::onUpdate();

    float alpha = 255;

    if (ttl != nullptr) {
        ttl->update();
        const float ttlPercentage = ttl->currentPercentage();

        if (ttlPercentage <= 15.0f) {
            alpha = (ttlPercentage / 15.0f) * 255;
        } else if (ttlPercentage >= 80.0f) {
            alpha = 255 - (ttlPercentage - 80.0f) / 20.0f * 255;
        }

        if (ttl->isFinished()) {
            setRemoved(true);
        }
    }

    const auto w = animation->getCurrentFrame()->width();
    const auto h = animation->getCurrentFrame()->height();

    animation->getCurrentFrame()->DrawFlatAlpha(
        x - w/2,
        y - h/2,
        alpha/255,
        ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer()
    );

    if (removeWhenEnds && animation->isEndAnimation()) {
        this->setRemoved(true);
    }
}

TextureAnimated *Image2DAnimation::getAnimation() const
{
    return animation;
}

void Image2DAnimation::updatePosition(int x, int y)
{
    this->x = x;
    this->y = y;
}

const char *Image2DAnimation::getTypeObject()
{
    return SceneObjectTypes::IMAGE_2D_ANIMATION;
}

const char *Image2DAnimation::getTypeIcon()
{
    return IconsByObject::IMAGE_2D_ANIMATION;
}

void Image2DAnimation::DrawPropertiesGUI()
{
    Object3D::DrawPropertiesGUI();
    Image2DAnimationGUI::DrawPropertiesGUI(this);
}

Image2DAnimation *Image2DAnimation::create(int x, int y, const std::string &sheet, int w, int h, int frames, int fps)
{
    auto *o = new Image2DAnimation(
        x,
        y,
        false,
        new TextureAnimated(sheet,w,h, frames,fps)
    );

    o->setBelongToScene(true);

    return o;
}

int Image2DAnimation::getX() const
{
    return x;
}

int Image2DAnimation::getY() const
{
    return y;
}
