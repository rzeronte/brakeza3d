#include <SDL2/SDL_image.h>
#include "../../include/Misc/TextureAtlas.h"
#include "../../include/Render/Logging.h"


TextureAtlas::TextureAtlas(int totalWidth, int totalHeight) :
        total_width(totalWidth), total_height(totalHeight) {
    atlas_surface = SDL_CreateRGBSurface(0, totalWidth, totalHeight, 32, 0, 0, 0, 0);

    mask = new bool[total_width * total_height];

    for (int i = 0; i < (total_width * total_height); i++) {
        mask[i] = false;
    }
}

SDL_Surface *TextureAtlas::getAtlasSurface() const {
    return atlas_surface;
}

bool TextureAtlas::addTexture(Texture *texture, bool lightmap, const std::string& name) {
    SDL_Surface *texture_surface = texture->getSurface(1);

    if (lightmap) {
        texture_surface = texture->lightmap;
    }

    int texw = texture_surface->w;
    int texh = texture_surface->h;

    for (int y = 0; y < total_height; y++) {
        for (int x = 0; x < total_width; x++) {

            if (this->checkForAllocate(x, y, texw, texh)) {
                Logging::Log(
                        "Add texture to atlas SUCCESS(" + texture->getFilename() + ", x: " + std::to_string(x) +
                        ", y: " + std::to_string(y) + ", width: " + std::to_string(texw) + ", height:" +
                        std::to_string(texh) + ")", "TextureAtlas");

                TextureAtlasImageInfo t_info;
                t_info.name = name;
                t_info.x = (float) x;
                t_info.y = (float) y;
                t_info.width = (float) texw;
                t_info.height = (float) texh;

                textures_info.push_back(t_info);
                textures.push_back(texture);

                allocateMask(x, y, texw, texh);

                SDL_Rect r;
                r.x = x;
                r.y = y;
                r.w = texw;
                r.h = texh;

                SDL_BlitSurface(texture_surface, nullptr, atlas_surface, &r);

                return true;
            }
        }
    }

    Logging::Log("addTexture failed!", "ERROR");

    return false;
}

bool TextureAtlas::checkForAllocate(int xpos, int ypos, int width, int height) {
    int baseOffset = ypos * total_width + xpos;
    int max_global_x = (total_width * ypos) + total_width;

    if (baseOffset + width > max_global_x) {
        //std::string msg = "checkForAllocate hit final line!! w: " + std::to_string(width) + ", h: " + std::to_string(height) + ", baseOffset: " + std::to_string(baseOffset) + ", max_global_x:" + std::to_string(max_global_x);
        //Logging::Log(msg);
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

void TextureAtlas::allocateMask(int xpos, int ypos, int width, int height) {
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

TextureAtlasImageInfo TextureAtlas::getAtlasTextureInfoForName(const std::string& name) {
    for (int i = 0; i < this->textures_info.size(); i++) {
        if (textures_info[i].name == name) {
            return textures_info[i];
        }
    }
    return TextureAtlasImageInfo();
}

void TextureAtlas::saveJPG(const std::string& name) const {
    IMG_SavePNG(getAtlasSurface(), name.c_str());
}

