//
// Created by darkhead on 8/1/20.
//

#ifndef BRAKEDA3D_COMPONENTRENDER_H
#define BRAKEDA3D_COMPONENTRENDER_H

#include <vector>
#include <mutex>
#include "Component.h"
#include "../Objects/Triangle3D.h"
#include "../../include/Render/Drawable.h"
#include "../../include/Render/Maths.h"
#include "../2D/TextWriter.h"
#include "../Misc/SharedLUAContext.h"
#include "../Misc/SceneLoader.h"
#include "../OpenGL/ShaderOpenGLCustom.h"
#include "../Misc/ProjectLoader.h"
#include "../OpenGL/ShaderOpenGL.h"
#include "../OpenGL/ShaderOpenGLImage.h"
#include "../OpenGL/ShaderOpenGLRender.h"
#include "../OpenGL/ShaderOpenGLLine.h"
#include "../OpenGL/ShaderOpenGLWireframe.h"
#include "../OpenGL/ShaderOpenGLShading.h"
#include "../OpenGL/ShaderOpenGLPoints.h"
#include "../OpenGL/ShaderOpenGLOutline.h"
#include "../OpenGL/ShaderOpenGLColor.h"
#include "../OpenGL/ShaderOpenGLParticles.h"
#include "../OpenGL/ShaderOpenGLDOF.h"
#include "../OpenGL/ShaderOpenGLDepthMap.h"
#include "../OpenGL/ShaderOpenGLFOG.h"
#include "../OpenGL/ShaderOpenGLTint.h"
#include "../OpenGL/ShaderOpenGLLine3D.h"
#include "../OpenGL/ShaderOpenGLCustom.h"

class ComponentRender : public Component {
private:

    int fps;
    int fpsFrameCounter;
    float frameTime;

    Object3D *selectedObject;

    TextWriter *textWriter;
    SceneLoader sceneLoader;
    ProjectLoader projectLoader;

    bool sceneShadersEnabled;
    std::vector<ShaderOpenGLCustom*> sceneShaders;

    ShaderOpenGLRender *shaderOGLRender;
    ShaderOpenGLImage *shaderOGLImage;
    ShaderOpenGLLine *shaderOGLLine;
    ShaderOpenGLWireframe *shaderOGLWireframe;
    ShaderOpenGLLine3D *shaderOGLLine3D;
    ShaderOpenGLShading *shaderOGLShading;
    ShaderOpenGLPoints *shaderOGLPoints;
    ShaderOpenGLOutline *shaderOGLOutline;
    ShaderOpenGLColor *shaderOGLColor;
    ShaderOpenGLParticles *shaderOGLParticles;
    ShaderOpenGLDOF *shaderOGLDOF;
    ShaderOpenGLDepthMap *shaderOGLDepthMap;
    ShaderOpenGLFOG *shaderOGLFOG;
    ShaderOpenGLTint *shaderOGLTint;

    GLuint lastFrameBufferUsed;
    GLuint lastProgramUsed;
public:
    ComponentRender();

    ~ComponentRender() override;

    void onStart() override;

    void preUpdate() override;

    void onUpdate() override;

    void postUpdate() override;

    void onEnd() override;

    void onSDLPollEvent(SDL_Event *event, bool &finish) override;

    static void onUpdateSceneObjects();

    void updateFPS();

    [[nodiscard]] Object3D* getSelectedObject() const;

    void setSelectedObject(Object3D *o);

    static Object3D *getObject3DFromClickPoint(int xClick, int yClick);

    void updateSelectedObject3D();

    [[nodiscard]] int getFps() const;

    static void deleteRemovedObjects();

    SceneLoader &getSceneLoader();

    ProjectLoader &getProjectLoader();

    std::vector<ShaderOpenGLCustom *> &getSceneShaders();

    void addShaderToScene(ShaderOpenGLCustom *shader);

    [[nodiscard]] bool isSceneShadersEnabled() const;

    void setSceneShadersEnabled(bool sceneShadersEnabled);

    void runShadersOpenCLPostUpdate();

    void removeShader(int index);

    void runShadersOpenCLPreUpdate();

    ShaderOpenGLCustom *getSceneShaderByIndex(int i);

    ShaderOpenGLCustom *getSceneShaderByLabel(const std::string& name);

    static bool compareDistances(Object3D *obj1, Object3D *obj2);

    void setGlobalIlluminationDirection(Vertex3D d) const;

    void setGlobalIlluminationAmbient(Vertex3D a) const;

    void setGlobalIlluminationDiffuse(Vertex3D d) const;

    void setGlobalIlluminationSpecular(Vertex3D s) const;

    void drawLine(Vertex3D from, Vertex3D to, Color c) const;

    [[nodiscard]] ShaderOpenGLLine3D *getShaderOGLLine3D() const;

    [[nodiscard]] ShaderOpenGLImage *getShaderOGLImage() const;

    [[nodiscard]] ShaderOpenGLRender *getShaderOGLRender() const;

    [[nodiscard]] ShaderOpenGLLine *getShaderOGLLine() const;

    [[nodiscard]] ShaderOpenGLWireframe *getShaderOGLWireframe() const;

    [[nodiscard]] ShaderOpenGLShading *getShaderOGLShading() const;

    [[nodiscard]] ShaderOpenGLPoints *getShaderOGLPoints() const;

    [[nodiscard]] ShaderOpenGLOutline *getShaderOGLOutline() const;

    [[nodiscard]] ShaderOpenGLColor *getShaderOGLColor() const;

    [[nodiscard]] ShaderOpenGLParticles *getShaderOGLParticles() const;

    [[nodiscard]] ShaderOpenGLDOF *getShaderOGLDOF() const;

    [[nodiscard]] ShaderOpenGLFOG *getShaderOGLFOG() const;

    [[nodiscard]] ShaderOpenGLTint *getShaderOGLTint() const;

    [[nodiscard]] ShaderOpenGLDepthMap *getShaderOGLDepthMap() const;

    GLuint getLastFrameBufferUsed();

    void setLastFrameBufferUsed(GLuint lastFrameBufferUsed);

    GLuint getLastProgramUsed() const;

    void setLastProgramUsed(GLuint lastProgramUsed);

    void changeOpenGLFramebuffer(GLuint);

    void changeOpenGLProgram(GLuint);
};


#endif //BRAKEDA3D_COMPONENTRENDER_H
