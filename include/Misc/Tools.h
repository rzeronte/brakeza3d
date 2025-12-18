#ifndef SDL2_3D_ENGINE_TOOLS_H
#define SDL2_3D_ENGINE_TOOLS_H

#include "../3D/Vertex3D.h"
#include "../Render/Point2D.h"
#include "../Components/Camera3D.h"
#include "../3D/Object3D.h"
#include "../3D/LightPoint.h"
#include "../Render/M3.h"
#include "../Render/TextureAnimated.h"
#include "../Render/ParticlesContext.h"
#include <vector>
#include <glm/vec2.hpp>

class Tools {

public:
    static std::vector<std::string> split(const std::string &, char);
    static int random(int min, int max);
    static char *ReadFile(const std::string &name);
    static float getXTextureFromUV(SDL_Surface *surface, float u);
    static float getYTextureFromUV(SDL_Surface *surface, float v);
    static bool FileExists(const char *name);
    static bool isPixelInWindow(int &x, int &y);
    static bool getBit(unsigned char byte, int position);
    static bool isZeroVector(Vertex3D &v);
    static bool saveTextureToFile(GLuint textureID, int width, int height, const char* fileName);
    static bool checkRectangleAABBOverlap(Point2D l1, Point2D r1, Point2D l2, Point2D r2);
    static bool isValidVector(Vertex3D &v);
    static bool RemoveFile(const std::string &filePath);
    static bool CopyFile(const std::string &origen, const std::string &destino);
    static void SurfacePutPixel(const SDL_Surface *surface, int x, int y, Uint32 pixel);
    static void WriteToFile(const std::string& file, const char *content);
    static Color alphaBlend(Uint32 color1, Uint32 color2, Uint32 alpha);
    static Color MixColor(Color color1, Color color2, float color2Intensity);
    static Uint32 getSurfacePixel(SDL_Surface *surface, int x, int y);
    static btTransform GLMMatrixToBulletTransform(const glm::mat4& glmMatrix);
    static Vertex3D screenToWorld(float x, float y, float width, float height, const glm::mat4 &projection, const glm::mat4 &view);
    static glm::mat4 aiMat4toGLMMat4(const aiMatrix4x4t<ai_real> &from);
    static std::string getExtensionFromFilename(const std::string &filename);
    static std::string getFilenameWithoutExtension(const std::string &filename);
    static std::vector<std::string> getFolderFolders(std::string path);
    static std::vector<std::string> getFolderFiles(std::string &path, std::string extension);
    static std::string GoBackFromFolder(const std::string& folder);
    static std::string removeSubstring(const std::string &str, const std::string &toRemove);
    static std::string ImGuiUnique(const std::string &text);
};

#endif //SDL2_3D_ENGINE_TOOLS_H

