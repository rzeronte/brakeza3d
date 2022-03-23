
#include <SDL_events.h>
#include "../../include/Misc/Color.h"

Color::Color() {

}
Color::Color(int r, int g, int b)
{
    this->r = r;
    this->g = g;
    this->b = b;

    this->color = (b << 16) + (g << 8) + (r);
}

Color::Color(uint32_t v)
{
    this->color = v;

    auto *colors = (uint8_t*)&v;

    this->r = colors[0];
    this->g = colors[1];
    this->b = colors[2];
}

void Color::setRed(float v)
{
    this->r = v;
}

void Color::setGreen(float v)
{
    this->g = v;
}

void Color::setBlue(float v)
{
    this->b = v;
}

Color Color::operator+(const Color &pm) const
{
    return Color(this->r + pm.r, this->g + pm.g, this->b + pm.b);
}

Color Color::operator-(const Color &pm) const
{
    return Color(this->r - pm.r, this->g - pm.g, this->b - pm.b);
}

Color Color::operator*(const Color &pm) const
{
    return Color(this->r * pm.r, this->g * pm.g, this->b * pm.b);
}

bool Color::operator==(const Color &pm) const
{
    if (this->r == pm.r && this->g == pm.g && this->b == pm.b) {
        return true;
    }

    return false;
}

uint32_t Color::getColor()
{
    return this->color;
}

Color Color::operator*(float s) const
{
    return Color(this->r * s, this->g * s, this->b * s);
}

Color Color::operator/(float s) const
{
    return Color(this->r / s, this->g / s, this->b / s);
}


Color Color::mixColor(Color &c1, Color &c2, float c2Intensity)
{
    float originIntensity = 1 - c2Intensity;
    return Color(
        (c2.r * c2Intensity) + (c1.r * originIntensity),
        (c2.g * c2Intensity) + (c1.g * originIntensity),
        (c2.b * c2Intensity) + (c1.b * originIntensity)
    );
}
