#ifndef SDL2_3D_ENGINE_COLOR_H
#define SDL2_3D_ENGINE_COLOR_H

#include <vector>
#include <cstdint>

class Color {
public:
    unsigned int r;
    unsigned int g;
    unsigned int b;
    unsigned int a;

    Color();
    Color(int r, int g, int b, int a = 0);
    Color(uint32_t v);

    Color operator+(const Color &pm) const;
    Color operator-(const Color &pm) const;
    bool operator==(const Color &pm) const;
    Color operator*(const Color &pm) const;
    Color operator*(float s) const;
    Color operator/(float s) const;

    static Color white() { return Color(255, 255, 255); }
    static Color red() { return Color(255, 0, 0); }
    static Color green() { return Color(0, 255, 0); }
    static Color blue() { return Color(0, 0, 255); }
    static Color black() { return Color(0, 0, 0); }
    static Color yellow() { return Color(255, 255, 0); }
    static Color fuchsia() { return Color(255, 0, 255); }
    static Color cyan() { return Color(0, 255, 255); }
    static Color orange() { return Color(100, 82, 42); }
    static Color olive() { return Color(128, 128, 0); }
    static Color gray() { return Color(128, 128, 128); }
    static Color FOGDefault() { return Color::black(); }
    static Color mixColor(Color &c1, Color &c2, float c2Intensity);

    void setRed(float v);
    void setGreen(float v);
    void setBlue(float v);

    uint32_t getColor();

    unsigned int getA() const;

    void setA(unsigned int a);

    uint32_t color;

};


#endif //SDL2_3D_ENGINE_COLOR_H
