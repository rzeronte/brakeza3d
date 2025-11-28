#ifndef SDL2_3D_ENGINE_OBJECT3D_H
#define SDL2_3D_ENGINE_OBJECT3D_H

//#define SOL_ALL_SAFETIES_ON 1
#define SOL_SAFE_USERTYPE 1
#define SOL_SAFE_REFERENCES  0
#define SOL_SAFE_NUMERICS 1
#define SOL_SAFE_GETTER 0
#define SOL_SAFE_FUNCTION_CALLS 0

#include <string>
#include <vector>
#include <glm/detail/type_mat4x4.hpp>
#include "Vertex3D.h"
#include "Vector3D.h"
#include "../Render/M3.h"
#include "../Misc/ScriptLUA.h"
#include "../../sol/sol.hpp"
#include "../Render/Collider.h"
#include "../Render/Color.h"
#include "../Misc/Timer.h"

struct ObjectGUIFeatures {
    bool position = true;
    bool rotation = true;
    bool scale = true;
    bool alpha = true;
    bool shaders = true;
    bool attached = true;
    bool collider = true;
    bool misc = true;
};

class Object3DGUI;

class Object3D: public Collider
{
protected:
    int id = 0;
    float scale = 1.f;
    float alpha = 1.f;
    float distanceToCamera = 0.f;
    bool enabled = true;
    bool removed = false;
    bool decal = false;
    bool belongToScene = false;
    bool multiScene = false;
    bool alphaEnabled = false;
    bool enableLights = false;

    Vertex3D position = Vertex3D(1, 1, 1);
    Vertex3D drawOffset;

    Object3D *parent = nullptr;

    ObjectGUIFeatures featuresGUI;

    std::string label;

    std::vector<ScriptLUA*> scripts;
    std::vector<Object3D*> attachedObjects;
    sol::environment luaEnvironment;

    Timer timer;
    Color pickingColor;
    M3 rotation = M3::getMatrixIdentity();
public:
    Object3D();
    virtual ~Object3D();

    int getId() const;
    bool& enabledPointer();
    void setParent(Object3D *object);
    void setLabel(const std::string& label);
    void setPosition(const Vertex3D &p);
    void addToPosition(const Vertex3D &p);
    void setRotation(const M3 &r);
    void setScale(float value);
    void setRemoved(bool value);
    void setDecal(bool value);
    void setDrawOffset(const Vertex3D &v);
    void setAlpha(float alpha);
    void setAlphaEnabled(bool value);
    void setEnableLights(bool value);
    void lookAt(Object3D *o);
    void attachScript(ScriptLUA *script);
    void runScripts();
    void reloadScriptsEnvironment();
    void reloadScriptsCode() const;
    void removeScript(const ScriptLUA *script);
    void runStartScripts();
    void setBelongToScene(bool belongToScene);
    void setMultiScene(bool multiScene);
    void attachObject(Object3D *o);
    void makeSimpleRigidBody(float mass, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask) override;
    void makeKineticBody(float x, float y, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask) override;
    void integrate() override;
    void updateFromBullet();
    void resolveCollision(CollisionInfo with) override;
    void runResolveCollisionScripts(CollisionInfo with);
    virtual void checkClickObject(Vector3D ray, Object3D*& foundObject, float &lastDepthFound);
    virtual const char *getTypeObject();
    virtual const char *getTypeIcon();
    virtual void setEnabled(bool value);
    virtual void onUpdate();
    virtual void postUpdate();
    virtual void DrawPropertiesGUI();
    void setupGhostCollider(CollisionShape mode) override;
    glm::mat4 getModelMatrix() const;
    M3& rotationPointer();
    M3 getM3ModelMatrix() const;
    M3 getRotation() const;
    Vertex3D& positionPointer();
    Vertex3D &getPosition();
    Vertex3D AxisUp() const;
    Vertex3D AxisDown() const;
    Vertex3D AxisForward() const;
    Vertex3D AxisBackwards() const;
    Vertex3D AxisRight() const;
    Vertex3D AxisLeft() const;
    sol::object getLocalScriptVar(const char *varName);
    [[nodiscard]] const std::vector<ScriptLUA *> &getScripts() const;
    [[nodiscard]] bool &isAlphaEnabled();
    [[nodiscard]] Vertex3D &getDrawOffset();
    [[nodiscard]] float &getAlpha();
    [[nodiscard]] bool isDecal() const;
    [[nodiscard]] float getScale() const;
    [[nodiscard]] bool isRemoved() const;
    [[nodiscard]] bool &isEnabled();
    [[nodiscard]] std::string getLabel() const;
    [[nodiscard]] Object3D *getParent() const;
    [[nodiscard]] bool isEnableLights() const;
    [[nodiscard]] bool isBelongToScene() const;
    [[nodiscard]] const std::vector<Object3D *> &getAttached() const;
    [[nodiscard]] const sol::environment &getLuaEnvironment() const;
    [[nodiscard]] const Timer &getTimer() const;
    [[nodiscard]] float getDistanceToCamera() const;
    [[nodiscard]] bool isMultiScene() const;
    [[nodiscard]] Color getPickingColor() const;

    friend class Object3DSerializer;
    friend class Object3DGUI;
};

#endif //SDL2_3D_ENGINE_OBJECT3D_H
