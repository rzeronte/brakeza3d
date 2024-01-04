
#include <SDL_rect.h>
#include "../../include/Misc/Color.h"
#include "imgui.h"

Color::Color(): r(0), g(0), b(0), a(0), color(0)
{
}

Color::Color(float r, float g, float b, float a): r(r), g(g), b(b), a(a), color(0)
{
    auto s = (unsigned char*) &this->color;

    s[0] = r;
    s[1] = g;
    s[2] = b;
    s[3] = 0;
}

Color::Color(uint32_t v): a(0)
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

float Color::getA() const {
    return a;
}

void Color::setAlpha(float value) {
    Color::a = value;
}

glm::vec3 Color::toGLM() const
{
    return {r, g, b};
}

SDL_Color Color::toSDL() const
{
    return  {(Uint8) (b * 255), (Uint8) (g * 255), (Uint8) (r * 255)};
}

ImVec4 Color::toImVec4() const
{
    return ImVec4 {r, g, b, a};
}