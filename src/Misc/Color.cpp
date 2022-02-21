
#include "../../include/Misc/Color.h"

Color::Color() {

}
Color::Color(int r, int g, int b)
{
    this->r = r;
    this->g = g;
    this->b = b;

    this->color = this->createRGB(r, g, b);
}

Color::Color(unsigned long v)
{
    this->color = v;

    this->r = this->getRedValueFromColor(this->color);
    this->g = this->getGreenValueFromColor(this->color);
    this->b = this->getBlueValueFromColor(this->color);
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

uint32_t Color::createRGB(int b, int g, int r)
{
    return (unsigned long) ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
}

uint8_t Color::getRedValueFromColor(uint32_t c)
{
    return ((uint32_t) c >> 16);
}

uint8_t Color::getGreenValueFromColor(uint32_t c)
{
    return ((uint16_t) c >> 8);
}

uint8_t Color::getBlueValueFromColor(uint32_t c)
{
    return (c);
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
