#define GL_GLEXT_PROTOTYPES

#include <SDL2/SDL_image.h>
#include "../../include/2D/TextWriter.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Brakeza3D.h"
#include <SDL2/SDL_opengl.h>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

TextWriter::TextWriter(SDL_Renderer *renderer, TTF_Font *font)
:
    renderer(renderer),
    font(font),
    alpha(1.0f)
{
}

void TextWriter::writeTextTTF(int x, int y, int w, int h, const char *text, Color c)
{
    auto *surfaceTTF = TTF_RenderText_Blended(font, text, c.toSDL());
    auto *textureTTF = SDL_CreateTextureFromSurface(renderer, surfaceTTF);

    GLuint texID = Image::makeOGLImage(surfaceTTF);

    auto renderer = ComponentsManager::get()->getComponentWindow();

    int windowWidth = renderer->width;
    int windowHeight = renderer->height;

    SDL_Rect dstRect;
    dstRect.x = (x * windowWidth) / EngineSetup::get()->screenWidth;
    dstRect.y = (y * windowHeight) / EngineSetup::get()->screenHeight;
    dstRect.w = (w * windowWidth) / EngineSetup::get()->screenWidth;
    dstRect.h = (h * windowHeight) / EngineSetup::get()->screenHeight;

    ComponentsManager::get()->getComponentWindow()->getShaderOGLImage()->renderTexture(
        texID,
        dstRect.x, dstRect.y,
        dstRect.w, dstRect.h,
        alpha,
        false,
        ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer()
    );

    glDeleteTextures(1, &texID);
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

    int totalW = ComponentsManager::get()->getComponentWindow()->width;
    int totalH = ComponentsManager::get()->getComponentWindow()->height;

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

