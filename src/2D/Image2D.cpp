//
// Created by Eduardo on 12/01/2025.
//

#include "../../include/2D/Image2D.h"
#include "../../include/Components/Components.h"
#include "../../include/GUI/Objects/Image2DGUI.h"

Image2D::Image2D(const std::string &file, int width, int height)
:
    width(width),
    height(height),
    filepath(file)
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

    image->DrawFlatAlpha(x, y, width, height, alpha, Components::get()->Window()->getForegroundFramebuffer());
}

void Image2D::setSize(int w, int h)
{
    this->width = w;
    this->height = h;
}

void Image2D::setScreenPosition(int x, int y)
{
    this->x = x;
    this->y = y;
}

void Image2D::setFilePath(const std::string &filepath)
{
    this->filepath = filepath;
}

void Image2D::DrawPropertiesGUI()
{
    Object3D::DrawPropertiesGUI();
    Image2DGUI::DrawPropertiesGUI(this);
}

void Image2D::setImage(Image *value)
{
    image = value;
}