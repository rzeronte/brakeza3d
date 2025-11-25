#ifndef SDL2_3D_ENGINE_TOOLS_H
#define SDL2_3D_ENGINE_TOOLS_H

#include "../Objects/Vertex3D.h"
#include "../2D/Point2D.h"
#include "../Components/Camera3D.h"
#include "../Objects/Object3D.h"
#include "../Objects/LightPoint.h"
#include "../Render/M3.h"
#include "../2D/TextureAnimated.h"
#include "../Objects/ParticlesContext.h"
#include <vector>
#include <glm/vec2.hpp>

class Tools {

public:
    static std::vector<std::string> split(const std::string &, char);
    static int random(int min, int max); //range : [min, max)
    static char *readFile(const std::string &name, size_t &source_size);
    static float interpolate(float val, float bound_left, float bound_right);
    static float clamp(float n, float lower, float upper);
    static float percentage(int value, int total);
    static float getXTextureFromUV(SDL_Surface *surface, float u);
    static float getYTextureFromUV(SDL_Surface *surface, float v);
    static bool fileExists(const char *name);
    static bool isPixelInWindow(int &x, int &y);
    static bool getBit(unsigned char byte, int position);
    static bool isZeroVector(Vertex3D &v);
    static bool saveTextureToFile(GLuint textureID, int width, int height, const char* fileName);
    static bool checkRectangleAABBOverlap(Point2D l1, Point2D r1, Point2D l2, Point2D r2);
    static bool isValidVector(Vertex3D &v);
    static Color alphaBlend(Uint32 color1, Uint32 color2, Uint32 alpha);
    static Color mixColor(Color color1, Color color2, float color2Intensity);
    static Uint32 getSurfacePixel(SDL_Surface *surface, int x, int y);
    static btTransform GLMMatrixToBulletTransform(const glm::mat4& glmMatrix);
    static Vertex3D screenToWorld(float x, float y, float screenWidth, float screenHeight, const glm::mat4 &projectionMatrix, const glm::mat4 &viewMatrix);
    static glm::mat4 aiMat4toGLMMat4(const aiMatrix4x4t<ai_real> &from);
    static std::string getExtensionFromFilename(const std::string &filename);
    static std::string getFilenameWithoutExtension(const std::string &filename);
    static std::vector<std::string> getFolderFolders(const std::string &path);
    static std::vector<std::string> getFolderFiles(const std::string &path, const std::string &extension);
    static std::string GoBackFromFolder(const std::string& folder);
    static std::string removeSubstring(const std::string &str, const std::string &toRemove);
    static void ImGuiVertex3D(const char *label, const char *labelX, const char *labelY, const char *labelZ, void *data, float sensibility, float min, float max);
    static void SurfacePutPixel(const SDL_Surface *surface, int x, int y, Uint32 pixel);
    static void writeToFile(const std::string& file, const char *content);
    static bool removeFile(const std::string &filePath);
    static bool copyFile(const std::string &origen, const std::string &destino);
};

#endif //SDL2_3D_ENGINE_TOOLS_H

