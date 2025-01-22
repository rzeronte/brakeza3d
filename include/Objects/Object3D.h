
//#define SOL_ALL_SAFETIES_ON 1

#define SOL_SAFE_USERTYPE 1
#define SOL_SAFE_REFERENCES  0
#define SOL_SAFE_NUMERICS 1
#define SOL_SAFE_GETTER 0
#define SOL_SAFE_FUNCTION_CALLS 0

#ifndef SDL2_3D_ENGINE_OBJECT3D_H
#define SDL2_3D_ENGINE_OBJECT3D_H

#include <string>
#include <vector>
#include <glm/detail/type_mat4x4.hpp>
#include "Vertex3D.h"
#include "Vector3D.h"
#include "../../include/Render/M3.h"
#include "../Behaviors/Object3DBehavior.h"
#include "Point2D.h"
#include "../Misc/cJSON.h"
#include "ScriptLUA.h"
#include "../../sol/sol.hpp"
#include "../OpenGL/FXEffectOpenGL.h"
#include "../src/Collisions/Collider.h"

struct ObjectGUIFeatures {
    bool position = true;
    bool rotation = true;
    bool scale = true;
    bool alpha = true;
    bool shaders = true;
    bool attached = true;
    bool collider = true;
};

class Object3D: public Collider {

protected:
    Vertex3D drawOffset;
    M3 rotation;

    Object3DBehavior *motion;
    Object3D *parent;

    ObjectGUIFeatures featuresGUI;

    bool removed;
    bool decal; // Decals exclude UV Coordinates out of [0, 1]
    bool followCamera;
    bool belongToScene;
    bool multiScene;

    std::string label;
    float rotX, rotY, rotZ; // For easy management from UI
    bool alphaEnabled;
    float alpha;

    std::vector<ScriptLUA*> scripts;
    std::vector<Object3D*> attachedObjects;
    sol::environment luaEnvironment;
    std::vector<FXEffectOpenGL*> shaders;

    Timer timer;
    float distanceToCamera;
public:
    Vertex3D position;
    Vertex3D rotationFrame;
    bool enableLights;
    float scale;
    bool rotationFrameEnabled;

    Object3D();

    virtual ~Object3D();

    void setParent(Object3D *object);

    void setLabel(const std::string& label);

    Vertex3D &getPosition();

    M3 getRotation();

    void setPosition(Vertex3D p);

    void addToPosition(Vertex3D p);

    void setRotation(M3 r);

    void setRotation(float, float, float);

    virtual void setEnabled(bool enabled);

    Vertex3D AxisUp();

    Vertex3D AxisDown();

    Vertex3D AxisForward();

    Vertex3D AxisBackwards();

    Vertex3D AxisRight();

    Vertex3D AxisLeft();

    void setScale(float value);

    void setRemoved(bool value);

    void setDecal(bool value);

    void setDrawOffset(Vertex3D v);

    void setFollowCamera(bool followCamera);

    virtual void onUpdate();

    virtual void postUpdate();

    [[nodiscard]] bool isRotationFrameEnabled() const;

    void setRotationFrameEnabled(bool value);

    void setRotationFrame(Vertex3D v);

    void setAlpha(float alpha);

    void setAlphaEnabled(bool alphaEnabled);

    void setBehavior(Object3DBehavior *motion);

    float &getRotX();

    float &getRotY();

    float &getRotZ();

    [[nodiscard]] Object3DBehavior *getBehavior() const;

    [[nodiscard]] bool &isAlphaEnabled();

    [[nodiscard]] Vertex3D &getDrawOffset();

    [[nodiscard]] float &getAlpha();

    [[nodiscard]] bool &isFollowCamera();

    [[nodiscard]] bool isDecal() const;

    [[nodiscard]] float getScale() const;

    [[nodiscard]] bool isRemoved() const;

    [[nodiscard]] bool &isEnabled();

    [[nodiscard]] std::string getLabel() const;

    [[nodiscard]] Object3D *getParent() const;

    virtual void onDrawHostBuffer();

    [[nodiscard]] bool isEnableLights() const;

    void setEnableLights(bool enableLights);

    virtual cJSON *getJSON();

    void lookAt(Object3D *o);

    virtual void drawImGuiProperties();

    void attachScript(ScriptLUA *script);

    void runScripts();

    void reloadScriptsEnvironment();

    void reloadScriptsCode();

    virtual const char *getTypeObject();

    virtual const char *getTypeIcon();

    void removeScript(ScriptLUA *script);

    [[nodiscard]] const std::vector<ScriptLUA *> &getScripts() const;

    void runStartScripts();

    [[nodiscard]] bool isBelongToScene() const;

    void setBelongToScene(bool belongToScene);

    static void createFromJSON(cJSON *currentType);

    static void setPropertiesFromJSON(cJSON *object, Object3D *o);

    glm::mat4 getModelMatrix();

    void addMesh3DShader(FXEffectOpenGL *shader);

    void removeShader(int i);

    [[nodiscard]] const Timer &getTimer() const;

    [[nodiscard]] float getDistanceToCamera() const;

    [[nodiscard]] bool isMultiScene() const;

    void setMultiScene(bool multiScene);

    [[nodiscard]] const std::vector<Object3D *> &getAttached() const;

    void attachObject(Object3D *o);

    LUADataValue getLocalScriptVar(const char *varName);

    void makeSimpleRigidBody(float mass, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask) override;
    void integrate() override;
    void updateFromBullet();
    void resolveCollision(Collider *with) override;
    void runResolveCollisionScripts(Collider *with);

    const sol::environment &getLuaEnvironment() const;

    bool enabled;
};

#endif //SDL2_3D_ENGINE_OBJECT3D_H
