
#include <iostream>
#include "../../include/Objects/Object3D.h"
#include "../../include/Misc/Tools.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"

Object3D::Object3D() :
    position(Vertex3D(1, 1, 1)),
    motion(nullptr),
    parent(nullptr),
    enabled(true),
    removed(false),
    decal(false),
    followCamera(false),
    stencilBufferEnabled(false),
    rotX(0),
    rotY(0),
    rotZ(0),
    alphaEnabled(false),
    alpha(0),
    enableLights(false),
    scale(1),
    rotationFrameEnabled(false),
    belongToScene(false),
    luaEnvironment(sol::environment(
        EngineBuffers::get()->getLua(),
        sol::create, EngineBuffers::get()->getLua().globals())
    )
{
    luaEnvironment.set("this", this);
}

Vertex3D &Object3D::getPosition() {
    return position;
}

M3 Object3D::getRotation() {
    return rotation;
}

void Object3D::setPosition(Vertex3D p) {
    position = p;
}

void Object3D::setRotation(M3 r) {
    this->rotation = r;
}

std::string Object3D::getLabel() const {
    return label;
}

void Object3D::setLabel(const std::string& label) {
    Object3D::label = label;
}

bool &Object3D::isEnabled() {
    return enabled;
}

void Object3D::setEnabled(bool enabled) {
    Object3D::enabled = enabled;
}

Vertex3D Object3D::AxisUp() {
    Vertex3D v = getRotation() * EngineSetup::get()->up;
    return v.getNormalize();
}

Vertex3D Object3D::AxisDown() {
    Vertex3D v = getRotation() * EngineSetup::get()->down;
    return v.getNormalize();
}

Vertex3D Object3D::AxisForward() {
    Vertex3D v = getRotation() * EngineSetup::get()->forward;
    return v.getNormalize();
}

Vertex3D Object3D::AxisBackwards() {
    Vertex3D v = getRotation() * EngineSetup::get()->backward;

    return v.getNormalize();
}

Vertex3D Object3D::AxisRight() {
    Vertex3D v = getRotation() * EngineSetup::get()->right;

    return v.getNormalize();
}

Vertex3D Object3D::AxisLeft() {
    Vertex3D v = getRotation() * EngineSetup::get()->left;

    return v.getNormalize();
}

float Object3D::getScale() const {
    return scale;
}

void Object3D::setScale(float value) {
    Object3D::scale = value;
}

bool Object3D::isRemoved() const {
    return removed;
}

void Object3D::setRemoved(bool value) {
    Object3D::removed = value;
}

bool Object3D::isDecal() const {
    return decal;
}

void Object3D::setDecal(bool value) {
    Object3D::decal = value;
}

void Object3D::setDrawOffset(Vertex3D offset) {
    this->drawOffset = offset;
}

Vertex3D &Object3D::getDrawOffset() {
    return this->drawOffset;
}

Object3D *Object3D::getParent() const {
    return parent;
}

void Object3D::setParent(Object3D *object) {
    Object3D::parent = object;
}

bool &Object3D::isFollowCamera() {
    return this->followCamera;
}

void Object3D::setFollowCamera(bool value) {
    Object3D::followCamera = value;
}

void Object3D::onUpdate()
{
    if (isRemoved()) return;

    if (!isEnabled()) return;

    if (this->isFollowCamera()) {
        this->setPosition(ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition());
        this->setRotation(ComponentsManager::get()->getComponentCamera()->getCamera()->getRotation().getTranspose());
    }

    if (getBehavior() != nullptr) {
        motion->onUpdate(position);
    }

    if (ComponentsManager::get()->getComponentRender()->getStateScripts() == EngineSetup::LUA_PLAY) {
        runScripts();
    }
}

void Object3D::runScripts()
{
    for (auto script: scripts) {
        script->runEnvironment(luaEnvironment);
    }
}

void Object3D::postUpdate()
{
    if (!isEnabled()) return;

    if (isRotationFrameEnabled()) {
        setRotation(getRotation() * M3::getMatrixRotationForEulerAngles(rotationFrame.x, rotationFrame.y, rotationFrame.z));
    }
}

void Object3D::setRotation(float x, float y, float z) {
    this->rotX = x;
    this->rotY = y;
    this->rotZ = z;
    this->setRotation(M3::getMatrixRotationForEulerAngles(x, y, z));
}

void Object3D::addToPosition(Vertex3D v) {
    this->position = this->position + v;
}

bool Object3D::isRotationFrameEnabled() {
    return rotationFrameEnabled;
}

void Object3D::setRotationFrameEnabled(bool value) {
    this->rotationFrameEnabled = value;
}

