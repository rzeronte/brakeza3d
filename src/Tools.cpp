//
// Created by darkhead on 28/4/18.
//

#include <string>
#include <vector>
#include <SDL_system.h>
#include <SDL_ttf.h>
#include <iostream>
#include <algorithm>
#include <math.h>
#include <cmath>
#include "../headers/Tools.h"
#include "../headers/EngineSetup.h"
#include "../headers/Line2D.h"
#include "../headers/EngineBuffers.h"
#include "../headers/Vector3D.h"
#include "../headers/Triangle.h"
#include "../headers/M3.h"
#include "../headers/Engine.h"
#include "../headers/Render.h"
#include "../headers/Drawable.h"
#include "../headers/Core/Logging.h"

float Tools::degreesToRadians(float angleDegrees)
{
    float rads;

    rads = angleDegrees * (float) M_PI / (float) 180.0;

    return rads;
}

float Tools::radiansToDegrees(float angleRadians)
{
    float degrees;

    degrees = angleRadians * (float) 180.0 / (float) M_PI;

    return degrees;
}

std::vector<std::string> Tools::split(const std::string &text, char sep) {
    std::vector<std::string> tokens;
    std::size_t start = 0, end = 0;
    while ((end = text.find(sep, start)) != std::string::npos) {
        tokens.push_back(text.substr(start, end - start));
        start = end + 1;
    }
    tokens.push_back(text.substr(start));

    return tokens;
}


void Tools::SurfacePutPixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    if (isPixelInWindow(x, y)) {
        //Convert the pixels to 32 bit
        Uint32 *pixels = (Uint32 *)surface->pixels;

        //Set the pixel
        pixels[ ( y * surface->w ) + x ] = pixel;
        //EngineBuffers::getInstance()->pixelesDrawed++;
    }
}

bool Tools::isPixelInWindow(int x, int y)
{
    if ( !(x >= 0 && x < EngineSetup::getInstance()->SCREEN_WIDTH && y >= 0 && y < EngineSetup::getInstance()->SCREEN_HEIGHT) ) {
        return false;
    }

    return true;
}

unsigned long Tools::createRGB(int r, int g, int b)
{
    return ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
}

uint8_t Tools::getRedValueFromColor(uint32_t c) {
    return ((uint32_t)c >> 16);
}

uint8_t Tools::getGreenValueFromColor(uint32_t c) {
    return ((uint16_t)c >> 8);
}

uint8_t Tools::getBlueValueFromColor(uint32_t c) {
    return (c);
}

int Tools::getXTextureFromUV(SDL_Surface *surface, float u) {
    return surface->w * u;
}

int Tools::getYTextureFromUV(SDL_Surface *surface, float v) {
    return surface->h * v;
}

Uint32 Tools::readSurfacePixelFromUV(SDL_Surface *surface, float u, float v)
{
    int x = Tools::getXTextureFromUV(surface, u);
    int y = Tools::getYTextureFromUV(surface, v);

    return Tools::readSurfacePixel(surface, x, y);
}

Uint32 Tools::readSurfacePixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
        case 1:
            return *p;
            break;

        case 2:
            return *(Uint16 *)p;
            break;

        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;
            break;

        case 4:
            return *(Uint32 *)p;
            break;

        default:
            return 0;
    }
}

bool Tools::fileExists(const std::string& name)
{
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    }

    Logging::getInstance()->Log("File " + name  + " not found", "ERROR");

    return false;
}

void Tools::getTextAndRect(SDL_Renderer *renderer, int x, int y, char *text, TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect)
{
    int text_width;
    int text_height;

    SDL_Surface *surface;
    SDL_Color textColor = {255, 255, 255, 0};

    surface = TTF_RenderText_Solid(font, text, textColor);
    *texture = SDL_CreateTextureFromSurface(renderer, surface);

    text_width = surface->w;
    text_height = surface->h;
    SDL_FreeSurface(surface);

    rect->x = x;
    rect->y = y;
    rect->w = text_width;
    rect->h = text_height;
}

void Tools::writeText(SDL_Renderer *renderer, TTF_Font *font, int x, int y, Uint32 color, std::string text)
{
    SDL_Texture *textTexture;
    SDL_Rect textRect;
    Tools::getTextAndRect(renderer, x, y, const_cast<char *>(text.c_str()), font, &textTexture, &textRect);
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
}


void Tools::sortVertexByY(Vertex &A, Vertex &B, Vertex &C)
{
    int n = 3;
    Vertex v[3];
    v[0] = A; v[1] = B; v[2] = C;

    for (int i = 1 ; i < n; i++) {
        for (int j = 0 ; j < (n - i); j++) {
            if (v[j].y > v[j+1].y) {
                Vertex aux = v[j];
                v[j] = v[j+1];
                v[j+1] = aux;
            }
        }
    }

    A = v[0]; B = v[1]; C = v[2];

    return;
}

void Tools::sortVertexByX(Vertex &A, Vertex &B, Vertex &C)
{
    int n = 3;
    Vertex v[3];
    v[0] = A; v[1] = B; v[2] = C;

    for (int i = 1 ; i < n; i++) {
        for (int j = 0 ; j < (n - i); j++) {
            if (v[j].x > v[j+1].x) {
                Vertex aux = v[j];
                v[j] = v[j+1];
                v[j+1] = aux;
            }
        }
    }

    A = v[0]; B = v[1]; C = v[2];

    return;
}

