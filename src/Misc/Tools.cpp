
#include <string>
#include <vector>
#include <SDL_system.h>
#include <SDL_ttf.h>
#include <algorithm>
#include <math.h>
#include "../../headers/Misc/Tools.h"
#include "../../headers/Render/Transforms.h"
#include "../../headers/Render/Maths.h"
#include "../../headers/EngineSetup.h"
#include "../../headers/Render/Logging.h"

#define MAX_SOURCE_SIZE (0x100000)

std::vector<std::string> Tools::split(const std::string &text, char sep) {
    std::vector<std::string> tokens;
    std::size_t start = 0, end = 0;
    while ((end = text.find(sep, start)) != std::string::npos) {
        tokens.emplace_back(text.substr(start, end - start));
        start = end + 1;
    }
    tokens.emplace_back(text.substr(start));

    return tokens;
}

void Tools::SurfacePutPixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    //Convert the pixels to 32 bit
    Uint32 *pixels = (Uint32 *)surface->pixels;

    //Set the pixel
    pixels[ ( y * surface->w ) + x ] = pixel;
}

bool Tools::isPixelInWindow(int x, int y)
{
    if ( !(x >= 0 && x < EngineSetup::getInstance()->screenWidth && y >= 0 && y < EngineSetup::getInstance()->screenHeight) ) {
        return false;
    }

    return true;
}

unsigned long Tools::createRGB(int r, int g, int b)
{
    return ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
}

uint8_t Tools::getRedValueFromColor(uint32_t c) {
    return ((uint32_t)c >> 16);
}

uint8_t Tools::getGreenValueFromColor(uint32_t c) {
    return ((uint16_t)c >> 8);
}

uint8_t Tools::getBlueValueFromColor(uint32_t c) {
    return (c);
}

const float Tools::getXTextureFromUV(SDL_Surface *surface, float u) {
    return surface->w * u;
}

const float Tools::getYTextureFromUV(SDL_Surface *surface, float v) {
    return surface->h * v;
}

Uint32 Tools::readSurfacePixelFromBilinearUV(SDL_Surface *surface, float u, float v)
{
    float x = Tools::getXTextureFromUV(surface, u);
    float y = Tools::getYTextureFromUV(surface, v);

    int x1 = Tools::int_floor(x);    // current pixel
    int y1 = Tools::int_floor(y);

    if (x1+1 == surface->w || y1+1 == surface->h) {
        return Tools::readSurfacePixel(surface, x, y);
    }

    int x2 = x1 + 1;    // next pixel
    int y2 = y1 + 1;

    Uint8 Q1red, Q1green, Q1blue, Q1alpha;
    Uint8 Q2red, Q2green, Q2blue, Q2alpha;
    Uint8 Q3red, Q3green, Q3blue, Q3alpha;
    Uint8 Q4red, Q4green, Q4blue, Q4alpha;

    Uint32 Q1 = Tools::readSurfacePixel(surface, x1, y1);
    Uint32 Q3 = Tools::readSurfacePixel(surface, x1, y2);
    Uint32 Q2 = Tools::readSurfacePixel(surface, x2, y1);
    Uint32 Q4 = Tools::readSurfacePixel(surface, x2, y2);

    SDL_GetRGBA(Q1, surface->format, &Q1red, &Q1green, &Q1blue, &Q1alpha);
    SDL_GetRGBA(Q2, surface->format, &Q2red, &Q2green, &Q2blue, &Q2alpha);
    SDL_GetRGBA(Q3, surface->format, &Q3red, &Q3green, &Q3blue, &Q3alpha);
    SDL_GetRGBA(Q4, surface->format, &Q4red, &Q4green, &Q4blue, &Q4alpha);

    float f1 = (x2 - x) / (x2 - x1);
    float f2 = (x - x1) / (x2 - x1);

    Uint8 R1red, R1green, R1blue;
    R1red =   (f1 * Q1red)   + (f2 * Q2red);
    R1green = (f1 * Q1green) + (f2 * Q2green);
    R1blue =  (f1 * Q1blue)  + (f2 * Q2blue);

    Uint8 R2red, R2green, R2blue;
    R2red =   (f1 * Q3red)   + (f2 * Q4red);
    R2green = (f1 * Q3green) + (f2 * Q4green);
    R2blue =  (f1 * Q3blue)  + (f2 * Q4blue);

    float fy1 = (y2 - y) / (y2 - y1);
    float fy2 = (y - y1) / (y2 - y1);

    Uint8 Cred, Cgreen, Cblue;
    Cred =   (fy1 * R1red)   + (fy2 * R2red);
    Cgreen = (fy1 * R1green) + (fy2 * R2green);
    Cblue =  (fy1 * R1blue)  + (fy2 * R2blue);

    return (Uint32) Tools::createRGB(Cred, Cgreen, Cblue);
}

