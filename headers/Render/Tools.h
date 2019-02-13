#ifndef SDL2_3D_ENGINE_TOOLS_H
#define SDL2_3D_ENGINE_TOOLS_H

#include "../Objects/Vertex3D.h"
#include "../Objects/Point2D.h"
#include "../Objects/Camera3D.h"
#include "Frustum.h"
#include "../Objects/Object3D.h"
#include "../Objects/Vector3D.h"
#include "Texture.h"
#include "../Objects/LightPoint3D.h"
#include "M3.h"
#include <vector>
#include <SDL.h>
#include <SDL_ttf.h>

class Tools {

public:
    static std::vector<std::string> split(const std::string &, char);

    static void SurfacePutPixel(SDL_Surface *surface, int x, int y, Uint32 pixel);

    static const float getXTextureFromUV(SDL_Surface *surface, float u);
    static const float getYTextureFromUV(SDL_Surface *surface, float v);

    static Uint32 readSurfacePixelFromUV(SDL_Surface *surface, float u, float v);
    static Uint32 readSurfacePixelFromBilinearUV(SDL_Surface *surface, float u, float v);
    static Uint32 readSurfacePixel(SDL_Surface *surface, int x, int y);

    static bool fileExists(const std::string &name);

    static bool isPixelInWindow(int x, int y);

    static void writeText(SDL_Renderer *renderer, TTF_Font *font, int x, int y, Uint32 color, std::string text);
    static void writeText3D(SDL_Renderer *renderer, Camera3D *cam, TTF_Font *font, Vertex3D, Uint32 color, std::string text);
    static void getTextAndRect(SDL_Renderer *renderer, int x, int y, char *text, TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect, Uint32);

    static unsigned long createRGB(int r, int g, int b);
    static uint8_t getRedValueFromColor(uint32_t);
    static uint8_t getGreenValueFromColor(uint32_t);
    static uint8_t getBlueValueFromColor(uint32_t);

    static float interpolate(float val, float bound_left, float bound_right);
    static bool getBit(unsigned char byte, int position);

    static float clamp(float n, float lower, float upper);


};

#endif //SDL2_3D_ENGINE_TOOLS_H
