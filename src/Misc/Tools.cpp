#define GL_GLEXT_PROTOTYPES

#include <string>
#include <ctime>
#include <vector>
#include <SDL2/SDL_system.h>
#include <algorithm>
#include <fstream>
#include "../../include/Misc/Tools.h"
#include "../../include/EngineSetup.h"
#include "../../include/LUAManager.h"
#include "../../include/Render/Logging.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"
#include "../../include/Render/Transforms.h"
#include "../../include/2D/Image2DAnimation.h"
#include "../../include/Objects/Mesh3DAnimation.h"
#include "../../include/Objects/ParticleEmitter.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

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

void Tools::SurfacePutPixel(SDL_Surface *surface, int x, int y, Uint32 pixel) {
    //Convert the pixels to 32 bit
    auto *pixels = (Uint32 *) surface->pixels;

    //Set the pixel
    pixels[(y * surface->w) + x] = pixel;
}

bool Tools::isPixelInWindow(int &x, int &y) {
    if (x <= 0 || x >= EngineSetup::get()->screenWidth) return false;
    if (y <= 0 || y >= EngineSetup::get()->screenHeight) return false;

    return true;
}

float Tools::getXTextureFromUV(SDL_Surface *surface, float u) {
    return surface->w * u;
}

float Tools::getYTextureFromUV(SDL_Surface *surface, float v) {
    return surface->h * v;
}

bool Tools::fileExists(const char *name)
{
    if (FILE *file = fopen(name, "r")) {
        fclose(file);
        return true;
    }

    Logging::Log("File %s not found", name);

    return false;
}

char *Tools::readFile(const std::string &name, size_t &source_size)
{
    FILE *fp;

    fp = fopen(name.c_str(), "rb");

    if (!fp) {
        perror(("Error opening file " + name).c_str());
        return nullptr;
    }

    fseek(fp, 0, SEEK_END);
    source_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (source_size <= 0) {
        Logging::Message("File %s is empty or has an error!", name.c_str());
        fclose(fp);
        return nullptr;
    }

    char *file_str = (char *)malloc(source_size + 1);

    if (!file_str) {
        Logging::Message("Memory allocation failed!");
        fclose(fp);
        return nullptr;
    }

    if (fread(file_str, 1, source_size, fp) != source_size) {
        Logging::Message("Error reading file %s!", name.c_str());
        std::cout << "Error reading file :" << name.c_str() << std::endl;
        fclose(fp);
        free(file_str);
        return nullptr;
    }

    file_str[source_size] = '\0';

    fclose(fp);

    return file_str;
}

float Tools::interpolate(float val, float bound_left, float bound_right) {
    float Ax = val;                   // componente X de nuestro vértice en PANTALLA2D
    float vNLx = bound_left;          // Límite Izquierdo de PANTALLA2D
    float vNRx = bound_right;         // Límite Derecho de PANTALLA2D
    float tx0 = (Ax - vNLx);          // Distancia entre el límite Izquierdo y nuestro vértice
    float tx1 = 1 / (vNRx - vNLx);    // Multiplicador (para 2 unidades, rango [0,2])
    float xt = (tx0 * tx1) - 1;       // Calculamos el valor entre el rango [0,2], finalmente resta uno, tenemos [-1, 1]

    return xt;
}

bool Tools::getBit(unsigned char byte, int position) // position in range 0-7
{
    return (byte >> position) & 0x1;
}

float Tools::clamp(float n, float lower, float upper) {
    return std::max(lower, std::min(n, upper));
}


int Tools::random(int min, int max) //range : [min, max)
{
    static bool first = true;
    if (first) {
        srand(time(nullptr)); //seeding for the first time only!
        first = false;
    }
    return min + rand() % ((max + 1) - min);
}

bool Tools::isZeroVector(Vertex3D &v) {
    if ((v.x == 0.0f) && (v.y == 0.0f) && (v.z == 0.0f)) {
        return true;
    }

    return false;
}

