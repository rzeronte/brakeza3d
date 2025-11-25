#include <SDL2/SDL_image.h>
#include "../../include/Misc/TextureAtlas.h"
#include "../../include/Misc/Logging.h"

TextureAtlas::TextureAtlas(int totalWidth, int totalHeight)
:
    total_width(totalWidth),
    total_height(totalHeight)
{
    atlasSurface = SDL_CreateRGBSurface(0, totalWidth, totalHeight, 32, 0, 0, 0, 0);

    mask = new bool[total_width * total_height];

    for (int i = 0; i < total_width * total_height; i++) {
        mask[i] = false;
    }
}

SDL_Surface *TextureAtlas::getAtlasSurface() const
{
    return atlasSurface;
}

bool TextureAtlas::addTexture(Image *texture, const std::string& name)
{
    SDL_Surface *texture_surface = texture->getSurface();

    int texw = texture_surface->w;
    int texh = texture_surface->h;

    for (int y = 0; y < total_height; y++) {
        for (int x = 0; x < total_width; x++) {

            if (this->checkForAllocate(x, y, texw, texh)) {

                TextureAtlasImageInfo textureAtlasInfo;
                textureAtlasInfo.name = name;
                textureAtlasInfo.x = static_cast<float>(x);
                textureAtlasInfo.y = static_cast<float>(y);
                textureAtlasInfo.width = static_cast<float>(texw);
                textureAtlasInfo.height = static_cast<float>(texh);

                textures_info.push_back(textureAtlasInfo);
                textures.push_back(texture);

                allocateMask(x, y, texw, texh);

                SDL_Rect r;
                r.x = x;
                r.y = y;
                r.w = texw;
                r.h = texh;

                SDL_BlitSurface(texture_surface, nullptr, atlasSurface, &r);

                return true;
            }
        }
    }

    Logging::Message("addTexture failed!");

    return false;
}

bool TextureAtlas::checkForAllocate(int xpos, int ypos, int width, int height) const
{
    int baseOffset = ypos * total_width + xpos;
    int max_global_x = total_width * ypos + total_width;

    if (baseOffset + width > max_global_x) {
        return false;
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int localIndex = y * width + x;
            int globalIndex = baseOffset + localIndex;

            if (mask[globalIndex]) {
                return false;
            }
        }

        baseOffset += total_width - width;
    }

    return true;
}

void TextureAtlas::allocateMask(int xpos, int ypos, int width, int height) const
{
    int baseOffset = ypos * total_width + xpos;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int localIndex = y * width + x;
            int globalIndex = baseOffset + localIndex;

            mask[globalIndex] = true;
        }
        baseOffset += total_width - width;
    }
}

TextureAtlasImageInfo TextureAtlas::getAtlasTextureInfoForName(const std::string& name)
{
    for (int i = 0; i < this->textures_info.size(); i++) {
        if (textures_info[i].name == name) {
            return textures_info[i];
        }
    }

    return TextureAtlasImageInfo();
}

void TextureAtlas::saveJPG(const std::string& name) const
{
    IMG_SavePNG(getAtlasSurface(), name.c_str());
}