void Object3D::setRotationFrame(Vertex3D r) {
    this->rotationFrame = r;
}

bool &Object3D::isStencilBufferEnabled() {
    return stencilBufferEnabled;
}

void Object3D::setStencilBufferEnabled(bool value) {
    Object3D::stencilBufferEnabled = value;
}

Object3DBehavior *Object3D::getBehavior() const {
    return motion;
}

void Object3D::setBehavior(Object3DBehavior *motion) {
    Object3D::motion = motion;
}

float &Object3D::getAlpha() {
    return alpha;
}

void Object3D::setAlpha(float alpha) {
    Object3D::alpha = alpha;
}

bool &Object3D::isAlphaEnabled() {
    return alphaEnabled;
}

void Object3D::setAlphaEnabled(bool alphaEnabled) {
    Object3D::alphaEnabled = alphaEnabled;
}

Object3D::~Object3D()
{
}

float &Object3D::getRotX() {
    return rotX;
}

float &Object3D::getRotY() {
    return rotY;
}

float &Object3D::getRotZ() {
    return rotZ;
}

void Object3D::onDrawHostBuffer()
{
    if (EngineSetup::get()->RENDER_OBJECTS_AXIS) {
        Drawable::drawObject3DAxis(
            this,
            ComponentsManager::get()->getComponentCamera()->getCamera(),
            true,
            true,
            true
        );
    }
}

bool Object3D::isEnableLights() const {
    return enableLights;
}

void Object3D::setEnableLights(bool enableLights) {
    Object3D::enableLights = enableLights;
}

cJSON *Object3D::getJSON()
{
    return nullptr;
}

const Vertex3D &Object3D::getRotationFrame() const {
    return rotationFrame;
}

void Object3D::drawOnUpdateSecondPass() {
}

void Object3D::lookAt(Object3D *o)
{
    Vertex3D direction = (o->getPosition() - position).getNormalize();

    // Calcular el eje de rotación
    Vertex3D rightVector = Vertex3D(0, 0, -1) % (direction).getNormalize();
    Vertex3D correctedUpVector = direction % (rightVector).getNormalize();

    // Crear una matriz de rotación
    M3 r;
    r.setX(rightVector.x, correctedUpVector.x, -direction.x);
    r.setY(rightVector.y, correctedUpVector.y, -direction.y);
    r.setZ(rightVector.z, correctedUpVector.z, -direction.z);

    setRotation(r);
}

void Object3D::drawImGuiProperties()
{
    if (ImGui::TreeNode("Position")) {
        const float range_min = -500000;
        const float range_max = 500000;

        ImGui::SliderScalar("X", ImGuiDataType_Float, &position.x, &range_min, &range_max, "%f");
        ImGui::SliderScalar("Y", ImGuiDataType_Float, &position.y, &range_min, &range_max, "%f");
        ImGui::SliderScalar("Z", ImGuiDataType_Float, &position.z, &range_min, &range_max, "%f");

        ImGui::TreePop();
    }

}

void Object3D::attachScript(ScriptLUA *script)
{
    scripts.push_back(script);
    reloadScriptsEnvironment();
}

void Object3D::reloadScriptsEnvironment()
{
    for (auto script : scripts) {
        Logging::Message("Reloading script environment variables for %s", getLabel().c_str());
        script->reloadEnvironment(luaEnvironment);
    }
}

void Object3D::reloadScriptsCode()
{
    for (auto script : scripts) {
        script->reloadScriptCode();
    }
}

void Object3D::removeScript(ScriptLUA *script)
{
    Logging::Message("Removing object script %s", script->scriptFilename.c_str());

    for (auto it = scripts.begin(); it != scripts.end(); ++it) {
        if ((*it)->scriptFilename == script->scriptFilename) {
            delete *it;
            scripts.erase(it);
            return;
        }
    }
}

const char *Object3D::getTypeObject() {
    return "Object3D";
}


const char *Object3D::getTypeIcon() {
    return "objectIcon";
}

const std::vector<ScriptLUA *> &Object3D::getScripts() const {
    return scripts;
}

void Object3D::runStartScripts()
{
    for (auto script : scripts) {
        script->runStart(luaEnvironment);
    }
}

bool Object3D::isBelongToScene() const {
    return belongToScene;
}

void Object3D::setBelongToScene(bool belongToScene) {
    Object3D::belongToScene = belongToScene;
}

void Object3D::insertGlobalIntoEnvironment(ScriptLUA *scriptToInsert)
{
    for (auto script: scripts) {
        script->insertGlobalsIntoEnvironment(scriptToInsert, luaEnvironment);
    }
}
