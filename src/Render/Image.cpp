#include <SDL2/SDL_image.h>

#include "../../include/Render/Image.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Misc/Logging.h"
#include "../../include/Render/Maths.h"
#include "../../include/Components/ComponentsManager.h"


Image::Image(const std::string& filename)
{
    loadTGA(filename);
}

Image::Image(SDL_Surface *surface, SDL_Texture *texture)
:
    loaded(true),
    textureId(makeOGLImage(surface)),
    surface(surface),
    texture(texture)
{
}

void Image::createEmpty(int w, int h)
{
    SDL_CreateRGBSurface(SDL_PIXELFORMAT_RGBA32, w, h, 32, 0, 0, 0, 0);
    loaded = true;
}

void Image::loadTGA(const std::string& filename)
{
    if (Tools::fileExists(filename.c_str())) {
        surface = IMG_Load(filename.c_str());
        texture = SDL_CreateTextureFromSurface(ComponentsManager::get()->getComponentWindow()->getRenderer(), surface);

        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        textureId = makeOGLImage(surface);

        fileName = filename;
        loaded = true;

        Logging::Message("Loading Image file (%s) (%dx%d)", filename.c_str(), width(), height());

        return;
    }

    std::cout << "Error loading TGA texture '%s'" << filename.c_str() << std::endl;
    Logging::Message("Error loading TGA texture '%s'", filename.c_str());

    exit(-1);
}

void Image::drawFlatAlpha(int pos_x, int pos_y, float alpha, GLuint fbo)
{
    setAlpha(alpha);
    drawFlat(pos_x, pos_y, fbo);
}

void Image::drawFlat(int pos_x, int pos_y, GLuint fbo) const
{
    if (!loaded) return;

    auto window = ComponentsManager::get()->getComponentWindow();

    int windowWidth = window->getWidth();
    int windowHeight = window->getHeight();

    SDL_Rect srcRect;
    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = surface->w;
    srcRect.h = surface->h;

    SDL_Rect dstRect;
    dstRect.x = (pos_x * windowWidth) / BrakezaSetup::get()->screenWidth;
    dstRect.y = (pos_y * windowHeight) / BrakezaSetup::get()->screenHeight;
    dstRect.w = (surface->w * windowWidth) / BrakezaSetup::get()->screenWidth;
    dstRect.h = (surface->h * windowHeight) / BrakezaSetup::get()->screenHeight;

    ComponentsManager::get()->getComponentRender()->getShaderOGLImage()->renderTexture(
        textureId,
        dstRect.x,
        dstRect.y,
        dstRect.w,
        dstRect.h,
        alpha,
        false,
        fbo
    );
}

void Image::loadFromRaw(const unsigned int *texture, int w, int h)
{
    this->surface = SDL_CreateRGBSurface(0, h, w, 32, 0, 0, 0, 0);

    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            Tools::SurfacePutPixel(surface, x, y, texture[y + x * w]);
        }
    }

}

int Image::width() const
{
    return surface->w;
}

int Image::height() const
{
    return surface->h;
}

void *Image::pixels() const
{
    return surface->pixels;
}

bool Image::isLoaded() const
{
    return loaded;
}

float Image::getAreaForVertices(const Vertex3D &A, const Vertex3D &B, const Vertex3D &C) const
{
    float tx0 = Tools::getXTextureFromUV(surface, A.u / surface->w);
    float ty0 = Tools::getYTextureFromUV(surface, A.v / surface->h);

    float tx1 = Tools::getXTextureFromUV(surface, B.u / surface->w);
    float ty1 = Tools::getYTextureFromUV(surface, B.v / surface->h);

    float tx2 = Tools::getXTextureFromUV(surface, C.u / surface->w);
    float ty2 = Tools::getYTextureFromUV(surface, C.v / surface->h);

    float area = Maths::TriangleArea(tx0, ty0, tx1, ty1, tx2, ty2);

    return area;
}

SDL_Surface *Image::getSurface() const
{
    return surface;
}

SDL_Texture *Image::getTexture() const
{
    return texture;
}

const std::string &Image::getFileName() const {
    return fileName;
}

Image::~Image()
{
    if (loaded) {
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
        glDeleteTextures(1, &textureId);

    }
}

Color Image::getColor(const int x, const int y) const
{
    uint32_t *pixels = static_cast<uint32_t *>(this->pixels());

    const int index = y * this->surface->w + x;

    return Color(pixels[index]);
}

void Image::setImage(const std::string &filename)
{
    if (Tools::fileExists(filename.c_str())) {

        if (this->texture != nullptr) {
            SDL_DestroyTexture(this->texture);
            SDL_FreeSurface(this->surface);
            this->surface = IMG_Load(filename.c_str());
            this->texture = SDL_CreateTextureFromSurface(ComponentsManager::get()->getComponentWindow()->getRenderer(), surface);
        }

        if (textureId == 0 ) {
            SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
            glDeleteTextures(1, &textureId);
            textureId = makeOGLImage(surface);
        }

        this->fileName = filename;
        this->loaded = true;
        Logging::Message("Loading TGA texture '%s'", filename.c_str());

        return;
    }

    Logging::Message("Error loading TGA texture '%s'", filename.c_str());
    exit(-1);
}

GLuint Image::getOGLTextureID() const
{
    return textureId;
}

ImTextureID Image::getOGLImTexture() const
{
    return reinterpret_cast<ImTextureID>(textureId);
}

GLuint Image::makeOGLImage(const SDL_Surface *surfaceTTF)
{
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLenum Mode = GL_RGB;
    if (surfaceTTF->format->BytesPerPixel == 4) {
        Mode = GL_RGBA;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, Mode, surfaceTTF->w, surfaceTTF->h, 0, Mode, GL_UNSIGNED_BYTE, surfaceTTF->pixels);

    return texID;
}

float Image::getAlpha() const {
    return alpha;
}

void Image::setAlpha(float alpha) {
    Image::alpha = alpha;
}
