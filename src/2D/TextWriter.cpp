
#include <SDL2/SDL_image.h>
#include "../../include/2D/TextWriter.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Render/Logging.h"
#include "../../include/Brakeza3D.h"

TextWriter::TextWriter(SDL_Renderer *renderer, TTF_Font *font)
:
    renderer(renderer),
    font(font),
    alpha(255)
{
}

void TextWriter::writeTextTTF(int x, int y, int w, int h, const char *text, Color c)
{
    SDL_Color color;

    color.r = (int) c.r;
    color.g = (int) c.g;
    color.b = (int) c.b;

    auto *surfaceTTF = TTF_RenderText_Blended(font, text, color);
    auto *textureTTF = SDL_CreateTextureFromSurface(renderer, surfaceTTF);

    SDL_SetTextureAlphaMod(textureTTF, (int) alpha);

    SDL_Rect msgRect;
    msgRect.x = convertPositionXAspect(x);
    msgRect.y = convertPositionYAspect(y);
    msgRect.w = convertPositionXAspect(w);
    msgRect.h = convertPositionYAspect(h);

    SDL_RenderCopy(renderer, textureTTF, nullptr, &msgRect);

    SDL_FreeSurface(surfaceTTF);
    SDL_DestroyTexture(textureTTF);
}

void TextWriter::writeTextTTFAutoSize(int x, int y, const char *text, Color c, float sizeRatio)
{
    int w = 0, h = 0;
    TTF_SizeUTF8(font, text, &w, &h);

    w *= sizeRatio;
    h *= sizeRatio;

    writeTextTTF(x, y, w, h, text, c);
}

void TextWriter::writeTextTTFMiddleScreen(const char *text, Color c, float sizeRatio)
{
    int textWidth, textHeight;
    TTF_SizeUTF8(font, text, &textWidth, &textHeight);

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
    TTF_SizeUTF8(font, text, &w, &h);

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
    int w, h;
    SDL_GetRendererOutputSize(renderer, &w, &h);

    return (value * h) / EngineSetup::get()->screenHeight;
}

float TextWriter::getAlpha() const {
    return alpha;
}

void TextWriter::setAlpha(float alpha) {
    TextWriter::alpha = alpha;
}

TextWriter::~TextWriter()
{
}

void TextWriter::setFont(TTF_Font *font) {
    TextWriter::font = font;
}

TTF_Font *TextWriter::getFont() const {
    return font;
}

