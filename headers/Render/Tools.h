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
#include <SDL_mixer.h>

#define PLANE_BACKSIDE 0x000001
#define PLANE_FRONT    0x000010
#define ON_PLANE       0x000100

class Tools {

public:
    static std::vector<std::string> split(const std::string &, char);

    static void SurfacePutPixel(SDL_Surface *surface, int x, int y, Uint32 pixel);

    static const float getXTextureFromUV(SDL_Surface *surface, float u);
    static const float getYTextureFromUV(SDL_Surface *surface, float v);

    static Uint32 readSurfacePixelFromUV(SDL_Surface *surface, float &u, float &v);
    static Uint32 readSurfacePixelFromBilinearUV(SDL_Surface *surface, float u, float v);
    static Uint32 readSurfacePixel(SDL_Surface *surface, int x, int y);

    static bool fileExists(const std::string &name);
    static char* readFile(const std::string &name, size_t &source_size);

    static bool isPixelInWindow(int x, int y);

    static void writeText(SDL_Renderer *renderer, TTF_Font *font, int x, int y, Uint32 color, std::string text);
    static void writeText3D(SDL_Renderer *renderer, Camera3D *cam, TTF_Font *font, Vertex3D, Uint32 color, std::string text);
    static void writeTextCenter(SDL_Renderer *renderer, TTF_Font *font, Uint32 color, std::string text);
    static void writeTextCenterHorizontal(SDL_Renderer *renderer, TTF_Font *font, Uint32 color, std::string text, int ypos);

    static void getTextAndRect(SDL_Renderer *renderer, int x, int y, char *text, TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect, Uint32);
    static void getTextAndRectCenter(SDL_Renderer *renderer, char *text, TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect, Uint32 color);
    static void getTextAndRectCenterHorizontal(SDL_Renderer *renderer, char *text, TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect, Uint32 color, int ypos);

    static unsigned long createRGB(int r, int g, int b);

    static uint8_t getRedValueFromColor(uint32_t);
    static uint8_t getGreenValueFromColor(uint32_t);
    static uint8_t getBlueValueFromColor(uint32_t);

    static float interpolate(float val, float bound_left, float bound_right);
    static bool getBit(unsigned char byte, int position);

    static float clamp(float n, float lower, float upper);
    static int random(int min, int max); //range : [min, max)
    static Vertex3D wedge(Vertex3D v1, Vertex3D v2);
    static int classifyPoint(Vertex3D point, Vertex3D pO, Vertex3D pN);

    static bool isZeroVector(Vertex3D& v);
    static bool isValidVector(Vertex3D& v);
    static bool checkRectangleAABBOverlap(Point2D l1, Point2D r1, Point2D l2, Point2D r2);
    static std::string floatTruncate(float val, int numDigits);
    static void playMixedSound(Mix_Chunk *chunk, int channel, int times);

    static Uint32 mixLightColor(Uint32 color, float distance, LightPoint3D *lp, Vertex3D Q);
    static Uint32 alphaBlend(Uint32 color1, Uint32 color2, Uint32 alpha);
    static Uint32 mixColor(Uint32 color1, Uint32 color2, float color2Intensity);

};

#endif //SDL2_3D_ENGINE_TOOLS_H
