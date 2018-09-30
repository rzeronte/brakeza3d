#ifndef SDL2_3D_ENGINE_TOOLS_H
#define SDL2_3D_ENGINE_TOOLS_H

#include "Vertex.h"
#include "Point2D.h"
#include "Camera.h"
#include "Frustum.h"
#include "Object3D.h"
#include "Vector3D.h"
#include "Texture.h"
#include "LightPoint.h"
#include <vector>
#include <SDL.h>
#include <SDL_ttf.h>

class Tools {

public:
    static float degreesToRadians(float);
    static float radiansToDegrees(float);

    static std::vector<std::string> split(const std::string &, char);

    static void SurfacePutPixel(SDL_Surface *surface, int x, int y, Uint32 pixel);

    static unsigned long createRGB(int r, int g, int b);

    static int getXTextureFromUV(SDL_Surface *surface, float u);
    static int getYTextureFromUV(SDL_Surface *surface, float v);

    static Uint32 readSurfacePixelFromUV(SDL_Surface *surface, float u, float v);
    static Uint32 readSurfacePixel(SDL_Surface *surface, int x, int y);

    static bool fileExists(const std::string &name);

    static bool isPixelInWindow(int x, int y);

    static void getTextAndRect(SDL_Renderer *renderer, int x, int y, char *text, TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect);

    static void writeText(SDL_Renderer *renderer, TTF_Font *font, int x, int y, Uint32 color, std::string text);

    static void sortVertexesByX(Vertex vertexes[], int N);
    static void sortVertexesByY(Vertex vertexes[], int N);

    static void sortVertexByY(Vertex &A, Vertex &B, Vertex &C);
    static void sortVertexByX(Vertex &A, Vertex &B, Vertex &C);

    static void sortVertexesByAngles(Vertex *vertexes, int N);

    static void sortPointsByY(Point2D &A, Point2D &B, Point2D &C);
    static void sortPointsByX(Point2D &A, Point2D &B, Point2D &C);

    static void VertexSwap(Vertex *vertexes, int i, int j);

    static Vertex getCenterVertices(Vertex *vertices, int num_vertices);

    static float distanteBetweenpoints(Vertex v1, Vertex v2);

    static Uint32 mixColor(Uint32 color, float distance, LightPoint *lp, Vertex Q);

    static uint8_t getRedValueFromColor(uint32_t);
    static uint8_t getGreenValueFromColor(uint32_t);
    static uint8_t getBlueValueFromColor(uint32_t);

    static Vertex crossProduct(Vertex, Vertex);

    static float floatRound(double dval, int n);
};

#endif //SDL2_3D_ENGINE_TOOLS_H
