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
#include "../GUI/GUITypes.h"
#include "../Render/Collider.h"
#include "../Render/Color.h"

class Object3DGUI;
class Scene;

struct RenderSettings {
    GLenum mode_src = GL_SRC_ALPHA;
    GLenum mode_dst = GL_ONE_MINUS_SRC_ALPHA;
    bool blend = true;
    bool culling = true;
    bool depthTest = true;
    bool writeDepth = true;
    bool shadowMap = true;
    bool frustumCulling = true;
};

class Object3D: public Collider
{
protected:
    unsigned int id = 0;
    float scale = 1.f;
    float alpha = 1.f;
    float distanceToCamera = 0.f;
    float boundingRadius = 2.0f;
    bool enabled = true;
    bool removed = false;
    bool belongToScene = false;
    bool visibleInFrustum = true;
    bool multiScene = false;
    bool alphaEnabled = false;
    Scene *scene = nullptr;
    bool enableLights = true;
    bool selectable = true;
    bool highlighted = false;
    Color highlightColor;

    Vertex3D position = Vertex3D(1, 1, 1);
    Vertex3D drawOffset = Vertex3D::zero();

    Object3D *parent = nullptr;

    IconGUI::ObjectGUIFeatures featuresGUI;

    std::string name;

    std::vector<ScriptLUA*> scripts;
    std::vector<ScriptLUA*> pendingScripts;
    std::vector<Object3D*> attachedObjects;
    sol::environment luaEnvironment;

    Color pickingColor;
    M3 rotation = M3::getMatrixIdentity();
    ObjectType type;
    RenderSettings renderSettings;
public:
    bool showAvatar = true;

    Object3D();
    virtual ~Object3D();

    void AddToPosition(const Vertex3D &p);
    void LookAt(Object3D *o);
    void LookAt(const Vertex3D &target);
    void AttachScript(ScriptLUA *script);
    void AttachAndInitScript(ScriptLUA *script);
    void RunScripts();
    void onUpdateScripts();
    void ReloadScriptsEnvironment();
    void ReloadScriptsCode() const;
    void RemoveScript(const ScriptLUA *script);
    void RunStartScripts();
    void setParent(Object3D *object);
    void setName(const std::string& value);
    void setPosition(const Vertex3D &p);
    void setRotation(const M3 &r);
    void setScale(float value);
    void setRemoved(bool value);
    void setDrawOffset(const Vertex3D &v);
    void setAlpha(float alpha);
    void setAlphaEnabled(bool value);
    void setEnableLights(bool value);
    void setBelongToScene(bool belongToScene);
    void setMultiScene(bool multiScene);
    void AttachObject(Object3D *o);
    void MakeSimpleRigidBody(float mass, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask) override;
    btRigidBody* BuildSimpleRigidBodyOnly(float mass);
    void MakeKineticBody(float x, float y, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask) override;
    void Integrate() override;
    void UpdateFromBullet();
    void ResolveCollision(CollisionInfo with) override;
    void RunResolveCollisionScripts(CollisionInfo with);
    void SetupGhostCollider(CollisionShape mode) override;


    virtual GUIType::Sheet getIcon()                                    { return IconObject::OBJECT_3D; }
    bool& enabledPointer()                                              { return enabled; }
    M3& rotationPointer()                                               { return rotation; }
    Vertex3D& positionPointer()                                         { return position; }
    Vertex3D &getPosition()                                             { return position; }
    [[nodiscard]] const Vertex3D &getPosition() const                   { return position; }
    sol::object getLocalScriptVar(const char *varName)                  { return luaEnvironment[varName]; }
    void setLocalScriptVar(const char *varName, sol::object value)      { luaEnvironment[varName] = value; }
    [[nodiscard]] virtual ObjectType getTypeObject() const              { return ObjectType::Object3D; }
    [[nodiscard]] unsigned int getId() const                            { return id; }
    [[nodiscard]] float &getAlpha()                                     { return alpha; }
    [[nodiscard]] float getScale() const                                { return scale; }
    [[nodiscard]] float getDistanceToCamera() const                     { return distanceToCamera; }
    [[nodiscard]] float getBoundingRadius() const                       { return boundingRadius; }
    void setBoundingRadius(float r)                                     { boundingRadius = r; }
    [[nodiscard]] bool &isAlphaEnabled()                                { return alphaEnabled; }
    [[nodiscard]] bool isRemoved() const                                { return removed;}
    [[nodiscard]] bool isVisibleInFrustum() const                       { return visibleInFrustum; }
    void setVisibleInFrustum(bool value)                                { visibleInFrustum = value; }
    [[nodiscard]] bool &isEnabled()                                     { return enabled; }
    [[nodiscard]] bool isSelectable() const                             { return selectable; }
    void setSelectable(bool value)                                      { selectable = value; }
    [[nodiscard]] bool isHighlighted() const                            { return highlighted; }
    [[nodiscard]] Color getHighlightColor() const                       { return highlightColor; }
    void setHighlight(float r, float g, float b, float a)               { highlighted = true; highlightColor = Color(r, g, b, a); }
    void clearHighlight()                                               { highlighted = false; }
    [[nodiscard]] bool isEnableLights() const                           { return enableLights; }
    [[nodiscard]] bool isBelongToScene() const                          { return belongToScene; }
    [[nodiscard]] bool isMultiScene() const                             { return multiScene; }
    [[nodiscard]] Scene *getScene() const                               { return scene; }
    void setScene(Scene *s)                                             { scene = s; }
    [[nodiscard]] const std::vector<Object3D *> &getAttached() const    { return attachedObjects; }
    [[nodiscard]] const sol::environment &getLuaEnvironment() const     { return luaEnvironment; }
    [[nodiscard]] const std::vector<ScriptLUA *> &getScripts() const    { return scripts; }
    [[nodiscard]] M3 getM3ModelMatrix() const                           { return M3::fromMat3GLM(getModelMatrix()); }
    [[nodiscard]] M3 getRotation() const                                { return rotation; }
    [[nodiscard]] Color getPickingColor() const                         { return pickingColor; }
    [[nodiscard]] const std::string& getName() const                    { return name; }
    [[nodiscard]] Object3D *getParent() const                           { return parent; }
    [[nodiscard]] Vertex3D &getDrawOffset()                             { return this->drawOffset; }
    [[nodiscard]] RenderSettings &getRenderSettings()                         { return renderSettings; }
    [[nodiscard]] const RenderSettings &getRenderSettings() const             { return renderSettings; }
    [[nodiscard]] bool isTransparent() const                            { return alpha < 1.0f; }
    [[nodiscard]] glm::mat4 getModelMatrix() const;
    [[nodiscard]] bool isGUISelected() const;
    [[nodiscard]] Vertex3D up() const;
    [[nodiscard]] Vertex3D down() const;
    [[nodiscard]] Vertex3D forward() const;
    [[nodiscard]] Vertex3D backwards() const;
    [[nodiscard]] Vertex3D right() const;
    [[nodiscard]] Vertex3D left() const;

    virtual void setEnabled(bool value);
    virtual void onUpdate();
    virtual void postUpdate();
    virtual void DrawPropertiesGUI();

    friend class Object3DSerializer;
    friend class Object3DGUI;

};

#endif //SDL2_3D_ENGINE_OBJECT3D_H
