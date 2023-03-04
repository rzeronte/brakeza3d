#ifndef SDL2_3D_ENGINE_COLOR_H
#define SDL2_3D_ENGINE_COLOR_H

#include <vector>
#include <cstdint>
#include "ColorHSV.h"

class Color {
public:
    float r;
    float g;
    float b;
    float a;

    Color();
    Color(int r, int g, int b, int a = 0);
    explicit Color(uint32_t v);

    Color operator+(const Color &pm) const;
    Color operator-(const Color &pm) const;
    bool operator==(const Color &pm) const;
    Color operator*(const Color &pm) const;
    Color operator*(float s) const;
    Color operator/(float s) const;

    static Color white() { return {255, 255, 255}; }
    static Color red() { return {255, 0, 0}; }
    static Color green() { return {0, 255, 0}; }
    static Color blue() { return {0, 0, 255}; }
    static Color black() { return {0, 0, 0}; }
    static Color yellow() { return {255, 255, 0}; }
    static Color fuchsia() { return {255, 0, 255}; }
    static Color cyan() { return {0, 255, 255}; }
    static Color orange() { return {100, 82, 42}; }
    static Color olive() { return {128, 128, 0}; }
    static Color gray() { return {128, 128, 128}; }
    static Color FOGDefault() { return {128, 128, 128}; }
    static Color mixColor(Color &c1, Color &c2, float c2Intensity);

    void setRed(float v);
    void setGreen(float v);
    void setBlue(float v);

    uint32_t getColor() const;

    unsigned int getA() const;

    void setAlpha(unsigned int a);

    uint32_t color;

};


#endif //SDL2_3D_ENGINE_COLOR_H