bool Tools::isValidVector(Vertex3D &v) {
    if (fpclassify(v.x) == FP_NAN || fpclassify(v.y) == FP_NAN || fpclassify(v.z) == FP_NAN) {
        return false;
    }

    return true;
}

// Returns true if two rectangles (l1, r1) and (l2, r2) overlap
bool Tools::checkRectangleAABBOverlap(Point2D l1, Point2D r1, Point2D l2, Point2D r2) {
    int Axmin = l1.x;
    int Axmax = r1.x;
    int Aymax = r1.y;
    int Aymin = l1.y;

    int Bxmin = l2.x;
    int Bxmax = r2.x;
    int Bymax = r2.y;
    int Bymin = l2.y;

    // Collision check
    if (Axmax < Bxmin || Axmin > Bxmax) return false;
    if (Aymax < Bymin || Aymin > Bymax) return false;

    return true;
}

Color Tools::alphaBlend(Uint32 color1, Uint32 color2, Uint32 alpha) {
    Uint32 rb = color1 & 0xff00ff;
    Uint32 g = color1 & 0x00ff00;
    rb += ((color2 & 0xff00ff) - rb) * alpha >> 8;
    g += ((color2 & 0x00ff00) - g) * alpha >> 8;

    return Color((rb & 0xff00ff) | (g & 0xff00));
}

Color Tools::mixColor(Color a, Color b, float f) {
    return (a * (1.0f - f)) + (b * f);
}

void Tools::consoleVec3(vec3_t v, const std::string& name) {
    printf("%s: %f %f %f\r\n", name.c_str(), v[0], v[1], v[2]);
}

Uint32 Tools::getSurfacePixel(SDL_Surface *surface, int x, int y) {
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *) surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp) {
        case 1:
            return *p;
            break;
        case 2:
            return *(Uint16 *) p;
            break;
        case 3:
            return p[0] | p[1] << 8 | p[2] << 16;
            break;
        case 4:
            return *(Uint32 *) p;
            break;
        default:
            return 0;       /* shouldn't happen, but avoids warnings */
    }
}

void Tools::LoadPathFinderWithGrid3D(Grid3D *grid, PathFinder *pathfinder) {
    for (int x = 0; x < grid->numberCubesX; x++) {
        for (int y = 0; y < grid->numberCubesZ; y++) {
            CubeGrid3D *c = grid->getFromPosition(x, 0, y); // grid de altura 0
            pathfinder->setValue(y, x, c->is_empty);
        }
    }
}

std::vector<Vertex3D> Tools::getVerticesFromPathFinderPath(Grid3D *grid, std::stack<PairData> path) {
    std::vector<Vertex3D> result;

    while (!path.empty()) {
        std::pair<int, int> p = path.top();
        path.pop();

        int x = p.first;
        int y = 0;
        int z = p.second;

        CubeGrid3D *cube = grid->getFromPosition(x, y, z);
        result.push_back(cube->box->getCenter());
    }

    return result;
}

btMatrix3x3 Tools::M3ToBulletM3(M3 m)
{
    return btMatrix3x3(m.m[0],m.m[1],m.m[2],m.m[3],m.m[4],m.m[5],m.m[6],m.m[7],m.m[8]);
}

btTransform Tools::GLMMatrixToBulletTransform(const glm::mat4& glmMatrix)
{
    btTransform bulletTransform;
    btMatrix3x3 bulletRotationMatrix(
        glmMatrix[0][0], glmMatrix[1][0], glmMatrix[2][0],
        glmMatrix[0][1], glmMatrix[1][1], glmMatrix[2][1],
        glmMatrix[0][2], glmMatrix[1][2], glmMatrix[2][2]
    );
    bulletTransform.setBasis(bulletRotationMatrix);
    bulletTransform.setOrigin(btVector3(glmMatrix[3][0], glmMatrix[3][1], glmMatrix[3][2]));
    return bulletTransform;
}

M3 Tools::GLMMatrixToM3(const glm::mat3& glmMatrix)
{
    return {
        glmMatrix[0][0], glmMatrix[1][0], glmMatrix[2][0],
        glmMatrix[0][1], glmMatrix[1][1], glmMatrix[2][1],
        glmMatrix[0][2], glmMatrix[1][2], glmMatrix[2][2]
    };
}