Uint32 Tools::readSurfacePixelFromUV(SDL_Surface *surface, float &u, float &v)
{
    return Tools::readSurfacePixel(
        surface,
        Tools::getXTextureFromUV(surface, u),
        Tools::getYTextureFromUV(surface, v)
    );
}

Uint32 Tools::readSurfacePixel(SDL_Surface *surface, int x, int y)
{
    Uint32 *pixels = (Uint32 *)surface->pixels;

    return pixels[ ( y * surface->w ) + x ];
}

bool Tools::fileExists(const std::string& name)
{
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    }

    Logging::getInstance()->Log("File " + name  + " not found", "ERROR");

    return false;
}

char * Tools::readFile(const std::string &name, size_t &source_size)
{
    // Load the kernel source code into the array source_str
    FILE *fp;

    fp = fopen(name.c_str(), "r");

    if (!fp) {
        Logging::getInstance()->Log("File " + name  + " can't be loaded!", "ERROR");
        return "";
    }
    char *file_str = (char*)malloc(MAX_SOURCE_SIZE);

    source_size = fread( file_str, 1, MAX_SOURCE_SIZE, fp);

    fclose( fp );
    return file_str;

}

void Tools::getTextAndRectCenter(SDL_Renderer *renderer, char *text, TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect, Uint32 color)
{
    int renderer_w, renderer_h;
    SDL_GetRendererOutputSize(renderer, &renderer_w, &renderer_h);

    int text_width;
    int text_height;

    SDL_Surface *surface;
    SDL_Color textColor = {(Uint8) Tools::getRedValueFromColor(color), (Uint8) Tools::getGreenValueFromColor(color), (Uint8) Tools::getBlueValueFromColor(color), 0};

    surface = TTF_RenderText_Solid(font, text, textColor);
    *texture = SDL_CreateTextureFromSurface(renderer, surface);

    text_width = surface->w;
    text_height = surface->h;
    SDL_FreeSurface(surface);

    rect->x = renderer_w/2 - text_width/2;
    rect->y = renderer_h/2 - 50; // 20 = offset estético

    rect->w = text_width;
    rect->h = text_height;
}

void Tools::getTextAndRect(SDL_Renderer *renderer, int x, int y, char *text, TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect, Uint32 color)
{
    int renderer_w, renderer_h;
    SDL_GetRendererOutputSize(renderer, &renderer_w, &renderer_h);
    int text_width;
    int text_height;

    SDL_Surface *surface;
    SDL_Color textColor = {(Uint8) Tools::getRedValueFromColor(color), (Uint8) Tools::getGreenValueFromColor(color), (Uint8) Tools::getBlueValueFromColor(color), 0};

    surface = TTF_RenderText_Solid(font, text, textColor);
    *texture = SDL_CreateTextureFromSurface(renderer, surface);

    text_width = surface->w;
    text_height = surface->h;
    SDL_FreeSurface(surface);

    rect->x = (float)x / EngineSetup::getInstance()->screenWidth * renderer_w;
    rect->y = (float)y / EngineSetup::getInstance()->screenHeight * renderer_h;

    rect->w = text_width;
    rect->h = text_height;
}

void Tools::writeText(SDL_Renderer *renderer, TTF_Font *font, int x, int y, Uint32 color, std::string text)
{
    SDL_Texture *textTexture;
    SDL_Rect textRect;
    Tools::getTextAndRect(renderer, x, y, const_cast<char *>(text.c_str()), font, &textTexture, &textRect, color);

    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_DestroyTexture(textTexture);
}

