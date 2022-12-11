
#include <SDL2/SDL_image.h>
#include "../../include/2D/TextWriter.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Render/Logging.h"
#include "../../include/Brakeza3D.h"

TextWriter::TextWriter(SDL_Renderer *renderer, const char *concharsFile) {
    if (!Tools::fileExists(concharsFile)) {
        Logging::Log(std::string("Error loading file") + concharsFile, "TextWriter");
        return;
    }

    this->renderer = renderer;

    sprite = IMG_Load(concharsFile);

    SDL_Rect r;
    r.w = CONCHARS_CHARACTER_W;
    r.h = CONCHARS_CHARACTER_H;
    Uint32 rmask, gmask, bmask, amask;

    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        rmask = 0xff000000;
        gmask = 0x00ff0000;
        bmask = 0x0000ff00;
        amask = 0x000000ff;
    #else
        rmask = 0x000000ff;
        gmask = 0x0000ff00;
        bmask = 0x00ff0000;
        amask = 0xff000000;
    #endif

    for (int y = 0; y < CONCHARS_HEIGHT; y += CONCHARS_CHARACTER_H) {
        for (int x = 0; x < CONCHARS_WIDTH; x += CONCHARS_CHARACTER_W) {
            r.x = x;
            r.y = y;

            auto *s = SDL_CreateRGBSurface(
                    0,
                    CONCHARS_CHARACTER_W,
                    CONCHARS_CHARACTER_H,
                    32,
                    rmask,
                    gmask,
                    bmask,
                    amask
            );

            SDL_BlitSurface(sprite, &r, s, nullptr);
            this->characterSurfaces.push_back(s);
        }
    }
}

void TextWriter::putCharacter(int ascii, int xOrigin, int yOrigin) {
    SDL_Surface *c = this->characterSurfaces[ascii];

    for (int x = 0; x < c->w; x++) {
        for (int y = 0; y < c->h; y++) {
            Color color = Tools::readSurfacePixel(this->characterSurfaces[ascii], x, y);

            if (color.getColor() == Color::black().getColor()) {
                continue;
            }
            //EngineBuffers::get()->HUDbuffer[(yOrigin + y) * 320 + (xOrigin + x) ] = Tools::random(200, 256);
            EngineBuffers::getInstance()->setVideoBuffer(xOrigin + x, yOrigin + y, color.getColor());
        }
    }
}


void TextWriter::writeCenterHorizontal(int y, const char *text, bool bold)
{
    int totalW = EngineSetup::get()->screenWidth;

    int xPosition = (totalW / 2) - (int) (strlen(text) * CONCHARS_CHARACTER_W) / 2;

    writeText(xPosition, y, text, bold);
}


void TextWriter::writeTextMiddleScreen(const char *text, bool bold)
{
    int totalW = EngineSetup::get()->screenWidth;
    int totalH =EngineSetup::get()->screenHeight;

    int xPosition = (totalW / 2) - (int) (strlen(text) * CONCHARS_CHARACTER_W) / 2;

    writeText(xPosition, totalH / 2, text, bold);
}


void TextWriter::writeText(int x, int y, const char *text, bool bold) {
    int currentX = x;

    // The sprite have two sets of characters, first normal, second bold
    int lengthDictionary = ((CONCHARS_WIDTH / CONCHARS_CHARACTER_W) * (CONCHARS_HEIGHT / CONCHARS_CHARACTER_H)) / 2;
    for (int i = 0; i < (int) strlen(text); i++) {
        int c = (int) text[i];

        if (bold) {
            c += lengthDictionary;
        }
        this->putCharacter(c, currentX, y);
        currentX += CONCHARS_CHARACTER_W;
    }
}

void TextWriter::writeTextTTF(int x, int y, int w, int h, const char *text, Color c)
{
    SDL_Color color;

    auto font = ComponentsManager::get()->getComponentWindow()->fontDefault;

    color.r = (int) c.r;
    color.g = (int) c.g;
    color.b = (int) c.b;

    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, text, color);

    SDL_Texture* msg = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    SDL_Rect msgRect;
    msgRect.x = convertPositionXAspect(x);
    msgRect.y = convertPositionYAspect(y);
    msgRect.w = convertPositionXAspect(w);
    msgRect.h = convertPositionYAspect(h);

    SDL_RenderCopy(renderer, msg, NULL, &msgRect);

    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(msg);
}

void TextWriter::writeTextTTFAutoSize(int x, int y, const char *text, Color c, float sizeRatio)
{
    auto font = ComponentsManager::get()->getComponentWindow()->fontDefault;
    int w, h;
    TTF_SizeText(font, text, &w, &h);

    w *= sizeRatio;
    h *= sizeRatio;

    writeTextTTF(x, y, w, h, text, c);
}

void TextWriter::writeTextTTFMiddleScreen(const char *text, Color c, float sizeRatio)
{
    int textWidth, textHeight;
    auto font = ComponentsManager::get()->getComponentWindow()->fontDefault;
    TTF_SizeText(font, text, &textWidth, &textHeight);

    textWidth *= sizeRatio;
    textHeight *= sizeRatio;

    int totalW = EngineSetup::get()->screenWidth;
    int totalH = EngineSetup::get()->screenHeight;

    int xPosition = (totalW / 2) - textWidth / 2;
    int yPosition = totalH / 2;

    writeTextTTFAutoSize(xPosition, yPosition, text, c, sizeRatio);
}

void TextWriter::writeTTFCenterHorizontal(int y, const char *text, Color c, float sizeRatio)
{
    int w, h;
    auto font = ComponentsManager::get()->getComponentWindow()->fontDefault;
    TTF_SizeText(font, text, &w, &h);

    w *= sizeRatio;
    h *= sizeRatio;

    int totalW = EngineSetup::get()->screenWidth;

    int xPosition = (totalW / 2) - w / 2;

    writeTextTTFAutoSize(xPosition, y, text, c, sizeRatio);
}

int TextWriter::convertPositionXAspect(int value)
{
    int windowWidth, windowHeight;
    SDL_GetRendererOutputSize(renderer, &windowWidth, &windowHeight);

    return (value * windowWidth) / EngineSetup::get()->screenWidth;
}

int TextWriter::convertPositionYAspect(int value)
{
    int windowWidth, windowHeight;
    SDL_GetRendererOutputSize(renderer, &windowWidth, &windowHeight);

    return (value * windowHeight) / EngineSetup::get()->screenHeight;
}
