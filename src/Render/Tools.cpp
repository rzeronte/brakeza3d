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
#include "../../headers/Render/Tools.h"
#include "../../headers/Render/EngineSetup.h"
#include "../../headers/Objects/Line2D.h"
#include "../../headers/Render/EngineBuffers.h"
#include "../../headers/Objects/Vector3D.h"
#include "../../headers/Objects/Triangle3D.h"
#include "../../headers/Render/M3.h"
#include "../../headers/Render/Engine.h"
#include "../../headers/Render/Transforms.h"
#include "../../headers/Render/Drawable.h"
#include "../../headers/Render/Logging.h"

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

void Tools::getTextAndRect(SDL_Renderer *renderer, int x, int y, char *text, TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect, Uint32 color)
{
    int text_width;
    int text_height;

    SDL_Surface *surface;
    SDL_Color textColor = {(Uint8) Tools::getRedValueFromColor(color), (Uint8) Tools::getGreenValueFromColor(color), (Uint8) Tools::getBlueValueFromColor(color), 0};

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
    Tools::getTextAndRect(renderer, x, y, const_cast<char *>(text.c_str()), font, &textTexture, &textRect, color);

    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
}

void Tools::writeText3D(SDL_Renderer *renderer, Camera3D *cam, TTF_Font *font, Vertex3D v, Uint32 color, std::string text)
{
    Vertex3D tmpV;
    tmpV = Transforms::cameraSpace( v, cam );
    tmpV = Transforms::homogeneousClipSpace(tmpV, cam);
    Point2D text_point = Transforms::screenSpace(tmpV, cam);

    // Las coordenadas que debemos darle, dependen del tamaño de la ventana, ya que el renderer
    // se encuentra sampleado desde el tamaño configurado en EngineSetup.

    int renderer_w, renderer_h;
    SDL_GetRendererOutputSize(renderer, &renderer_w, &renderer_h);

    int real_x = (int) (text_point.x * renderer_w / EngineSetup::getInstance()->SCREEN_WIDTH);
    int real_y = (int) (text_point.y * renderer_h / EngineSetup::getInstance()->SCREEN_HEIGHT) ;

    real_x += 5; // Offset estético

    Tools::writeText(renderer, font, real_x, real_y, color, text);
}
