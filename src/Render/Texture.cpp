#include "../../headers/Render/Texture.h"
#include "../../headers/Misc/Tools.h"
#include "../../headers/Misc/Color.h"
#include "../../headers/Render/Logging.h"
#include "../../headers/EngineBuffers.h"
#include "../../headers/Render/Maths.h"
#include <SDL_image.h>

Texture::Texture()
{
    this->mipMapped = false;
    this->lightMapped = false;
}

Texture::Texture(int w, int h)
{
    this->mip_mapping_1 = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
    this->mip_mapping_2 = SDL_CreateRGBSurface(0, w/2, h/2, 32, 0, 0, 0, 0);
    this->mip_mapping_4 = SDL_CreateRGBSurface(0, w/4, h/4, 32, 0, 0, 0, 0);
    this->mip_mapping_8 = SDL_CreateRGBSurface(0, w/8, h/8, 32, 0, 0, 0, 0);
    this->lightmap = SDL_CreateRGBSurface(0, 16, 16, 32, 0, 0, 0, 0);

    this->mipMapped = false;
    this->lightMapped = false;
}

SDL_Surface *Texture::getSurface(int mip_mapping)
{
    if (!isMipMapped()) {
        return this->mip_mapping_1;
    }

    switch(mip_mapping) {
        default:
        case 1:
            return this->mip_mapping_1;
        case 2:
            return this->mip_mapping_2;
        case 4:
            return this->mip_mapping_4;
        case 8:
            return this->mip_mapping_8;
    }
}

void Texture::loadBMP(const char *file, int mip_mapping )
{
    this->filename = file;
    this->loaded = true;

    switch(mip_mapping) {
        default:
        case 1:
            mip_mapping_1 = SDL_LoadBMP_RW(SDL_RWFromFile(file, "rb"),1);
            break;
        case 2:
            mip_mapping_2 = SDL_LoadBMP_RW(SDL_RWFromFile(file, "rb"),1);
            break;
        case 4:
            mip_mapping_4 = SDL_LoadBMP_RW(SDL_RWFromFile(file, "rb"),1);
            break;
        case 8:
            mip_mapping_8 = SDL_LoadBMP_RW(SDL_RWFromFile(file, "rb"),1);
            break;
    }
    //printf("Loading BMP texture '%s' (w: %d, h: %d)\r\n", file, texture_surface->w, texture_surface->h);
}

void Texture::loadJPG(const char *file, int mip_mapping )
{
    this->filename = file;
    this->loaded = true;
    switch(mip_mapping) {
        default:
        case 1:
            mip_mapping_1 = IMG_Load(file);
            break;
        case 2:
            mip_mapping_2 = IMG_Load(file);
            break;
        case 4:
            mip_mapping_4 = IMG_Load(file);
            break;
        case 8:
            mip_mapping_8 = IMG_Load(file);
            break;
    }
    //printf("Loading JPG texture '%s' (w: %d, h: %d)\r\n", file, texture_surface->w, texture_surface->h);
}

bool Texture::loadTGA(const char *file, int mip_mapping )
{
    if (Tools::fileExists(file)) {
        this->filename = file;
        this->loaded = true;
        switch(mip_mapping) {
            default:
            case 1:
                mip_mapping_1 = IMG_Load(file);
                break;
            case 2:
                mip_mapping_2 = IMG_Load(file);
                break;
            case 4:
                mip_mapping_4 = IMG_Load(file);
                break;
            case 8:
                mip_mapping_8 = IMG_Load(file);
                break;
        }
    } else {
        Logging::getInstance()->Log("Error loading TGA texture '" + std::string(file), "TEXTURES");
        return false;
    }

    Logging::getInstance()->Log("Loading TGA texture '" + std::string(file) + "'", "TEXTURES");
    return true;
}

void Texture::drawFlatLightMap(int pos_x, int pos_y)
{
    int width = lightmap->w;
    int height = lightmap->h;

    //Logging::get()->Log("Draw drawFlatLightMap: Width: " + std::to_string(width) + ", Height: " + std::to_string(height), "");

    for (int i = 0 ; i < height ; i++) {
        for (int j = 0 ; j < width ; j++) {
            Uint32 *pixels = (Uint32 *)lightmap->pixels;
            EngineBuffers::getInstance()->setVideoBuffer( j + pos_x, i + pos_y, pixels[i * width + j]);
        }
    }
}

