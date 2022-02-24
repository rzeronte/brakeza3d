#ifndef SDL2_3D_ENGINE_COLOR_H
#define SDL2_3D_ENGINE_COLOR_H

#include <vector>
#include <cstdint>

class Color {
public:
    int r;
    int g;
    int b;

    Color();
    Color(int r, int g, int b);
    Color(unsigned long v);

    Color operator+(const Color &pm) const;
    Color operator-(const Color &pm) const;
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
    static Color orange() { return Color(0, 0, 0); }
    static Color olive() { return Color(128, 128, 0); }
    static Color gray() { return Color(128, 128, 128); }
    static Color FOGDefault() { return Color::black(); }
    static Color mixColor(Color &c1, Color &c2, float c2Intensity);

    void setRed(float v);
    void setGreen(float v);
    void setBlue(float v);

    uint32_t getColor();
    uint32_t createRGB(int r, int g, int b);

    uint8_t getRedValueFromColor(uint32_t);
    uint8_t getGreenValueFromColor(uint32_t);
    uint8_t getBlueValueFromColor(uint32_t);

private:

    uint32_t color;
};


#endif //SDL2_3D_ENGINE_COLOR_H