M3 Tools::BulletM3ToM3(const btMatrix3x3& m) {
    return M3(
            m.getRow(0).getX(), m.getRow(0).getY(), m.getRow(0).getZ(),
            m.getRow(1).getX(), m.getRow(1).getY(), m.getRow(1).getZ(),
            m.getRow(2).getX(), m.getRow(2).getY(), m.getRow(2).getZ()
    );
}

Vertex3D Tools::randomVertex() {
    return Vertex3D(
            Tools::random(-2, 2),
            Tools::random(-2, 2),
            Tools::random(-2, 2)
    );
}

const char *Tools::sprintf(const char *format, ...)
{
    va_list args;
    va_start (args, format);
    vsnprintf (LUAManager::get()->text, 255, format, args);

    va_end (args);

    return LUAManager::get()->text;
}

float Tools::percentage(int value, int total)
{
    if (total == 0) {
        return 0;
    }

    return (float) ((float) value * 100 / (float) total);
}


void Tools::addSceneObject(const std::string& filename, const std::string& name)
{
    Vertex3D position = ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition();

    auto *newObject = new Mesh3DAnimation();
    newObject->setBelongToScene(true);
    newObject->setPosition(position);
    newObject->AssimpLoadAnimation(filename);

    Logging::Message("Loading from file: %s", std::string(filename).c_str());

    Brakeza3D::get()->addObject3D(newObject, Brakeza3D::uniqueObjectLabel(name.c_str()));
}

void Tools::makeFadeInSprite(Vertex3D position, TextureAnimated *animation)
{
    Point2D P1 = Transforms::WorldToPoint(position);

    Brakeza3D::get()->addObject3D(
            new Image2DAnimation(P1.x, P1.y, true, new TextureAnimated(animation)),
            Brakeza3D::uniqueObjectLabel("Sprite2DLive")
    );
}

void Tools::makeLoopSprite(Vertex3D position, TextureAnimated *animation, float ttl)
{
    Point2D P1 = Transforms::WorldToPoint(position);

    Brakeza3D::get()->addObject3D(
            new Image2DAnimation(P1.x, P1.y, ttl, new TextureAnimated(animation)),
            Brakeza3D::uniqueObjectLabel("fadeInSpriteExplosion")
    );
}

std::string Tools::getExtensionFromFilename(const std::string& filename)
{
    size_t dotPosition = filename.find_last_of(".");
    if (dotPosition != std::string::npos) {
        return filename.substr(dotPosition + 1);
    }

    return "";
}

std::string Tools::getFilenameWithoutExtension(const std::string& filename)
{
    size_t dotPosition = filename.find_last_of(".");
    if (dotPosition != std::string::npos) {
        return filename.substr(0, dotPosition);
    }

    return filename;
}

void Tools::writeToFile(const std::string& fileName, const char *content)
{
    Logging::Message("Writing to file %s!", fileName.c_str());

    std::ofstream file(fileName, std::ios::trunc | std::ios::binary); // Abrir en modo binario

    if (!file.is_open()) {
        Logging::Message("File %s can't be loaded!", fileName.c_str());
        return;
    }

    // Utiliza '\n' como salto de línea, independientemente del sistema operativo
    while (*content) {
        if (*content == '\r\n') {
            file.put('\r'); // Agrega un '\r' antes de cada '\n' para Windows
        }
        file.put(*content++);
    }

    file.close();

    if (file.fail()) {
        Logging::Message("Error writing to file %s", fileName.c_str());
        return;
    }
}

