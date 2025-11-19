#define GL_GLEXT_PROTOTYPES

#include "../../include/2D/TextWriter.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Brakeza3D.h"

TextWriter::TextWriter(SDL_Renderer *renderer, TTF_Font *font)
:
    renderer(renderer),
    font(font),
    alpha(1.0f)
{
}

TextWriter* TextWriter::create(const std::string& fontFile)
{
    if (!Tools::fileExists(fontFile.c_str())) {
        Logging::Message("[TextWriter] Cannot open font file: %s", fontFile.c_str());

        return nullptr;
    }

    return new TextWriter(
        ComponentsManager::get()->getComponentWindow()->getRenderer(),
        TTF_OpenFont(fontFile.c_str(), 35)
    );
}


void TextWriter::writeTextTTF(int x, int y, int w, int h, const char *text, Color c)
{
    auto surfaceTTF = TTF_RenderText_Blended(font, text, c.toSDL());
#ifdef _WIN32
    auto surfaceTTFGoodFormat = SDL_ConvertSurfaceFormat(surfaceTTF, SDL_PIXELFORMAT_BGRA32, 0);
    GLuint texID = Image::makeOGLImage(surfaceTTFGoodFormat);
#else
    GLuint texID = Image::makeOGLImage(surfaceTTF);
#endif

    auto renderer = ComponentsManager::get()->getComponentWindow();

    int windowWidth = renderer->widthWindow;
    int windowHeight = renderer->heightWindow;

    SDL_Rect dstRect;
    dstRect.x = (x * windowWidth) / EngineSetup::get()->screenWidth;
    dstRect.y = (y * windowHeight) / EngineSetup::get()->screenHeight;
    dstRect.w = (w * windowWidth) / EngineSetup::get()->screenWidth;
    dstRect.h = (h * windowHeight) / EngineSetup::get()->screenHeight;

    ComponentsManager::get()->getComponentRender()->getShaderOGLImage()->renderTexture(
        texID,
        dstRect.x, dstRect.y,
        dstRect.w, dstRect.h,
        alpha,
        false,
        ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer()
    );

    glDeleteTextures(1, &texID);
    SDL_FreeSurface(surfaceTTF);
#ifdef _WIN32
    SDL_FreeSurface(surfaceTTFGoodFormat);
#endif
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

    const int totalW = EngineSetup::get()->screenWidth;
    const int totalH = EngineSetup::get()->screenHeight;

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