void Texture::drawFlat(int pos_x, int pos_y)
{
    int width = mip_mapping_1->w;
    int height = mip_mapping_1->h;

    //Logging::get()->Log("Draw Texture: Width: " + std::to_string(width) + ", Height: " + std::to_string(height), "");

    for (int i = 0 ; i < height ; i++) {
        for (int j = 0 ; j < width ; j++) {
            Uint32 *pixels = (Uint32 *)mip_mapping_1->pixels;
            EngineBuffers::getInstance()->setVideoBuffer( j + pos_x, i + pos_y, pixels[i * getSurface(1)->w + j]);
        }
    }
}

void Texture::consoleInfo()
{
    Logging::getInstance()->Log("consoleInfo: Texture " + std::to_string(loaded) +  "(w: " + std::to_string(mip_mapping_1->w) + ", h: " + std::to_string(mip_mapping_1->h), "TEXTURE");
}

void Texture::loadLightmapFromRaw(int frame, unsigned int *texture, int w, int h)
{
    SDL_Surface *surf;
    switch(frame) {
        default:
        case 0:
            this->lightmap = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
            surf = this->lightmap;
            break;
        case 1:
            this->lightmap2 = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
            surf = this->lightmap2;
            break;
        case 2:
            this->lightmap3 = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
            surf = this->lightmap3;
            break;
        case 3:
            this->lightmap4 = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
            surf = this->lightmap4;
            break;
    }

    int width  = w;
    int height = h;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Tools::SurfacePutPixel(surf, x, y, texture[(y * width) + x]);
        }
    }
}

void Texture::loadFromRaw(unsigned int *texture, int w, int h, int mip_mapping)
{
    int width;
    int height;

    switch(mip_mapping) {
        default:
        case 1:
            this->mip_mapping_1 = SDL_CreateRGBSurface(0, h, w, 32, 0, 0, 0, 0);

            width = mip_mapping_1->w;
            height = mip_mapping_1->h;

            for (int x = 0; x < width; x++) {
                for (int y = 0; y < height; y++) {
                    Tools::SurfacePutPixel(mip_mapping_1, x, y, texture[y + x * w]);
                }
            }
            break;
        case 2:
            this->mip_mapping_2 = SDL_CreateRGBSurface(0, h, w, 32, 0, 0, 0, 0);

            width = mip_mapping_2->w;
            height = mip_mapping_2->h;

            for (int x = 0; x < width; x++) {
                for (int y = 0; y < height; y++) {
                    Tools::SurfacePutPixel(mip_mapping_2, x, y, texture[y + x * w]);
                }
            }
            break;
        case 4:
            this->mip_mapping_4 = SDL_CreateRGBSurface(0, h, w, 32, 0, 0, 0, 0);

            width = mip_mapping_4->w;
            height = mip_mapping_4->h;

            for (int x = 0; x < width; x++) {
                for (int y = 0; y < height; y++) {
                    Tools::SurfacePutPixel(mip_mapping_4, x, y, texture[y + x * w]);
                }
            }
            break;
        case 8:
            this->mip_mapping_8 = SDL_CreateRGBSurface(0, h, w, 32, 0, 0, 0, 0);

            width = mip_mapping_8->w;
            height = mip_mapping_8->h;

            for (int x = 0; x < width; x++) {
                for (int y = 0; y < height; y++) {
                    Tools::SurfacePutPixel(mip_mapping_8, x, y, texture[y + x * w]);
                }
            }
            break;
    }
}

void Texture::setFilename(const std::string &filename) {
    Texture::filename = filename;
}

const std::string &Texture::getFilename() const {
    return filename;
}

float Texture::getAreaForVertices(Vertex3D A, Vertex3D B, Vertex3D C, int lod)
{
    float tx0 = Tools::getXTextureFromUV(getSurface(lod), A.u/getSurface(lod)->w);
    float ty0 = Tools::getYTextureFromUV(getSurface(lod), A.v/getSurface(lod)->h);

    float tx1 = Tools::getXTextureFromUV(getSurface(lod), B.u/getSurface(lod)->w);
    float ty1 = Tools::getYTextureFromUV(getSurface(lod), B.v/getSurface(lod)->h);

    float tx2 = Tools::getXTextureFromUV(getSurface(lod), C.u/getSurface(lod)->w);
    float ty2 = Tools::getYTextureFromUV(getSurface(lod), C.v/getSurface(lod)->h);

    float area = Maths::TriangleArea(tx0, ty0, tx1, ty1, tx2, ty2);

    return area;
}

bool Texture::isMipMapped() const {
    return mipMapped;
}

void Texture::setMipMapped(bool mipMapped) {
    Texture::mipMapped = mipMapped;
}

bool Texture::isLightMapped() const {
    return lightMapped;
}

void Texture::setLightMapped(bool lightMapped) {
    Texture::lightMapped = lightMapped;
}
