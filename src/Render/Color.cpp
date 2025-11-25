
#include "../../include/Render/Color.h"
#include "imgui.h"

Color::Color(): r(0), g(0), b(0), a(0), color(0)
{
}

Color::Color(float r, float g, float b, float a)
:
    r(r), g(g), b(b), a(a), color(0)
{
    auto s = (unsigned char*) &color;

    s[0] = r;
    s[1] = g;
    s[2] = b;
    s[3] = 0;
}

Color::Color(uint32_t v)
:
    a(0),
    color(v)
{
    auto colors = (uint8_t*)&v;

    r = colors[0];
    g = colors[1];
    b = colors[2];
}

void Color::setRed(float value)
{
    r = value;
}

void Color::setGreen(float value)
{
    g = value;
}

void Color::setBlue(float value)
{
    b = value;
}

Color Color::operator+(const Color &pm) const
{
    return Color(r + pm.r, g + pm.g, b + pm.b);
}

Color Color::operator-(const Color &pm) const
{
    return Color(r - pm.r, g - pm.g, b - pm.b);
}

Color Color::operator*(const Color &pm) const
{
    return Color(r * pm.r, g * pm.g, b * pm.b);
}

bool Color::operator==(const Color &pm) const
{
    if (r == pm.r && g == pm.g && b == pm.b) {
        return true;
    }

    return false;
}

uint32_t Color::getColor() const
{
    return color;
}

Color Color::operator*(float s) const
{
    return Color(r * s, g * s, b * s);
}

Color Color::operator/(float s) const
{
    return Color(r / s, g / s, b / s);
}

Color Color::mixColor(Color &c1, Color &c2, float c2Intensity)
{
    return c1 * (1.0f - c2Intensity) + c2 * c2Intensity;
}

float Color::getA() const {
    return a;
}

void Color::setAlpha(float value) {
    a = value;
}

glm::vec3 Color::toGLM() const
{
    return {r, g, b};
}

SDL_Color Color::toSDL() const
{
    return {
        static_cast<Uint8>(b * 255),
        static_cast<Uint8>(g * 255),
        static_cast<Uint8>(r * 255)
    };
}

ImVec4 Color::toImVec4() const
{
    return ImVec4 {r, g, b, a};
}

Color Color::idToColor(const unsigned int id)
{
    unsigned int u = static_cast<unsigned int>(id);

    unsigned char R = (u >> 16) & 0xFF;
    unsigned char G = (u >> 8) & 0xFF;
    unsigned char B = u & 0xFF;

    return Color(
        R / 255.0f,
        G / 255.0f,
        B / 255.0f,
        1.0f
    );
}

int Color::colorToId(const Color &c)
{
    unsigned char R = (unsigned char)(c.r * 255.0f);
    unsigned char G = (unsigned char)(c.g * 255.0f);
    unsigned char B = (unsigned char)(c.b * 255.0f);

    unsigned int id = (R << 16) | (G << 8) | B;
    return static_cast<int>(id);
}