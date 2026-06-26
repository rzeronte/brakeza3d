#include <SDL2/SDL_image.h>

#include "../../include/Render/Image.h"
#include "../../include/Brakeza.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Misc/Logging.h"
#include "../../include/Misc/ToolsMaths.h"
#include "../../include/Components/Components.h"
#include "../../include/Threads/ThreadJobLoadImage.h"

Image::Image(const FilePath::ImageFile& filename)
:
    fileName(filename)
{
    auto job = std::make_shared<ThreadJobLoadImage>(this, filename);
    Brakeza::get()->PoolImages().enqueueWithMainThreadCallback(job);
}

Image::Image(SDL_Surface *surface, SDL_Texture *texture)
:
    loaded(true),
    textureId(MakeOGLImage(surface)),
    fileName("BySDLTexture"),
    surface(surface),
    texture(texture)
{
}

Image::Image(GLuint externalTextureId, int width, int height)
    : loaded(true)
    , alpha(1.0f)
    , textureId(externalTextureId)
    , fileName(FilePath::ImageFile("[External Texture]"))
{
    // Crear una superficie SDL vacía solo para almacenar dimensiones
    // (necesaria porque otros métodos usan surface->w y surface->h)
    surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);

    if (!surface) {
        LOG_MESSAGE("Warning: Could not create dummy SDL_Surface for external texture");
    }
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

void Image::setImage(const FilePath::ImageFile &filename)
{
    if (!Tools::FileExists(filename.c_str())) {
        LOG_ERROR("[Image] Error loading file '%s'", filename.c_str());
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

    const int renderW = window->getWidthRender();
    const int renderH = window->getHeightRender();

    SDL_Rect dstRect;
    dstRect.x = x * renderW / Config::get()->screenWidth;
    dstRect.y = y * renderH / Config::get()->screenHeight;
    dstRect.w = w * renderW / Config::get()->screenWidth;
    dstRect.h = h * renderH / Config::get()->screenHeight;

    Components::get()->Render()->getShaders()->shaderOGLImage->renderTexture(
        textureId,
        dstRect.x, dstRect.y,
        dstRect.w, dstRect.h,
        renderW, renderH,
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

    /*SDL_Rect srcRect;
    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = surface->w;
    srcRect.h = surface->h;*/

    SDL_Rect dstRect;
    dstRect.x = pos_x * windowWidth / Config::get()->screenWidth;
    dstRect.y = pos_y * windowHeight / Config::get()->screenHeight;
    dstRect.w = surface->w * windowWidth / Config::get()->screenWidth;
    dstRect.h = surface->h * windowHeight / Config::get()->screenHeight;

    Components::get()->Render()->getShaders()->shaderOGLImage->renderTexture(
        textureId,
        dstRect.x, dstRect.y,
        dstRect.w, dstRect.h,
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

void Image::setFilePath(const FilePath::ImageFile &path)
{
    fileName = path;
}

Image* Image::createEmpty(int w, int h)
{
    auto* surface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA32);
    SDL_FillRect(surface, nullptr, 0);
    return new Image(surface, nullptr);
}

void Image::clearChannel(int channel, uint8_t value)
{
    auto* pixels = static_cast<uint8_t*>(surface->pixels);
    int total = surface->w * surface->h;
    for (int i = 0; i < total; i++) {
        pixels[i * 4 + channel] = value;
    }
}

void Image::fillCircle(int cx, int cy, int radius,
                       uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    auto* pixels = static_cast<uint8_t*>(surface->pixels);
    int w = surface->w, h = surface->h;
    int r2 = radius * radius;
    for (int dz = -radius; dz <= radius; dz++) {
        for (int dx = -radius; dx <= radius; dx++) {
            if (dx*dx + dz*dz > r2) continue;
            int nx = cx + dx, ny = cy + dz;
            if (nx < 0 || nx >= w || ny < 0 || ny >= h) continue;
            int idx = (ny * w + nx) * 4;
            pixels[idx + 0] = r;
            pixels[idx + 1] = g;
            pixels[idx + 2] = b;
            pixels[idx + 3] = a;
        }
    }
}

void Image::upload()
{
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                    surface->w, surface->h,
                    GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Image::destroy()
{
    if (loaded) {
        if (textureId != 0) {
            glDeleteTextures(1, &textureId);
            textureId = 0;
        }
        if (surface != nullptr) {
            SDL_FreeSurface(surface);
            surface = nullptr;
        }
        if (texture != nullptr) {
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }
        loaded = false;
    }
}

Image::~Image()
{
    if (texture != nullptr) SDL_DestroyTexture(texture);
    if (surface != nullptr) SDL_FreeSurface(surface);
    if (textureId != 0)     glDeleteTextures(1, &textureId);
}
