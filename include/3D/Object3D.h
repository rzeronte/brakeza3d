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
#include "../SceneObjectTypes.h"
#include "../Render/M3.h"
#include "../Misc/ScriptLUA.h"
#include "../../sol/sol.hpp"
#include "../GUI/GUI.h"
#include "../Render/Collider.h"
#include "../Render/Color.h"
#include "../Misc/Timer.h"


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
    Vertex3D drawOffset = Vertex3D::zero();

    Object3D *parent = nullptr;

    IconGUI::ObjectGUIFeatures featuresGUI;

    std::string name;

    std::vector<ScriptLUA*> scripts;
    std::vector<Object3D*> attachedObjects;
    sol::environment luaEnvironment;

    Timer timer;
    Color pickingColor;
    M3 rotation = M3::getMatrixIdentity();
    ObjectType type;
public:
    Object3D();
    virtual ~Object3D();

    bool& enabledPointer();
    void setParent(Object3D *object);
    void setName(const std::string& value);
    void setPosition(const Vertex3D &p);
    void AddToPosition(const Vertex3D &p);
    void setRotation(const M3 &r);
    void setScale(float value);
    void setRemoved(bool value);
    void setDecal(bool value);
    void setDrawOffset(const Vertex3D &v);
    void setAlpha(float alpha);
    void setAlphaEnabled(bool value);
    void setEnableLights(bool value);
    void LookAt(Object3D *o);
    void AttachScript(ScriptLUA *script);
    void RunScripts();
    void ReloadScriptsEnvironment();
    void ReloadScriptsCode() const;
    void RemoveScript(const ScriptLUA *script);
    void RunStartScripts();
    void setBelongToScene(bool belongToScene);
    void setMultiScene(bool multiScene);
    void AttachObject(Object3D *o);
    void MakeSimpleRigidBody(float mass, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask) override;
    void MakeKineticBody(float x, float y, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask) override;
    void Integrate() override;
    void UpdateFromBullet();
    void ResolveCollision(CollisionInfo with) override;
    void RunResolveCollisionScripts(CollisionInfo with);
    virtual ObjectType getTypeObject() const;
    virtual GUIType::Sheet getIcon();
    virtual void setEnabled(bool value);
    virtual void onUpdate();
    virtual void postUpdate();
    virtual void DrawPropertiesGUI();
    void SetupGhostCollider(CollisionShape mode) override;
    M3& rotationPointer();
    Vertex3D& positionPointer();
    Vertex3D &getPosition();
    [[nodiscard]] int getId() const;
    [[nodiscard]] float &getAlpha();
    [[nodiscard]] float getScale() const;
    [[nodiscard]] float getDistanceToCamera() const;
    [[nodiscard]] bool isGUISelected() const;
    [[nodiscard]] bool &isAlphaEnabled();
    [[nodiscard]] bool isDecal() const;
    [[nodiscard]] bool isRemoved() const;
    [[nodiscard]] bool &isEnabled();
    [[nodiscard]] bool isEnableLights() const;
    [[nodiscard]] bool isBelongToScene() const;
    [[nodiscard]] bool isMultiScene() const;
    [[nodiscard]] const std::vector<Object3D *> &getAttached() const;
    [[nodiscard]] const sol::environment &getLuaEnvironment() const;
    [[nodiscard]] const Timer &getTimer() const;
    [[nodiscard]] const std::vector<ScriptLUA *> &getScripts() const;
    [[nodiscard]] glm::mat4 getModelMatrix() const;
    [[nodiscard]] M3 getM3ModelMatrix() const;
    [[nodiscard]] M3 getRotation() const;
    [[nodiscard]] Vertex3D &getDrawOffset();
    [[nodiscard]] Vertex3D up() const;
    [[nodiscard]] Vertex3D down() const;
    [[nodiscard]] Vertex3D forward() const;
    [[nodiscard]] Vertex3D backwards() const;
    [[nodiscard]] Vertex3D right() const;
    [[nodiscard]] Vertex3D left() const;
    [[nodiscard]] Color getPickingColor() const;
    [[nodiscard]] std::string getName();
    [[nodiscard]] Object3D *getParent() const;
    sol::object getLocalScriptVar(const char *varName);

    friend class Object3DSerializer;
    friend class Object3DGUI;
};

#endif //SDL2_3D_ENGINE_OBJECT3D_H
