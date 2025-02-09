#ifndef SDL2_3D_ENGINE_COLOR_H
#define SDL2_3D_ENGINE_COLOR_H

#include <vector>
#include <cstdint>
#include <glm/vec3.hpp>
#include <SDL2/SDL.h>
#include "imgui.h"

class Color {
public:
    float r;
    float g;
    float b;
    float a;

    Color();
    Color(float r, float g, float b, float a = 0);
    explicit Color(uint32_t v);

    Color operator+(const Color &pm) const;
    Color operator-(const Color &pm) const;
    bool operator==(const Color &pm) const;
    Color operator*(const Color &pm) const;
    Color operator*(float s) const;
    Color operator/(float s) const;

    static Color white() { return {1, 1, 1}; }
    static Color red() { return {1, 0, 0}; }
    static Color green() { return {0, 1, 0}; }
    static Color blue() { return {0, 0, 1}; }
    static Color black() { return {0, 0, 0}; }
    static Color yellow() { return {1, 1, 0}; }
    static Color fuchsia() { return {1, 0, 1}; }
    static Color cyan() { return {0, 1, 1}; }
    static Color orange() { return {1, 0.5f, 0.2f}; }
    static Color olive() { return {0.5f, 0.5f, 0}; }
    static Color gray() { return {0.5f, 0.5f, 0.5f}; }
    static Color pink() { return {1, 0.41f, 0.71f}; }  // Rosa
    static Color lime() { return {0.75f, 1, 0}; }  // Verde lima
    static Color turquoise() { return {0.25f, 0.88f, 0.82f}; }  // Turquesa
    static Color violet() { return {0.93f, 0.51f, 0.93f}; }  // Violeta
    static Color gold() { return {1, 0.84f, 0}; }  // Dorado
    static Color indigo() { return {0.29f, 0, 0.51f}; }  // Índigo
    static Color magenta() { return {1, 0, 0.56f}; }  // Magenta
    static Color aquamarine() { return {0.5f, 1, 0.83f}; }  // Aguamarina
    static Color crimson() { return {0.86f, 0.08f, 0.24f}; }  // Carmesí
    static Color FOGDefault() { return {0.5f, 0.5f, 0.5f}; }
    static Color mixColor(Color &c1, Color &c2, float c2Intensity);

    void setRed(float v);
    void setGreen(float v);
    void setBlue(float v);

    uint32_t getColor() const;

    float getA() const;

    void setAlpha(float a);

    uint32_t color;

    [[nodiscard]] glm::vec3 toGLM() const;

    [[nodiscard]] SDL_Color toSDL() const;

    ImVec4 toImVec4() const;
};


#endif //SDL2_3D_ENGINE_COLOR_H
