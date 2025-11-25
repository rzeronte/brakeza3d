//
// Created by Eduardo on 12/01/2025.
//

#include "../../include/2D/Image2D.h"
#include "../../include/Components/ComponentsManager.h"
#include "../../include/Brakeza.h"
#include "../../include/GUI/Objects/Image2DGUI.h"

Image2D::Image2D(int x, int y, Image *image)
:
    x(x),
    y(y),
    image(image)
{
    featuresGUI.position = false;
    featuresGUI.rotation = false;
    featuresGUI.scale = false;
    featuresGUI.collider = false;
}

void Image2D::onUpdate()
{
    Object3D::onUpdate();

    if (!image->isLoaded()) return;

    const auto w = image->width();
    const auto h = image->height();

    image->drawFlatAlpha(
        x - w/2,
        y - h/2,
        alpha,
        ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer()
    );
}

void Image2D::updatePosition(int x, int y)
{
    this->x = x;
    this->y = y;
}

const char *Image2D::getTypeObject()
{
    return SceneObjectTypes::IMAGE_2D;
}

const char *Image2D::getTypeIcon()
{
    return IncosByObject::IMAGE_2D;;
}

void Image2D::drawImGuiProperties()
{
    Object3D::drawImGuiProperties();
    Image2DGUI::drawImGuiProperties(this);
}

Image2D *Image2D::create(int x, int y, const std::string& imageFile)
{
    return new Image2D(x, y, new Image(imageFile));
}