void Tools::writeTextCenter(SDL_Renderer *renderer, TTF_Font *font, Uint32 color, std::string text)
{
    SDL_Texture *textTexture;
    SDL_Rect textRect;
    Tools::getTextAndRectCenter(renderer, const_cast<char *>(text.c_str()), font, &textTexture, &textRect, color);

    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_DestroyTexture( textTexture );
}

void Tools::writeText3D(SDL_Renderer *renderer, Camera3D *cam, TTF_Font *font, Vertex3D v, Uint32 color, std::string text)
{
    Vertex3D tmpV;

    Transforms::cameraSpace( tmpV, v, cam );
    tmpV = Transforms::NDCSpace(tmpV, cam);
    Point2D pointText; Transforms::screenSpace(pointText, tmpV);

    // Las coordenadas que debemos darle, dependen del tamaño de la ventana, ya que el renderer
    // se encuentra sampleado desde el tamaño configurado en EngineSetup.

    int renderer_w, renderer_h;
    SDL_GetRendererOutputSize(renderer, &renderer_w, &renderer_h);

    int real_x = (int) (pointText.x * renderer_w / EngineSetup::getInstance()->screenWidth);
    int real_y = (int) (pointText.y * renderer_h / EngineSetup::getInstance()->screenHeight) ;

    real_x += 5; // Offset estético

    Tools::writeText(renderer, font, real_x, real_y, color, text);
}

float Tools::interpolate(float val, float bound_left, float bound_right)
{
    float Ax = val;                     // componente X de nuestro vértice en PANTALLA2D
    float vNLx = bound_left;            // Límite Izquierdo de PANTALLA2D
    float vNRx = bound_right;           // Límite Derecho de PANTALLA2D
    float tx0 = (Ax - vNLx);            // Distancia entre el límite Izquierdo y nuestro vértice
    float tx1 =  1 / ( vNRx - vNLx);    // Multiplicador (para 2 unidades, rango [0,2])
    float xt =  (tx0 * tx1)  - 1;       // Calculamos el valor entre el rango [0,2], finalmente resta uno, tenemos [-1, 1]

    return xt;
}

bool Tools::getBit(unsigned char byte, int position) // position in range 0-7
{
    return (byte >> position) & 0x1;
}

float Tools::clamp(float n, float lower, float upper)
{
    return std::max(lower, std::min(n, upper));
}


int Tools::random(int min, int max) //range : [min, max)
{
    static bool first = true;
    if (first)
    {
        srand( time(NULL) ); //seeding for the first time only!
        first = false;
    }
    return min + rand() % (( max + 1 ) - min);
}


Vertex3D Tools::wedge(Vertex3D v1, Vertex3D v2)
{
    Vertex3D result;

    result.x = (v1.y * v2.z) - (v2.y * v1.z);
    result.y = (v1.z * v2.x) - (v2.z * v1.x);
    result.z = (v1.x * v2.y) - (v2.x * v1.y);

    return (result);
}

int Tools::classifyPoint(Vertex3D point, Vertex3D pO, Vertex3D pN) {

    Vertex3D dir = pO - point;
    double d = (dir * pN);

    if (d<-0.001f)
        return PLANE_FRONT;
    else
    if (d>0.001f)
        return PLANE_BACKSIDE;

    return ON_PLANE;
}

bool Tools::isZeroVector(Vertex3D& v)
{
    if ((v.x == 0.0f) && (v.y == 0.0f) && (v.z == 0.0f)) {
        return true;
    }

    return false;
}

bool Tools::isValidVector(Vertex3D& v)
{
    if (isnan(v.x) || isnan(v.y == v.y) || isnan(v.z)) {
        return false;
    }

    return true;
}

// Returns true if two rectangles (l1, r1) and (l2, r2) overlap
bool Tools::checkRectangleAABBOverlap(Point2D l1, Point2D r1, Point2D l2, Point2D r2)
{
    int Axmin = l1.x;
    int Axmax = r1.x;
    int Aymax = r1.y;
    int Aymin = l1.y;

    int Bxmin = l2.x;
    int Bxmax = r2.x;
    int Bymax = r2.y;
    int Bymin = l2.y;

    // Collision check
    if(Axmax < Bxmin || Axmin > Bxmax ) return false;
    if(Aymax < Bymin || Aymin > Bymax ) return false;

    return true;
}