Vertex3D Tools::screenToWorld(float x, float y, float screenWidth, float screenHeight, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix) {
    // Escalar las coordenadas de pantalla a las dimensiones deseadas (de 0 a 1)
    float normalizedX = x / screenWidth;
    float normalizedY = 1.0f - y / screenHeight;

    // Mapear las coordenadas normalizadas al espacio de la cámara
    float viewX = normalizedX * 2.0f - 1.0f;
    float viewY = normalizedY * 2.0f - 1.0f;

    // Crear un vector de clip
    glm::vec4 clipCoords(viewX, viewY, -1.0f, 1.0f);

    // Obtener la matriz inversa de la matriz de proyección
    glm::mat4 inverseProjection = glm::inverse(projectionMatrix);

    // Obtener las coordenadas de vista
    glm::vec4 eyeCoords = inverseProjection * clipCoords;
    eyeCoords.z = -1.0f;
    eyeCoords.w = 1.0f;

    // Obtener la matriz inversa de la matriz de vista
    glm::mat4 inverseView = glm::inverse(viewMatrix);

    // Obtener las coordenadas del mundo
    glm::vec4 rayWorld = inverseView * eyeCoords;
    glm::vec3 rayDirection = glm::normalize(glm::vec3(rayWorld));

    return Vertex3D(rayDirection.x, rayDirection.y, rayDirection.z);
}

// Función para guardar el contenido de una textura OpenGL en un archivo de imagen
bool Tools::saveTextureToFile(GLuint textureID, int width, int height, const char* fileName) {
    // Crear un surface para copiar el contenido de la textura
    SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

    // Verificar si se creó el surface correctamente
    if (surface == nullptr) {
        SDL_Log("Error al crear el surface: %s", SDL_GetError());
        return false;
    }

    // Guardar el estado actual de OpenGL
    GLint previousFramebuffer;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFramebuffer);

    // Crear un framebuffer temporal y asociar la textura a él
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

    // Leer los píxeles de la textura al surface
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

    // Restaurar el estado previo de OpenGL
    glBindFramebuffer(GL_FRAMEBUFFER, previousFramebuffer);
    glDeleteFramebuffers(1, &framebuffer);

    // Invertir los píxeles verticalmente para que coincida con la orientación de la imagen guardada
    int pitch = surface->pitch;
    void* pixels = surface->pixels;
    for (int i = 0; i < height / 2; ++i) {
        void* top = static_cast<uint8_t*>(pixels) + i * pitch;
        void* bottom = static_cast<uint8_t*>(pixels) + (height - 1 - i) * pitch;
        std::swap_ranges(static_cast<uint8_t*>(top), static_cast<uint8_t*>(top) + pitch, static_cast<uint8_t*>(bottom));
    }

    // Guardar el surface como una imagen
    if (IMG_SavePNG(surface, fileName) != 0) {
        SDL_Log("Error al guardar la imagen: %s", IMG_GetError());
        SDL_FreeSurface(surface);
        return false;
    }

    // Liberar el surface
    SDL_FreeSurface(surface);

    return true;
}

std::vector<std::string> Tools::getFolderFiles(const std::string& path, const std::string& extension)
{
    std::vector<std::string> result;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (path.c_str())) != nullptr) {
        while ((ent = readdir (dir)) != nullptr) {
            auto fileName = ent->d_name;

            if (Tools::getExtensionFromFilename(ent->d_name) != extension) continue;
            if (strcmp(fileName, ".") == 0 || strcmp(fileName, "..") == 0) continue;

            result.emplace_back(ent->d_name);
        }
        std::sort( result.begin(), result.end() );
        closedir (dir);
    }

    return result;
}

std::vector<std::string> Tools::getFolderFolders(const std::string& path)
{
    std::vector<std::string> result;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (path.c_str())) != nullptr) {
        while ((ent = readdir (dir)) != nullptr) {
            auto fileName = ent->d_name;

            if (strcmp(fileName, ".") == 0 || strcmp(fileName, "..") == 0) continue;
            std::string fullPath = path + "/" + fileName;

            struct stat fileStat;
            if (stat(fullPath.c_str(), &fileStat) == 0) {
                if (S_ISDIR(fileStat.st_mode)) {
                    result.emplace_back(fileName);
                }
            }
        }
        std::sort( result.begin(), result.end() );

        closedir (dir);
    }

    return result;
}