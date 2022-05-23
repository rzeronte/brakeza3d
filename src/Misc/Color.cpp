
#include <cmath>
#include "../../include/Misc/Color.h"

Color::Color()
{
    this->r = 0;
    this->g = 0;
    this->b = 0;
    this->a = 0;

    //this->color = (b << 16) + (g << 8) + (r);
}

Color::Color(int r, int g, int b, int a)
{
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;

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

uint32_t Color::getColor() const
{
    return this->color;
}

Color Color::operator*(float s) const
{
    return Color(r * s, g * s, b * s);
}

Color Color::operator/(float s) const
{
    return Color(this->r / s, this->g / s, this->b / s);
}


Color Color::mixColor(Color &c1, Color &c2, float c2Intensity)
{
    return (c1 * (1.0f - c2Intensity)) + (c2 * c2Intensity);
}

unsigned int Color::getA() const {
    return a;
}

void Color::setAlpha(unsigned int a) {
    Color::a = a;
}