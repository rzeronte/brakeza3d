#include <SDL2/SDL_image.h>

#include "../../include/Render/Image.h"
#include "../../include/Brakeza.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Misc/Logging.h"
#include "../../include/Misc/ToolsMaths.h"
#include "../../include/Components/Components.h"
#include "../../include/Threads/ThreadJobLoadImage.h"
#include "../../include/Render/Profiler.h"

Image::Image(const std::string& filename)
{
    auto job = std::make_shared<ThreadJobLoadImage>(this, filename);
    Brakeza::get()->PoolImages().enqueueWithMainThreadCallback(job);
}

Image::Image(SDL_Surface *surface, SDL_Texture *texture)
:
    loaded(true),
    textureId(MakeOGLImage(surface)),
    surface(surface),
    texture(texture)
{
    std::string n = "BySDLTexture";
    setFilePath(n);
    Profiler::get()->AddImage(this);
}

void Image::CreateSDLTexture()
{
    texture = SDL_CreateTextureFromSurface(Components::get()->Window()->getRenderer(), surface);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
}

void Image::LoadSDLSurface()
{
    surface = IMG_Load(fileName.c_str());
}

void Image::setImage(const std::string &filename) {
    if (!Tools::FileExists(filename.c_str())) {
        Logging::Error("[Image] Error loading file '%s'", filename.c_str());
        return;
    }

    fileName = filename;

    LoadSDLSurface();
}

void Image::DrawFlatAlpha(int pos_x, int pos_y, float alpha, GLuint fbo)
{
    setAlpha(alpha);
    DrawFlat(pos_x, pos_y, fbo);
}

void Image::DrawFlatAlpha(int x, int y, int w, int h, float alpha, GLuint fbo)
{
    setAlpha(alpha);
    DrawFlat(x, y, w, h, fbo);
}

void Image::DrawFlat(int x, int y, int w, int h, GLuint fbo) const
{
    if (!loaded) return;

    auto window = Components::get()->Window();

    int windowWidth = window->getWidth();
    int windowHeight = window->getHeight();

    SDL_Rect srcRect;
    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = surface->w;
    srcRect.h = surface->h;

    SDL_Rect dstRect;
    dstRect.x = x * windowWidth / Config::get()->screenWidth;
    dstRect.y = y * windowHeight / Config::get()->screenHeight;
    dstRect.w = w * windowWidth / Config::get()->screenWidth;
    dstRect.h = h * windowHeight / Config::get()->screenHeight;

    Components::get()->Render()->getShaders()->shaderOGLImage->renderTexture(
        textureId,
        dstRect.x, dstRect.y,
        dstRect.w,dstRect.h,
        alpha,
        false,
        fbo
    );
}

void Image::DrawFlat(int pos_x, int pos_y, GLuint fbo) const
{
    if (!loaded) return;

    auto window = Components::get()->Window();

    int windowWidth = window->getWidth();
    int windowHeight = window->getHeight();

    SDL_Rect srcRect;
    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = surface->w;
    srcRect.h = surface->h;

    SDL_Rect dstRect;
    dstRect.x = pos_x * windowWidth / Config::get()->screenWidth;
    dstRect.y = pos_y * windowHeight / Config::get()->screenHeight;
    dstRect.w = surface->w * windowWidth / Config::get()->screenWidth;
    dstRect.h = surface->h * windowHeight / Config::get()->screenHeight;

    Components::get()->Render()->getShaders()->shaderOGLImage->renderTexture(
        textureId,
        dstRect.x, dstRect.y,
        dstRect.w, dstRect.h,
        alpha,
        false,
        fbo
    );
}

void Image::LoadFromRaw(const unsigned int *texture, int w, int h)
{
    this->surface = SDL_CreateRGBSurface(0, h, w, 32, 0, 0, 0, 0);

    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            Tools::SurfacePutPixel(surface, x, y, texture[y + x * w]);
        }
    }
}

float Image::getAreaForVertices(const Vertex3D &A, const Vertex3D &B, const Vertex3D &C) const
{
    float tx0 = Tools::getXTextureFromUV(surface, A.u / (float) surface->w);
    float ty0 = Tools::getYTextureFromUV(surface, A.v / (float) surface->h);

    float tx1 = Tools::getXTextureFromUV(surface, B.u / (float) surface->w);
    float ty1 = Tools::getYTextureFromUV(surface, B.v / (float) surface->h);

    float tx2 = Tools::getXTextureFromUV(surface, C.u / (float) surface->w);
    float ty2 = Tools::getYTextureFromUV(surface, C.v / (float) surface->h);

    float area = ToolsMaths::TriangleArea(tx0, ty0, tx1, ty1, tx2, ty2);

    return area;
}

Image::~Image()
{
    Profiler::get()->RemoveImage(this);

    if (loaded) {
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
        glDeleteTextures(1, &textureId);
    }
}

Color Image::getPixelColor(const int x, const int y) const
{
    auto pixels = static_cast<uint32_t *>(this->pixels());

    const int index = y * this->surface->w + x;

    return Color(pixels[index]);
}

void Image::MakeAutoOGLImage()
{
    if (textureId == 0 ) {
        glDeleteTextures(1, &textureId);
    }
    textureId = MakeOGLImage(surface);
}

GLuint Image::MakeOGLImage(const SDL_Surface *surfaceTTF)
{
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLint Mode = GL_RGB;
    if (surfaceTTF->format->BytesPerPixel == 4) {
        Mode = GL_RGBA;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, Mode, surfaceTTF->w, surfaceTTF->h, 0, Mode, GL_UNSIGNED_BYTE, surfaceTTF->pixels);

    return texID;
}

void Image::setAlpha(float alpha)
{
    Image::alpha = alpha;
}

void Image::setOGLTextureID(GLuint value)
{
    textureId = value;
}

void Image::setAlreadyLoaded()
{
    loaded = true;
}

void Image::setFilePath(std::string &path)
{
    fileName = path;
}