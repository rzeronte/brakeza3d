#define GL_GLEXT_PROTOTYPES

#include "../../include/Render/TextWriter.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Brakeza.h"
#include "../../include/Components/Components.h"
#include "../../include/Misc/Logging.h"

TextWriter::TextWriter(SDL_Renderer *renderer, TTF_Font *font)
:
    renderer(renderer),
    font(font),
    alpha(1.0f)
{
}

TextWriter* TextWriter::create(const std::string& fontFile)
{
    if (!Tools::FileExists(fontFile.c_str())) {
        Logging::Message("[TextWriter] Cannot open font file: %s", fontFile.c_str());

        return nullptr;
    }

    return new TextWriter(
        Components::get()->Window()->getRenderer(),
        TTF_OpenFont(fontFile.c_str(), 35)
    );
}


void TextWriter::WriteTextTTF(int x, int y, int w, int h, const char *text, const Color &c) const
{
    auto surfaceTTF = TTF_RenderText_Blended(font, text, c.toSDL());
#ifdef _WIN32
    auto surfaceTTFGoodFormat = SDL_ConvertSurfaceFormat(surfaceTTF, SDL_PIXELFORMAT_BGRA32, 0);
    GLuint texID = Image::MakeOGLImage(surfaceTTFGoodFormat);
#else
    GLuint texID = Image::MakeOGLImage(surfaceTTF);
#endif

    auto renderer = Components::get()->Window();

    int windowWidth = renderer->getWidth();
    int windowHeight = renderer->getHeight();

    SDL_Rect dstRect;
    dstRect.x = (x * windowWidth) / Config::get()->screenWidth;
    dstRect.y = (y * windowHeight) / Config::get()->screenHeight;
    dstRect.w = (w * windowWidth) / Config::get()->screenWidth;
    dstRect.h = (h * windowHeight) / Config::get()->screenHeight;

    Components::get()->Render()->getShaders()->shaderOGLImage->renderTexture(
        texID,
        dstRect.x, dstRect.y,
        dstRect.w, dstRect.h,
        alpha,
        false,
        Components::get()->Window()->getForegroundFramebuffer()
    );

    glDeleteTextures(1, &texID);
    SDL_FreeSurface(surfaceTTF);
#ifdef _WIN32
    SDL_FreeSurface(surfaceTTFGoodFormat);
#endif
}

void TextWriter::WriteTextTTFAutoSize(int x, int y, const char *text, const Color &c, float sizeRatio) const
{
    int w = 0, h = 0;
    TTF_SizeUTF8(font, text, &w, &h);

    w *= (int) sizeRatio;
    h *= (int) sizeRatio;

    WriteTextTTF(x, y, w, h, text, c);
}

void TextWriter::WriteTextTTFMiddleScreen(const char *text, const Color &c, float sizeRatio) const
{
    int textWidth, textHeight;
    TTF_SizeUTF8(font, text, &textWidth, &textHeight);

    textWidth *= (int) sizeRatio;
    textHeight *= (int) sizeRatio;

    const int totalW = Config::get()->screenWidth;
    const int totalH = Config::get()->screenHeight;

    int xPosition = (totalW / 2) - textWidth / 2;
    int yPosition = totalH / 2;

    WriteTextTTFAutoSize(xPosition, yPosition, text, c, sizeRatio);
}

void TextWriter::WriteTTFCenterHorizontal(int y, const char *text, const Color &c, float sizeRatio) const
{
    int w, h;
    TTF_SizeUTF8(font, text, &w, &h);

    w *= (int) sizeRatio;
    h *= (int) sizeRatio;

    int totalW = Config::get()->screenWidth;

    int xPosition = (totalW / 2) - w / 2;

    WriteTextTTFAutoSize(xPosition, y, text, c, sizeRatio);
}

float TextWriter::getAlpha() const {
    return alpha;
}

void TextWriter::setAlpha(float alpha) {
    TextWriter::alpha = alpha;
}

void TextWriter::setFont(TTF_Font *font) {
    TextWriter::font = font;
}