std::string Tools::floatTruncate(float val, int numDigits)
{
    std::string output = std::to_string(val).substr(0, numDigits+1);
    if (output.find('.') ==  std::string::npos ||
        output.back() == '.')
    {
        output.pop_back();
    }
    return output;
}

void Tools::playMixedSound(Mix_Chunk *chunk, int channel, int times)
{
    if (chunk == NULL) return;

    Mix_PlayChannel(channel, chunk, times );
}

void Tools::writeTextCenterHorizontal(SDL_Renderer *renderer, TTF_Font *font, Uint32 color, std::string text, int ypos)
{
    SDL_Texture *textTexture;
    SDL_Rect textRect;
    Tools::getTextAndRectCenterHorizontal(renderer, const_cast<char *>(text.c_str()), font, &textTexture, &textRect, color, ypos);

    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_DestroyTexture( textTexture );

}

void Tools::getTextAndRectCenterHorizontal(SDL_Renderer *renderer, char *text, TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect, Uint32 color, int ypos)
{
    int renderer_w, renderer_h;
    SDL_GetRendererOutputSize(renderer, &renderer_w, &renderer_h);

    int text_width;
    int text_height;

    SDL_Surface *surface;
    SDL_Color textColor = {(Uint8) Tools::getRedValueFromColor(color), (Uint8) Tools::getGreenValueFromColor(color), (Uint8) Tools::getBlueValueFromColor(color), 0};

    surface = TTF_RenderText_Solid(font, text, textColor);
    *texture = SDL_CreateTextureFromSurface(renderer, surface);

    text_width = surface->w;
    text_height = surface->h;
    SDL_FreeSurface(surface);

    rect->x = renderer_w/2 - text_width/2;
    rect->y = ypos;

    rect->w = text_width;
    rect->h = text_height;
}


Uint32 Tools::alphaBlend(Uint32 color1, Uint32 color2, Uint32 alpha) {
    Uint32 rb = color1 & 0xff00ff;
    Uint32 g  = color1 & 0x00ff00;
    rb += ((color2 & 0xff00ff) - rb) * alpha >> 8;
    g  += ((color2 & 0x00ff00) -  g) * alpha >> 8;

    return (rb & 0xff00ff) | (g & 0xff00);
}

Uint32 Tools::mixLightColor(Uint32 color, float distance, LightPoint3D *lp, Vertex3D Q)
{

    Vertex3D P = *lp->getPosition();
    Vertex3D R = lp->AxisForward();

    Vector3D L = Vector3D(P, Q);
    Vertex3D Lv = L.normal();

    const float min = R * Lv;

    float p = 100;
    float max = fmaxf(min, 0);
    float pow = powf(max, p);

    float intensity = pow / (lp->kc + lp->kl*distance + lp->kq * (distance * distance));

    int r_light = (int) (Tools::getRedValueFromColor(lp->color)   * intensity);
    int g_light = (int) (Tools::getGreenValueFromColor(lp->color) * intensity);
    int b_light = (int) (Tools::getBlueValueFromColor(lp->color)  * intensity);

    int r_original = (int) (Tools::getRedValueFromColor(color) * ( 1 - intensity) );
    int g_original = (int) (Tools::getGreenValueFromColor(color) * ( 1 - intensity) );
    int b_original = (int) (Tools::getBlueValueFromColor(color) * ( 1 - intensity) );

    Uint32 c = Tools::createRGB(
            r_light + r_original,
            g_light + g_original,
            b_light + b_original
    );

    return c;
}

Uint32 Tools::mixColor(Uint32 color1, Uint32 color2, float color2Intensity)
{

    int r_light = Tools::getRedValueFromColor(color2)   * color2Intensity;
    int g_light = Tools::getGreenValueFromColor(color2) * color2Intensity;
    int b_light = Tools::getBlueValueFromColor(color2)  * color2Intensity;

    int r_original = Tools::getRedValueFromColor(color1)   * (1 - color2Intensity);
    int g_original = Tools::getGreenValueFromColor(color1) * (1 - color2Intensity);
    int b_original = Tools::getBlueValueFromColor(color1)  * (1 - color2Intensity);

    return Tools::createRGB(
    r_light + r_original,
    g_light + g_original,
    b_light + b_original
    );
}

int Tools::int_floor(float x)
{
    int i = (int)x; /* truncate */
    return i - ( i > x ); /* convert trunc to floor */
}