#ifndef SDL2_3D_ENGINE_TOOLS_H
#define SDL2_3D_ENGINE_TOOLS_H

#include "../Objects/Vertex3D.h"
#include "../Objects/Point2D.h"
#include "../Components/Camera3D.h"
#include "../Render/Frustum.h"
#include "../Objects/Object3D.h"
#include "../Objects/Vector3D.h"
#include "../Objects/LightPoint3D.h"
#include "../Render/M3.h"
#include "Grid3D.h"
#include "PathFinder.h"
#include "../2D/TextureAnimated.h"
#include "../Objects/OpenCLTypes.h"
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

typedef float vec3_t[3];

class Tools {

public:
    static std::vector<std::string> split(const std::string &, char);

    static void SurfacePutPixel(SDL_Surface *surface, int x, int y, Uint32 pixel);

    static float getXTextureFromUV(SDL_Surface *surface, float u);

    static float getYTextureFromUV(SDL_Surface *surface, float v);

    static bool fileExists(const char *name);

    static char *readFile(const std::string &name, size_t &source_size);

    static bool isPixelInWindow(int &x, int &y);

    static float interpolate(float val, float bound_left, float bound_right);

    static bool getBit(unsigned char byte, int position);

    static float clamp(float n, float lower, float upper);

    static int random(int min, int max); //range : [min, max)

    static bool isZeroVector(Vertex3D &v);

    static bool isValidVector(Vertex3D &v);

    static bool checkRectangleAABBOverlap(Point2D l1, Point2D r1, Point2D l2, Point2D r2);

    static Color alphaBlend(Uint32 color1, Uint32 color2, Uint32 alpha);

    static Color mixColor(Color color1, Color color2, float color2Intensity);

    static void consoleVec3(vec3_t v, const std::string&);

    static Uint32 getSurfacePixel(SDL_Surface *surface, int x, int y);

    static void LoadPathFinderWithGrid3D(Grid3D *, PathFinder *);

    static std::vector<Vertex3D> getVerticesFromPathFinderPath(Grid3D *, std::stack<PairData> path);

    static btMatrix3x3 M3ToBulletM3(M3 m);

    static btTransform GLMMatrixToBulletTransform(const glm::mat4& glmMatrix);

    static M3 BulletM3ToM3(const btMatrix3x3& m);

    static Vertex3D randomVertex();

    static const char *sprintf(const char *, ...);

    static float percentage(int value, int total);

    static void addSceneObject(const std::string& filename, const std::string& name);

    static void makeFadeInSprite(Vertex3D position, TextureAnimated *animation);

    static void makeLoopSprite(Vertex3D position, TextureAnimated *animation, float ttl);

    static std::string getExtensionFromFilename(const std::string &filename);

    static void writeToFile(const std::string& file, const char *content);

    static Vertex3D screenToWorld(float x, float y, float screenWidth, float screenHeight, const glm::mat4 &projectionMatrix, const glm::mat4 &viewMatrix);

    static bool saveTextureToFile(GLuint textureID, int width, int height, const char* fileName);

    static std::string getFilenameWithoutExtension(const std::string &filename);

    static std::vector<std::string> getFolderFolders(const std::string &path);

    static std::vector<std::string> getFolderFiles(const std::string &path, const std::string &extension);
};

#endif //SDL2_3D_ENGINE_TOOLS_H