void Tools::sortPointsByY(Point2D &A, Point2D &B, Point2D &C)
{
    int n = 3;
    Point2D p[3];
    p[0] = A; p[1] = B; p[2] = C;


    for (int i = 1 ; i < n ; i++) {
        for (int j = 0 ; j < (n - i) ; j++) {
            if (p[j].y > p[j+1].y) {
                Point2D aux = p[j];
                p[j] = p[j+1];
                p[j+1] = aux;
            }
        }
    }

    A = p[0]; B = p[1]; C = p[2];

    return;
}

void Tools::sortPointsByX(Point2D &A, Point2D &B, Point2D &C)
{
    int n = 3;
    Point2D p[3];
    p[0] = A; p[1] = B; p[2] = C;

    for (int i = 1 ; i < n ; i++) {
        for (int j = 0 ; j < (n - i) ; j++) {
            if (p[j].x > p[j+1].x) {
                Point2D aux = p[j];
                p[j] = p[j+1];
                p[j+1] = aux;
            }
        }
    }

    A = p[0]; B = p[1]; C = p[2];
}

void Tools::VertexSwap(Vertex vertexes[], int i, int j)
{
    Vertex tmp = vertexes[i];
    vertexes[i] = vertexes[j];
    vertexes[j] = tmp;
}

void Tools::sortVertexesByX(Vertex vertexes[], int N)
{
    int i, j, k;
    for (i = 0; i < N - 1; i++)
    {
        for (k = i, j = i + 1; j < N; j++) {
            if (vertexes[j].x < vertexes[k].x) {
                k = j;
            }
        }
        Tools::VertexSwap (vertexes, i, k);
    }
}

void Tools::sortVertexesByAngles(Vertex vertexes[], int N)
{
    int i, j, k;

    for (i = 0; i < N - 1; i++) {
        for (k = i, j = i + 1; j < N; j++) {
            if (vertexes[j].angle < vertexes[k].angle) {
                k = j;
            }
        }
        Tools::VertexSwap (vertexes, i, k);
    }
}

void Tools::sortVertexesByY(Vertex vertexes[], int N)
{
    int i, j, k;
    for (i = 0; i < N - 1; i++)
    {
        for (k = i, j = i + 1; j < N; j++) {
            if (vertexes[j].x < vertexes[k].x) {
                k = j;
            }
        }
        Tools::VertexSwap (vertexes, i, k);
    }
}

Vertex Tools::getCenterVertices(Vertex vertices[], int num_vertices) {
    Vertex middle = Vertex(0, 0, 0);

    for (int i = 0; i < num_vertices; i++) {
        middle.x += vertices[i].x;
        middle.y += vertices[i].y;
        middle.z += vertices[i].z;
    }

    middle.x/= num_vertices;
    middle.y/= num_vertices;
    middle.z/= num_vertices;

    return middle;
}

float Tools::distanteBetweenpoints(Vertex v1, Vertex v2)
{

    float abs_vector = sqrtf( (v2.x - v1.x)*(v2.x - v1.x) + (v2.y - v1.y)*(v2.y - v1.y) + (v2.z - v1.z)*(v2.z - v1.z) );

    return abs_vector;
}

Uint32 Tools::mixColor(Uint32 color, float distance, LightPoint *lp, Vertex Q)
{

    Vertex P = lp->position;
    Vertex R = lp->forward.getUnitVector();

    Vector3D L = Vector3D(P, Q);
    Vertex Lv = L.getUnitVector();

    const float min = Vertex::dotProduct(R, Lv);

    float p = 100;
    float max = fmaxf(min, 0);
    float pow = powf(max, p);

    float intensity = pow / (lp->kc + lp->kl*distance + lp->kq * (distance * distance));

    int r_light = (int) (Tools::getRedValueFromColor(lp->color)   * intensity);
    int g_light = (int) (Tools::getGreenValueFromColor(lp->color) * intensity);
    int b_light = (int) (Tools::getBlueValueFromColor(lp->color)  * intensity);

    int r_original = (int) (Tools::getRedValueFromColor(color) * ( 1 - intensity) );
    int g_original = (int) (Tools::getGreenValueFromColor(color) * ( 1 - intensity) );
    int b_original = (int) (Tools::getBlueValueFromColor(color) * ( 1 - intensity) );

    Uint32 c = Tools::createRGB(
        r_light + r_original,
        g_light + g_original,
        b_light + b_original
    );

    return c;
}

Vertex Tools::crossProduct(Vertex u, Vertex v)
{
    Vertex V;

    V.x = (u.y * v.z) - (u.z * v.y);
    V.y = (u.z * v.x) - (u.x * v.z);
    V.z = (u.x * v.y) - (u.y * v.x);

    return V;
}

float Tools::floatRound(double f, int c)
{
    return (((float)((int)((f) * (c))) / (c)));
}

float Tools::getHorizontalAngleBetweenObject3DAndCamera(Object3D *o1, Camera *cam)
{
    o1->updateAxis();

    Vertex oRight = o1->right.getUnitVector();
    Vertex R = cam->rightVector();

    oRight.consoleInfo("oR", true);

    float rads = acosf( Vertex::dotProduct(R, oRight) / ( R.getNorm() * oRight.getNorm() ) );
    float degs = Tools::radiansToDegrees(rads);

    return degs;
}
