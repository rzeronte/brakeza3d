#ifndef SDL2_3D_ENGINE_TOOLS_H
#define SDL2_3D_ENGINE_TOOLS_H

#include "../Objects/Vertex3D.h"
#include "../Objects/Point2D.h"
#include "../Components/Camera3D.h"
#include "../Render/Frustum.h"
#include "../Objects/Object3D.h"
#include "../Objects/Vector3D.h"
#include "../Render/Texture.h"
#include "../Objects/LightPoint3D.h"
#include "../Render/M3.h"
#include "Grid3D.h"
#include "PathFinder.h"
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#define PLANE_BACKSIDE 0x000001
#define PLANE_FRONT    0x000010
#define ON_PLANE       0x000100

typedef float vec3_t[3];

class Tools {

public:
    static std::vector<std::string> split(const std::string &, char);

    static void SurfacePutPixel(SDL_Surface *surface, int x, int y, Uint32 pixel);

    static float getXTextureFromUV(SDL_Surface *surface, float u);

    static float getYTextureFromUV(SDL_Surface *surface, float v);

    static Color readSurfacePixelFromUV(SDL_Surface *surface, float &u, float &v);

    static Color readSurfacePixelFromBilinearUV(SDL_Surface *surface, float u, float v);

    static Color readSurfacePixel(SDL_Surface *surface, int x, int y);

    static bool fileExists(const std::string &name);

    static char *readFile(const std::string &name, size_t &source_size);

    static bool isPixelInWindow(int x, int y);

    static uint8_t getRedValueFromColor(uint32_t);

    static uint8_t getGreenValueFromColor(uint32_t);

    static uint8_t getBlueValueFromColor(uint32_t);

    static float interpolate(float val, float bound_left, float bound_right);

    static bool getBit(unsigned char byte, int position);

    static float clamp(float n, float lower, float upper);

    static int random(int min, int max); //range : [min, max)
    static Vertex3D wedge(Vertex3D v1, Vertex3D v2);

    static int classifyPoint(Vertex3D point, Vertex3D pO, Vertex3D pN);

    static bool isZeroVector(Vertex3D &v);

    static bool isValidVector(Vertex3D &v);

    static bool checkRectangleAABBOverlap(Point2D l1, Point2D r1, Point2D l2, Point2D r2);

    static void playMixedSound(Mix_Chunk *chunk, int channel, int times);

    static Color alphaBlend(Uint32 color1, Uint32 color2, Uint32 alpha);

    static Color mixColor(Color color1, Color color2, float color2Intensity);

    static int int_floor(float x);

    static void consoleVec3(vec3_t v, const std::string&);

    static Uint32 getSurfacePixel(SDL_Surface *surface, int x, int y);

    static void LoadPathFinderWithGrid3D(Grid3D *, PathFinder *);

    static std::vector<Vertex3D> getVerticesFromPathFinderPath(Grid3D *, std::stack<PairData> path);

    static btMatrix3x3 M3ToBulletM3(M3 m);

    static M3 BulletM3ToM3(btMatrix3x3 m);
};

#endif //SDL2_3D_ENGINE_TOOLS_H
