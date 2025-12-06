#include <SDL2/SDL_image.h>
#include "../../include/Misc/TextureAtlas.h"

#include "../../include/Components/ComponentsManager.h"
#include "../../include/Misc/Logging.h"

void TextureAtlas::AllocateEmptyMask(int totalWidth, int totalHeight)
{
    mask = new bool[totalWidth * totalHeight];

    for (int i = 0; i < totalWidth * totalHeight; i++) {
        mask[i] = false;
    }
}

TextureAtlas::TextureAtlas(int totalWidth, int totalHeight)
:
    totalWidth(totalWidth),
    totalHeight(totalHeight),
    atlasSurface(SDL_CreateRGBSurface(0, totalWidth, totalHeight, 32, 0, 0, 0, 0))
{
    AllocateEmptyMask(totalWidth, totalHeight);
}

int TextureAtlas::getIndexByXY(int x, int y) const
{
    return x + numColumns * y;
}

Image *TextureAtlas::getTextureByIndex(int index) const
{
    return textures[index];
}

void TextureAtlas::setXYByIndex(int index, int& x, int& y) const
{
    x = index % numColumns;
    y = index / numColumns;
}

Image *TextureAtlas::getTextureByXY(int x, int y) const
{
    return textures[x + numColumns * y];
}

int TextureAtlas::getTotalImages() const
{
    return numColumns * numRows;
}

void TextureAtlas::CreateFromSheet(const std::string &file, int spriteWidth, int spriteHeight)
{
    Logging::Message("TextureAtlas createFromSheet: %s, w: %d, h: %d", file.c_str(), spriteWidth, spriteHeight);

    if (!Tools::FileExists(file.c_str())) {
        Logging::Message("[TextureAtlas] Failed to load sprite sheet: %s", SDL_GetError());
        exit(-1);
    }

    SDL_Surface* spriteSheetSurface = IMG_Load(file.c_str());
    if (!spriteSheetSurface) {
        Logging::Message("[TextureAtlas] Failed to load sprite sheet: %s", SDL_GetError());
        exit(-1);
    }

    totalWidth = spriteSheetSurface->w;
    totalHeight = spriteSheetSurface->h;

    AllocateEmptyMask(totalWidth, totalHeight);

    numRows = spriteSheetSurface->h / spriteHeight;
    numColumns = spriteSheetSurface->w / spriteWidth;

    const auto renderer = ComponentsManager::get()->getComponentWindow()->getRenderer();

    for (int row = 0; row < numRows; ++row) {
        for (int column = 0; column < numColumns; ++column) {
            AllocateMask(row, column, spriteWidth, spriteHeight);

            SDL_Rect spriteRect = { column * spriteWidth, row * spriteHeight, spriteWidth, spriteHeight };
            SDL_Surface* destinySurface = SDL_CreateRGBSurfaceWithFormat(
                0,
                spriteWidth,
                spriteHeight,
                32,
                spriteSheetSurface->format->format
            );

            SDL_BlitSurface(spriteSheetSurface, &spriteRect, destinySurface, nullptr);

            SDL_Texture* spriteTexture = SDL_CreateTextureFromSurface(renderer, destinySurface);
            if (!spriteTexture) {
                Logging::Message("[TextureAtlas] Failed to create texture: %s", SDL_GetError());
                SDL_FreeSurface(spriteSheetSurface);
                return;
            }

            textures.push_back(new Image(destinySurface, spriteTexture));
        }
    }

    Logging::Message("[TextureAtlas] createFromSheet: Sheet successful loaded with %d images", (int) textures.size());
    SDL_FreeSurface(spriteSheetSurface);
}

int TextureAtlas::getNumRows() const
{
    return numRows;
}

int TextureAtlas::getNumColumns() const
{
    return numColumns;
}

bool TextureAtlas::AddToAtlas(Image *texture, const std::string& name)
{
    SDL_Surface *texture_surface = texture->getSurface();

    int texw = texture_surface->w;
    int texh = texture_surface->h;

    for (int y = 0; y < totalHeight; y++) {
        for (int x = 0; x < totalWidth; x++) {

            if (this->AllocationCheck(x, y, texw, texh)) {

                TextureAtlasImageInfo textureAtlasInfo;
                textureAtlasInfo.name = name.c_str();
                textureAtlasInfo.x = (float) x;
                textureAtlasInfo.y = (float) y;
                textureAtlasInfo.width = (float) texw;
                textureAtlasInfo.height = (float) texh;

                texturesData.push_back(textureAtlasInfo);
                textures.push_back(texture);

                AllocateMask(x, y, texw, texh);

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

bool TextureAtlas::AllocationCheck(int xpos, int ypos, int width, int height) const
{
    int baseOffset = ypos * totalWidth + xpos;
    int max_global_x = totalWidth * ypos + totalWidth;

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

        baseOffset += totalWidth - width;
    }

    return true;
}

void TextureAtlas::AllocateMask(int xpos, int ypos, int width, int height) const
{
    int baseOffset = ypos * totalWidth + xpos;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int localIndex = y * width + x;
            int globalIndex = baseOffset + localIndex;

            mask[globalIndex] = true;
        }
        baseOffset += totalWidth - width;
    }
}

TextureAtlasImageInfo TextureAtlas::getAtlasTextureInfoForName(const std::string& name)
{
    for (int i = 0; i < this->texturesData.size(); i++) {
        if (texturesData[i].name == name) {
            return texturesData[i];
        }
    }

    return TextureAtlasImageInfo();
}

void TextureAtlas::SavePNG(const std::string& name) const
{
    IMG_SavePNG(atlasSurface, name.c_str());
}

TextureAtlas::~TextureAtlas()
{
    delete[] mask;
    SDL_FreeSurface(atlasSurface);
}