//
// Created by eduardo on 5/10/23.
//

#ifndef BRAKEZA3D_LUAINTEGRATION_H
#define BRAKEZA3D_LUAINTEGRATION_H

#include "ObjectFactory.h"
#include "../../sol/sol.hpp"
#include "../3D/Vertex3D.h"
#include "../Misc/ScriptLUA.h"
#include "../Components/ComponentCamera.h"
#include "../Components/Components.h"
#include "../Brakeza.h"
#include "../Render/GlyphAtlas.h"
#include "../2D/Image2D.h"
#include "../2D/Image2DAnimation.h"
#include "../3D/Mesh3DAnimation.h"
#include "../3D/Image3D.h"
#include "../3D/LightPoint.h"
#include "../3D/LightSpot.h"
#include "../3D/Image3DAnimation.h"
#include "../3D/Image3DAnimation360.h"
#include "../3D/ParticleEmitter.h"
#include "../Misc/VideoPlayer.h"
#include "../Threads/ThreadJobPathfinding.h"
#include "../Threads/ThreadJobReadObjectScript.h"
#include "../Loaders/FBXLightLoader.h"
#include "../Loaders/Scene.h"
#include "../Loaders/SceneLoader.h"
#include "../3D/Projectile.h"

// Undefine Windows API macro that conflicts with ComponentSound::PlaySound
#ifdef PlaySound
#undef PlaySound
#endif

inline void LUAIntegration(sol::state &lua)
{
    lua.new_usertype<Vertex3D>(
        "Vertex3D",
        sol::constructors<Vertex3D(), Vertex3D(float, float, float)>(),
        "x", &Vertex3D::x,
        "y", &Vertex3D::y,
        "z", &Vertex3D::z,
        "__add", sol::overload(&Vertex3D::operator+),
        "__sub", sol::overload(&Vertex3D::operator-),
        "getScaled", sol::overload(
            static_cast<Vertex3D (Vertex3D::*)(float) const>(&Vertex3D::getScaled),
            static_cast<Vertex3D (Vertex3D::*)(float, float, float) const>(&Vertex3D::getScaled)
        ),
        "getNormalize", &Vertex3D::getNormalize,
        "getModule", &Vertex3D::getModule
    );

    lua.new_usertype<M3>(
        "M3",
        sol::constructors<M3()>(),
        "__mul", sol::overload(
            sol::resolve<Vertex3D(Vertex3D)>(&M3::operator*),
            sol::resolve<M3(const M3&)>(&M3::operator*)
        ),
        "getMatrixRotationForEulerAngles", &M3::getMatrixRotationForEulerAngles,
        "RX", &M3::RX,
        "RY", &M3::RY,
        "RZ", &M3::RZ,
        "X", &M3::X,
        "Y", &M3::Y,
        "Z", &M3::Z,
        "getTranspose",  &M3::getTranspose,
        "setMatrixRotationForEulerAngles", &M3::setMatrixRotationForEulerAngles,
        "getMatrixIdentity", &M3::getMatrixIdentity
    );

    lua.new_usertype<Color>(
        "Color",
        sol::constructors<Color(),
            Color(float, float, float, float)>(),
        "setRed", &Color::setRed,
        "setGreen", &Color::setGreen,
        "setBlue", &Color::setBlue
    );

    lua.new_usertype<Object3D>(
        "Object3D",
        "addToPosition", &Object3D::AddToPosition,
        "getPosition", static_cast<Vertex3D&(Object3D::*)()>(&Object3D::getPosition),
        "setPosition", &Object3D::setPosition,
        "setRotation", &Object3D::setRotation,
        "getTypeObject", &Object3D::getTypeObject,
        "getName", &Object3D::getName,
        "getRotation", &Object3D::getRotation,
        "setBelongToScene", &Object3D::setBelongToScene,
        "setEnabled", &Object3D::setEnabled,
        "setRemoved", &Object3D::setRemoved,
        "isRemoved", &Object3D::isRemoved,
        "setName", &Object3D::setName,
        "getScale", &Object3D::getScale,
        "setScale", &Object3D::setScale,
        "getModelMatrix", &Object3D::getModelMatrix,
        "getM3ModelMatrix", &Object3D::getM3ModelMatrix,
        "AxisForward", &Object3D::forward,
        "AxisUp", &Object3D::up,
        "AxisRight", &Object3D::right,
        "RemoveCollisionObject", &Object3D::RemoveCollisionObject,
        "SleepCollider", &Object3D::SleepCollider,
        "setCollisionsEnabled", &Object3D::setCollisionsEnabled,
        "setCollisionGroupMask", [](Object3D* o, int group, int mask) { o->setCollisionGroupMask(group, mask); },
        "DisableSimulationCollider", &Object3D::DisableSimulationCollider,
        "EnableSimulationCollider", &Object3D::EnableSimulationCollider,
        "DisableDeactivationCollider", &Object3D::DisableDeactivationCollider,
        "UpdateShapeCollider", &Object3D::UpdateShapeCollider,
        "setScalingCollider", &Object3D::setScalingCollider,
        "setSimpleShapeSize", [](Object3D* o, const Vertex3D& size) { o->setSimpleShapeSize(size); },
        "setCapsuleColliderSize", &Object3D::setCapsuleColliderSize,
        "moveCollider", &Object3D::moveCollider,
        "isCollisionsEnabled", &Object3D::isCollisionsEnabled,
        "setDrawOffset", &Object3D::setDrawOffset,
        "setupGhostCollider", &Object3D::SetupGhostCollider,
        "SetupRigidBodyCollider", &Object3D::SetupRigidBodyCollider,
        "setColliderStatic", &Object3D::setColliderStatic,
        "setDebugDraw", &Object3D::setDebugDraw,
        "isDebugDraw", &Object3D::isDebugDraw,
        "resetColliderRotation", &Object3D::resetColliderRotation,
        "ApplyCentralForce", &Object3D::ApplyCentralForce,
        "ApplyCentralImpulse", &Object3D::ApplyCentralImpulse,
        "ApplyImpulse", &Object3D::ApplyImpulse,
        "setLinearVelocity", &Object3D::setLinearVelocity,
        "setAngularVelocity", &Object3D::setAngularVelocity,
        "setFriction", &Object3D::setFriction,
        "setRestitution", &Object3D::setRestitution,
        "setAngularDamping", &Object3D::setAngularDamping,
        "setLinearDamping", &Object3D::setLinearDamping,
        "setAngularFactor", &Object3D::setAngularFactor,
        "setLinearFactor",  &Object3D::setLinearFactor,
        "setMass", &Object3D::setMass,
        "setGravityCollider", &Object3D::setGravityCollider,
        "setWalkingDirection", &Object3D::setWalkingDirection,
        "Jump", &Object3D::Jump,
        "onGround", &Object3D::onGround,
        "getLocalScriptVar",  &Object3D::getLocalScriptVar,
        "setLocalScriptVar",  &Object3D::setLocalScriptVar,
        "isSelectable",       &Object3D::isSelectable,
        "setSelectable",      &Object3D::setSelectable,
        "setHighlight",       &Object3D::setHighlight,
        "clearHighlight",     &Object3D::clearHighlight,
        "AttachScript", &Object3D::AttachScript,
        "AttachScriptAsync", [](Object3D* obj, const std::string& jsonPath) {
            Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(
                std::make_shared<ThreadJobReadObjectScript>(obj, jsonPath)
            );
        },
        "LookAt", sol::overload(
            static_cast<void(Object3D::*)(Object3D*)>(&Object3D::LookAt),
            static_cast<void(Object3D::*)(const Vertex3D&)>(&Object3D::LookAt)
        ),
        "ReloadScriptsEnvironment", &Object3D::ReloadScriptsEnvironment,
        "getLinearVelocity", &Object3D::getLinearVelocity,
        "setParent", &Object3D::setParent,
        "getParent", &Object3D::getParent,
        "AttachObject", &Object3D::AttachObject,
        "getAttached", &Object3D::getAttached,
        "getAlpha", &Object3D::getAlpha,
        "setAlpha", &Object3D::setAlpha,
        "isAlphaEnabled", &Object3D::isAlphaEnabled,
        "setAlphaEnabled", &Object3D::setAlphaEnabled,
        "isEnableLights", &Object3D::isEnableLights,
        "setEnableLights", &Object3D::setEnableLights,
        "isEnabled", &Object3D::isEnabled,
        "setCcdMotionThreshold", &Object3D::setCcdMotionThreshold,
        "setCcdSweptSphereRadius", &Object3D::setCcdSweptSphereRadius,
        "setShapeMargin", &Object3D::setShapeMargin,
        "setFrustumCulling", [](Object3D* o, bool v) { o->getRenderSettings().frustumCulling = v; },
        "setShadowMap",      [](Object3D* o, bool v) { o->getRenderSettings().shadowMap = v; },
        "isVisibleInFrustum", &Object3D::isVisibleInFrustum,
        "setScene", [](Object3D* o, const std::string& sceneName) {
            auto* scene = Components::get()->Scripting()->getSceneByName(sceneName);
            if (scene) {
                o->setScene(scene);
                scene->addObject(o);
            }
        }
    );

    lua.new_usertype<Component>("Component",
        "setEnabled", &Component::setEnabled,
        "isEnabled", &Component::isEnabled
    );

    lua.new_usertype<ComponentCamera>("ComponentCamera",
        sol::base_classes, sol::bases<Component>(),
        "getCamera", &ComponentCamera::getCamera,
        "isEnabled", &ComponentCamera::isEnabled,
        "setEnabled", &ComponentCamera::setEnabled,
        "getGLMMat4ViewMatrix", &ComponentCamera::getGLMMat4ViewMatrix,
        "getGLMMat4ProjectionMatrix", &ComponentCamera::getGLMMat4ProjectionMatrix,
        "worldToScreen", &ComponentCamera::worldToScreen
    );

    lua.new_usertype<ComponentSound>("ComponentSound",
    sol::base_classes, sol::bases<Component>(),
        "AddSound", &ComponentSound::AddSound,
        "AddMusic", &ComponentSound::AddMusic,
        "setMusicVolume", [](ComponentSound*, int v) { ComponentSound::setMusicVolume(v); },
        "setSoundsVolume", [](ComponentSound*, int v) { ComponentSound::setSoundsVolume(v); },
        "PlaySound", &ComponentSound::PlaySound,
        "PlayMusic", &ComponentSound::PlayMusic,
        "StopMusic", [](ComponentSound*) { ComponentSound::StopMusic(); },
        "StopChannel", [](ComponentSound*, int channel) { ComponentSound::StopChannel(channel); },
        "getSoundDuration", &ComponentSound::getSoundDuration,
        "LoadSoundsFromFile", &ComponentSound::LoadSoundsFromFile
    );

    lua.new_usertype<ComponentCollisions>("ComponentCollisions",
    sol::base_classes, sol::bases<Component>(),
        "isRayCollisionWith", &ComponentCollisions::isRayCollisionWith,
        "setEnableDebugMode", &ComponentCollisions::setEnableDebugMode,
        "setEnabled", &ComponentCollisions::setEnabled
    );

    lua.new_usertype<ComponentWindow>("ComponentWindow",
    sol::base_classes, sol::bases<Component>(),
        "setWindowTitle", &ComponentWindow::setWindowTitle,
        "ToggleFullScreen", &ComponentWindow::ToggleFullScreen,
        "getWidth", &ComponentWindow::getWidth,
        "getHeight", &ComponentWindow::getHeight,
        "getHeightRender", &ComponentWindow::getHeightRender,
        "getWidthRender", &ComponentWindow::getWidthRender,
        "isWindowMaximized", &ComponentWindow::isWindowMaximized,
        "LoadCursorImage", &ComponentWindow::LoadCursorImage,
        "setImGuiMouse", &ComponentWindow::setImGuiMouse,
        "setClearColor", &ComponentWindow::setClearColor
    );

    lua.new_usertype<ComponentRender>("ComponentRender",
    sol::base_classes, sol::bases<Component>(),
        "getSceneLoader", &ComponentRender::getSceneLoader,
        "DrawLine",    &ComponentRender::DrawLine,
        "DrawLine2D",     &ComponentRender::DrawLine2D,
        "DrawFilledRect", &ComponentRender::DrawFilledRect,
        "DrawCircle2D",   &ComponentRender::DrawCircle2D,
        "DrawCircle2DToFB", &ComponentRender::DrawCircle2DToFB,
        "DrawImage2D",    &ComponentRender::DrawImage2D,
        "DrawImage2DToFB", &ComponentRender::DrawImage2DToFB,
        "DrawImage2DFromImage", &ComponentRender::DrawImage2DFromImage,
        "setGlobalIlluminationDirection", &ComponentRender::setGlobalIlluminationDirection,
        "setGlobalIlluminationAmbient", &ComponentRender::setGlobalIlluminationAmbient,
        "setGlobalIlluminationDiffuse", &ComponentRender::setGlobalIlluminationDiffuse,
        "setGlobalIlluminationSpecular", &ComponentRender::setGlobalIlluminationSpecular,
        "getGlobalIlluminationDirection", &ComponentRender::getGlobalIlluminationDirection,
        "getGlobalIlluminationAmbient", &ComponentRender::getGlobalIlluminationAmbient,
        "getGlobalIlluminationDiffuse", &ComponentRender::getGlobalIlluminationDiffuse,
        "getGlobalIlluminationSpecular", &ComponentRender::getGlobalIlluminationSpecular,
        "getSceneShaderByLabel", [](ComponentRender& r, const std::string& label) -> ShaderBaseCustomOGLCode* {
            return dynamic_cast<ShaderBaseCustomOGLCode*>(r.getSceneShaderByLabel(label));
        },
        "clearSceneShaders", &ComponentRender::clearSceneShaders,
        "getFps", &ComponentRender::getFps,
        "MakeScreenShot", &ComponentRender::MakeScreenShot,
        // Single-selection (returns nil when 0 or >1 selected)
        "getSelectedObject", &ComponentRender::getSelectedObject,
        // Multi-selection API
        "getSelectedObjects", [](ComponentRender* cr) -> std::vector<Object3D*> {
            return cr->getSelectedObjects();
        },
        "addToSelection", &ComponentRender::addToSelection,
        "removeFromSelection", [](ComponentRender* cr, Object3D* o) {
            cr->removeFromSelection(o);
        },
        "clearSelection", &ComponentRender::clearSelection,
        "setSelectedObject", &ComponentRender::setSelectedObject,
        "hasMultipleSelected", &ComponentRender::hasMultipleSelected,
        "isObjectInSelection", [](ComponentRender* cr, Object3D* o) -> bool {
            return cr->isObjectInSelection(o);
        },
        "getLastRightClickedObject", &ComponentRender::getLastRightClickedObject,
        "clearRightClickedObject", &ComponentRender::clearRightClickedObject,
        "getLastRightClickedSubmeshName", &ComponentRender::getLastRightClickedSubmeshName,
        "getLastLeftClickedObject", &ComponentRender::getLastLeftClickedObject,
        "clearLeftClickedObject", &ComponentRender::clearLeftClickedObject,
        "getLastLeftClickedSubmeshName", &ComponentRender::getLastLeftClickedSubmeshName,
        "drawGroundCircle",  sol::overload(&ComponentRender::drawGroundCircle,  &ComponentRender::drawGroundCircleToFB),
        "drawGroundBlob",    sol::overload(&ComponentRender::drawGroundBlob,    &ComponentRender::drawGroundBlobToFB),
        "drawGroundDecal",   sol::overload(&ComponentRender::drawGroundDecal,   &ComponentRender::drawGroundDecalToFB),
        "drawOutlineSubmesh",  &ComponentRender::drawOutlineSubmesh,
        "getSubmeshCenter",    &ComponentRender::getSubmeshCenter,
        "DrawCircle3D", &ComponentRender::DrawCircle3D,
        "drawAxisQuad",    &ComponentRender::drawAxisQuad,
        "getTextWriter",   &ComponentRender::getTextWriter
    );

    lua.new_usertype<ComponentScripting>("ComponentScripting",
    sol::base_classes, sol::bases<Component>(),
        "PlayLUAScripts", &ComponentScripting::PlayLUAScripts,
        "StopLUAScripts", &ComponentScripting::StopLUAScripts,
        "ReloadLUAScripts", &ComponentScripting::ReloadLUAScripts,
        "ReloadScriptEnvironment", &ComponentScripting::ReloadScriptEnvironment,
        "AddSceneLUAScript", &ComponentScripting::AddSceneLUAScript,
        "AddProjectLUAScript", &ComponentScripting::AddProjectLUAScript,
        "getGlobalScriptVar",  &ComponentScripting::getGlobalScriptVar,
        "setGlobalScriptVar",  &ComponentScripting::setGlobalScriptVar,
        "RunProjectScriptsOnStart", &ComponentScripting::RunProjectScriptsOnStart,
        "loadJSON",  &ComponentScripting::loadJSON,
        "saveJSON",  &ComponentScripting::saveJSON
    );

    lua.new_usertype<ComponentInput>("ComponentInput",
    sol::base_classes, sol::bases<Component>(),
        "setKeyboardEnabled", &ComponentInput::setKeyboardEnabled,
        "setMouseEnabled", &ComponentInput::setMouseEnabled,
        "setPadEnabled", &ComponentInput::setPadEnabled,
        "isKeyboardEnabled", &ComponentInput::isKeyboardEnabled,
        "isMouseEnabled", &ComponentInput::isMouseEnabled,
        "isPadEnabled", &ComponentInput::isPadEnabled,
        "isKeyEventDown", &ComponentInput::isKeyEventDown,
        "isKeyEventUp", &ComponentInput::isKeyEventUp,
        "isCharPressed", &ComponentInput::isCharPressed,
        "isCharFirstEventDown", &ComponentInput::isCharFirstEventDown,
        "isMouseMotion", &ComponentInput::isMouseMotion,
        "isClickLeft", &ComponentInput::isClickLeft,
        "isClickRight", &ComponentInput::isClickRight,
        "isClickRightUp", &ComponentInput::isClickRightUp,
        "isMouseButtonDown", &ComponentInput::isMouseButtonDown,
        "isMouseButtonUp", &ComponentInput::isMouseButtonUp,
        "consumeLeftClick", &ComponentInput::consumeLeftClick,
        "isEnabled", &ComponentInput::isEnabled,
        "isAnyControllerButtonPressed", &ComponentInput::isAnyControllerButtonPressed,
        "getRelativeRendererMouseX", &ComponentInput::getRelativeRendererMouseX,
        "getRelativeRendererMouseY", &ComponentInput::getRelativeRendererMouseY,
        "getMouseMotionXRel", &ComponentInput::getMouseMotionXRel,
        "getMouseMotionYRel", &ComponentInput::getMouseMotionYRel,
        "isLeftMouseButtonPressed", &ComponentInput::isLeftMouseButtonPressed,
        "isRightMouseButtonPressed", &ComponentInput::isRightMouseButtonPressed,
        "isMiddleMouseButtonPressed", &ComponentInput::isMiddleMouseButtonPressed,
        "getMouseWheelY", &ComponentInput::getMouseWheelY,
        "getRawMouseX", &ComponentInput::getRawMouseX,
        "getRawMouseY", &ComponentInput::getRawMouseY,
        "isGameControllerAvailable", &ComponentInput::isGameControllerAvailable,
        "getControllerButtonA",  &ComponentInput::getControllerButtonA,
        "getControllerButtonB",  &ComponentInput::getControllerButtonB,
        "getControllerButtonX",  &ComponentInput::getControllerButtonX,
        "getControllerButtonY",  &ComponentInput::getControllerButtonY,
        "getControllerAxisTriggerLeft", &ComponentInput::getControllerAxisTriggerLeft,
        "getControllerAxisTriggerRight", &ComponentInput::getControllerAxisTriggerRight,

        "getControllerAxisLeftX", &ComponentInput::getControllerAxisLeftX,
        "getControllerAxisLeftY", &ComponentInput::getControllerAxisLeftY,
        "getControllerAxisRightX", &ComponentInput::getControllerAxisRightX,
        "getControllerAxisRightY", &ComponentInput::getControllerAxisRightY,
        "getControllerPadUp", &ComponentInput::getControllerPadUp,
        "getControllerPadDown", &ComponentInput::getControllerPadDown,
        "getControllerPadLeft", &ComponentInput::getControllerPadLeft,
        "getControllerPadRight", &ComponentInput::getControllerPadRight,

         "getControllerShoulderLeft", &ComponentInput::getControllerShoulderLeft,
         "getControllerShoulderRight", &ComponentInput::getControllerShoulderRight,

         "getControllerButtonBack", &ComponentInput::getControllerButtonBack,
         "getControllerButtonGuide", &ComponentInput::getControllerButtonGuide,
         "getControllerButtonStart", &ComponentInput::getControllerButtonStart
    );

    lua.new_usertype<Components>("Components",
    "Window", &Components::Window,
        "Render", &Components::Render,
        "Camera", &Components::Camera,
        "Collisions", &Components::Collisions,
        "Input", &Components::Input,
        "Sound", &Components::Sound,
        "Scripting", &Components::Scripting
    );

    lua.new_usertype<Brakeza>("Brakeza3D",
        "getDeltaTime", &Brakeza::getDeltaTime,
        "getDeltaTimeMicro",  &Brakeza::getDeltaTimeMicro,
        "getExecutionTime", &Brakeza::getExecutionTime,
        "uniqueObjectLabel", &Brakeza::UniqueObjectLabel,
        "Shutdown", &Brakeza::Shutdown,
        "AddObject3D",  &Brakeza::AddObject3D,
        "getObjectByName",    &Brakeza::getObjectByName,
        "getObjectAtScreen",  &Brakeza::getObjectAtScreen,
        "removeAllObjects",      &Brakeza::removeAllObjects,
        "getPendingJobsCount",   &Brakeza::getPendingJobsCount,
        "getMesh3DAnimationByName", [](Brakeza* b, const std::string& name) -> Mesh3DAnimation* {
            return dynamic_cast<Mesh3DAnimation*>(b->getObjectByName(name));
        },
        "getProjectileByName", [](Brakeza* b, const std::string& name) -> Projectile* {
            return dynamic_cast<Projectile*>(b->getObjectByName(name));
        },
        "loadLightsFromFBX", [](Brakeza* b, const std::string& path,
                                   float posX, float posY, float posZ,
                                   float rotX, float rotY, float rotZ,
                                   float scale,
                                   sol::optional<bool> enabledByDefault,
                                   sol::this_state s) -> sol::table {
            bool enabled = enabledByDefault.value_or(true);
            auto lights = FBXLightLoader::LoadLightsFromFile(path, posX, posY, posZ, rotX, rotY, rotZ, scale, enabled);
            sol::state_view lua(s);
            sol::table result = lua.create_table();
            int idx = 1;
            for (auto* light : lights) {
                b->AddObject3D(light, light->getName());
                if (light->getTypeObject() == ObjectType::LightSpot)
                    result[idx++] = static_cast<LightSpot*>(light);
                else
                    result[idx++] = static_cast<LightPoint*>(light);
            }
            return result;
        }
    );

    lua.new_usertype<Camera3D>("Camera3D",
       sol::base_classes, sol::bases<Object3D>(),
        "getM3ViewMatrix", &Camera3D::getM3ViewMatrix,
       "setFOV", &Camera3D::setFOV,
       "getM3ProjectionMatrix", &Camera3D::getM3ProjectionMatrix,
       "setRotationFromEulerAngles", &Camera3D::setRotationFromEulerAngles,
       "getNearPlane", [](Camera3D*) { return Config::get()->FRUSTUM_NEARPLANE_DISTANCE; },
       "setNearPlane", [](Camera3D*, float v) { Config::get()->FRUSTUM_NEARPLANE_DISTANCE = v; },
       "getFarPlane",  [](Camera3D*) { return Config::get()->FRUSTUM_FARPLANE_DISTANCE; },
       "setFarPlane",  [](Camera3D*, float v) { Config::get()->FRUSTUM_FARPLANE_DISTANCE = v; }
    );

    lua.new_usertype<AABB3D>("AABB3D",
    sol::constructors<AABB3D()>(),
        "min", &AABB3D::min,
        "max", &AABB3D::max,
        "vertices", &AABB3D::vertices,
        "size", &AABB3D::size,
        "isColliding", &AABB3D::isColliding,
        "setScale", &AABB3D::setScale,
        "updateVertices", &AABB3D::updateVertices,
        "getPlanes", &AABB3D::getPlanes,
        "getCenter", &AABB3D::getCenter,
        "isPointInside", &AABB3D::isPointInside
    );

    lua.new_usertype<OctreeNode>("OctreeNode",
    "leaf", &OctreeNode::leaf,
        "bounds", &OctreeNode::bounds,
        "children", &OctreeNode::children, // Exponer el vector directamente
        "isLeaf", &OctreeNode::isLeaf
    );

    lua.new_usertype<Octree>("Octree",
    sol::constructors<Octree(AABB3D, int)>(),
        "BuildOctree", &Octree::BuildOctree,
        "FindNode", &Octree::FindNode,
        "isTriangleInsideAABB", &Octree::isTriangleInsideAABB,
        "root", &Octree::root
    );

    lua.new_usertype<CubeGrid3D>("CubeGrid3D",
        sol::constructors<CubeGrid3D()>(),
        "box", &CubeGrid3D::box,
        "posX", &CubeGrid3D::posX,
        "posY", &CubeGrid3D::posY,
        "posZ", &CubeGrid3D::posZ,
        "passed", &CubeGrid3D::passed
    );

    lua.new_usertype<Grid3D>("Grid3D",
        sol::constructors<Grid3D(AABB3D, int, int, int)>(),
        "isEmpty", &Grid3D::isEmpty,
        "getCubeFromPosition", &Grid3D::getCubeFromPosition,
        "getNumberCubesX", &Grid3D::getNumberCubesX,
        "getNumberCubesY", &Grid3D::getNumberCubesY,
        "getNumberCubesZ", &Grid3D::getNumberCubesZ,
        "getBoxes", &Grid3D::getBoxes,
        "setTravel", &Grid3D::setTravel,
        "MakeTravelCubesGrid", &Grid3D::MakeTravelCubesGrid,
        "computePath", &Grid3D::computePath,
        "fillGrid3DFromImage", sol::overload(
            [](Grid3D& g, const std::string& path, int threshold)                   { g.fillGrid3DFromImage(path, threshold); },
            [](Grid3D& g, const std::string& path, int threshold, bool fz, bool fx) { g.fillGrid3DFromImage(path, threshold, fz, fx); }
        ),
        "drawDebug", sol::overload(
            [](Grid3D& g)              { g.drawDebug(); },
            [](Grid3D& g, Color color) { g.drawDebug(color); }
        ),
        "isCellWalkable", &Grid3D::isCellWalkable,
        "snapToWalkable", &Grid3D::snapToWalkable,
        "requestPath", [](Grid3D& g, const std::string& unitName, int gx1, int gz1, int gx2, int gz2) {
            Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(
                std::make_shared<ThreadJobPathfinding>(&g, unitName, gx1, gz1, gx2, gz2)
            );
        }
    );

    lua.new_usertype<Mesh3D>("Mesh3D",
    sol::base_classes, sol::bases<Object3D>(),
        "AssimpLoadGeometryFromFile", &Mesh3D::AssimpLoadGeometryFromFile,
        "BuildGrid3D", &Mesh3D::BuildGrid3D,
        "BuildOctree", &Mesh3D::BuildOctree,
        "FillGrid3DFromGeometry", &Mesh3D::FillGrid3DFromGeometry,
        "getOctree", &Mesh3D::getOctree,
        "getGrid3D", &Mesh3D::getGrid3D,
        "isRenderPipelineDefault", &Mesh3D::isRenderPipelineDefault,
        "setRenderPipelineDefault",  &Mesh3D::setRenderPipelineDefault
    );

    lua.new_usertype<Mesh3DAnimation>("Mesh3DAnimation",
    sol::base_classes, sol::bases<Mesh3D, Object3D>(),
        "setIndexCurrentAnimation", &Mesh3DAnimation::setIndexCurrentAnimation,
        "setAnimationByName", &Mesh3DAnimation::setAnimationByName,
        "setAnimationSpeed", &Mesh3DAnimation::setAnimationSpeed,
        "isAnimationEnds", &Mesh3DAnimation::isAnimationEnds,
        "setLoop", &Mesh3DAnimation::setLoop,
        "isLoop", &Mesh3DAnimation::isLoop
    );

    lua.new_usertype<Projectile>("Projectile",
    sol::base_classes, sol::bases<Mesh3D, Object3D>(),
        "fire", [](Projectile* p, const Vertex3D& dir, float impulse, float accuracy, float shapeRadius, int collisionGroup, int collisionMask) {
            p->setCollisionsEnabled(true);
            float r = (shapeRadius > 0.001f) ? shapeRadius : 0.1f;
            p->setSimpleShapeSize(Vertex3D(r, r, r));
            p->setCollisionShape(SIMPLE_SHAPE);
            p->setCollisionMode(CollisionMode::BODY);
            p->setLinearDamping(0.0f);
            p->setAngularDamping(0.0f);
            p->setCcdMotionThreshold(0.05f);
            p->setCcdSweptSphereRadius(r);
            p->makeProjectileRigidBody(
                1.0f,
                dir,
                impulse,
                accuracy,
                Components::get()->Collisions()->getDynamicsWorld(),
                SIMPLE_SHAPE,
                collisionGroup,
                collisionMask
            );
        },
        "getDirection", &Projectile::getDirection,
        "setDirection", &Projectile::setDirection
    );

    lua.new_usertype<Image3DAnimation>("BillboardAnimation",
    sol::base_classes, sol::bases<Object3D>(),
        "CreateAnimation", &Image3DAnimation::CreateAnimation,
        "setAnimation", &Image3DAnimation::setAnimation,
        "UpdateBillboardSize", &Image3DAnimation::UpdateBillboardSize
    );

    lua.new_usertype<Image3DAnimation360>("BillboardAnimation8Directions",
        sol::base_classes, sol::bases<Object3D>()
    );

    lua.new_usertype<ScriptLUATypeData>("ScriptLUATypeData",
    "name", &ScriptLUATypeData::name,
        "value", &ScriptLUATypeData::value
    );

    lua.new_usertype<Scene>("Scene",
        "getName",    &Scene::getName,
        "getFilePath",&Scene::getFilePath,
        "isActive",   &Scene::isActive,
        "setActive",  &Scene::setActive,
        "getObjects", [](Scene* s, sol::this_state st) -> sol::table {
            sol::state_view lua(st);
            sol::table result = lua.create_table();
            int i = 1;
            for (auto *obj : s->getObjects()) result[i++] = obj;
            return result;
        }
    );

    lua.new_usertype<SceneLoader>("SceneLoader",
        "clearWorld", &SceneLoader::ClearWorld,
        "cleanWorld", &SceneLoader::CleanWorld,
        "setSceneActive", [](SceneLoader*, std::string name, bool active) {
            SceneLoader::setSceneActive(name, active);
        },
        "setSceneHidden", [](SceneLoader*, std::string name, bool hidden) {
            SceneLoader::setSceneHidden(name, hidden);
        },
        "unloadScene", [](SceneLoader*, std::string name) {
            SceneLoader::UnloadScene(name);
        },
        "reloadScene", [](SceneLoader*, std::string name) {
            SceneLoader::ReloadScene(name);
        },
        "SaveScene", [](SceneLoader*, std::string path) {
            SceneLoader::SaveScene(FilePath::SceneFile(path));
        },
        "LoadScene", [](SceneLoader*, std::string path) {
            SceneLoader::LoadScene(FilePath::SceneFile(path));
        },
        "loadSceneAdditive", [](SceneLoader*, std::string path,
                                sol::optional<bool> scripts,
                                sol::optional<bool> shaders,
                                sol::optional<bool> camera,
                                sol::optional<bool> renderSettings) {
            SceneLoader::LoadSceneAdditive(
                FilePath::SceneFile(path),
                scripts.value_or(false),
                shaders.value_or(false),
                camera.value_or(false),
                renderSettings.value_or(false)
            );
        }
    );

    lua.new_usertype<Image2D>("Image2D",
    sol::base_classes, sol::bases<Object3D>(),
        "setScreenPosition",    &Image2D::setScreenPosition,
        "setSize",              &Image2D::setSize,
        "setFilePath",          &Image2D::setFilePath,
        "loadVideo",            &Image2D::loadVideo,
        "stopVideo",            &Image2D::stopVideo,
        "isVideoFinished",      &Image2D::isVideoFinished,
        "hasVideo",             &Image2D::hasVideo,
        "setDrawInBackground",  &Image2D::setDrawInBackground,
        "isDrawInBackground",   &Image2D::isDrawInBackground
    );

    lua.new_usertype<Image2DAnimation>("Image2DAnimation",
    sol::base_classes, sol::bases<Object3D>(),
        "updatePosition", &Image2DAnimation::updatePosition
    );

    lua.new_usertype<Image3D>("Image3D",
       sol::base_classes, sol::bases<Object3D>()
    );

    lua.new_usertype<LightPoint>("LightPoint3D",
    sol::base_classes, sol::bases<Object3D>(),
        "setConstant", &LightPoint::setConstant,
        "setLinear", &LightPoint::setLinear,
        "setCuadratic", &LightPoint::setCuadratic,
        "setColor", &LightPoint::setColor,
        "setColorSpecular", &LightPoint::setColorSpecular,
        "setAmbient", &LightPoint::setAmbient,
        "setFrustumCullingEnabled", &LightPoint::setFrustumCullingEnabled,
        "setFrustumCullingOffset",  &LightPoint::setFrustumCullingOffset,
        "getFrustumCullingEnabled", &LightPoint::getFrustumCullingEnabled,
        "getFrustumCullingOffset",  &LightPoint::getFrustumCullingOffset
    );

    lua.new_usertype<LightSpot>("SpotLight3D",
    sol::base_classes, sol::bases<LightPoint, Object3D>(),
        "setCutOff", &LightSpot::setCutOff,
        "setOuterCutOff", &LightSpot::setOuterCutOff,
        "setCastsShadow", &LightSpot::setCastsShadow,
        "getCastsShadow", &LightSpot::getCastsShadow
    );

    lua.new_usertype<ScriptLUA>(
        "ScriptLUA",
        "content", sol::property(
                [](const ScriptLUA& script) { return script.content; },
                [](ScriptLUA& script, const std::string& newContent) { script.content = newContent; }
        ),
        "scriptFilename", sol::property(
                [](const ScriptLUA& script) { return script.getScriptFilename(); }
        ),
        "fileTypes", sol::property(
                [](const ScriptLUA& script) { return script.fileTypes; },
                [](ScriptLUA& script, const std::string& newFileTypes) { script.fileTypes = newFileTypes; }
        ),
        "UpdateFileTypes", &ScriptLUA::UpdateFileTypes,
        "getCode", &ScriptLUA::getCode,
        "dataTypesFileFor", &ScriptLUA::dataTypesFileFor
    );

    lua.new_usertype<ParticlesContext>("ParticlesContext",
    sol::constructors<
            ParticlesContext(),
                ParticlesContext(
                   float, float, float, int, int, int, int, int, int, int, float
           )
        >(),
        "GRAVITY", &ParticlesContext::GRAVITY,
        "PARTICLES_BY_SECOND", &ParticlesContext::PARTICLES_BY_SECOND,
        "PARTICLE_LIFESPAN", &ParticlesContext::PARTICLE_LIFESPAN,
        "SMOKE_ANGLE_RANGE", &ParticlesContext::SMOKE_ANGLE_RANGE,
        "MIN_VELOCITY", &ParticlesContext::MIN_VELOCITY,
        "MAX_VELOCITY", &ParticlesContext::MAX_VELOCITY,
        "MIN_ALPHA", &ParticlesContext::MIN_ALPHA,
        "MAX_ALPHA", &ParticlesContext::MAX_ALPHA,
        "POSITION_NOISE", &ParticlesContext::POSITION_NOISE,
        "VELOCITY_NOISE", &ParticlesContext::VELOCITY_NOISE,
        "DECELERATION_FACTOR", &ParticlesContext::DECELERATION_FACTOR,
        "defaultParticlesContext", &ParticlesContext::defaultParticlesContext
    );

    lua.new_usertype<ParticleEmitter>("ParticleEmitter",
        sol::base_classes, sol::bases<Object3D>(),
        "setContext", &ParticleEmitter::setContext,
        "setColorTo", &ParticleEmitter::setColorTo,
        "setColorFrom", &ParticleEmitter::setColorFrom,
        "setTexture", sol::overload(
            &ParticleEmitter::setTexture,
            [](ParticleEmitter& e, const std::string& path) {
                auto* img = new Image(FilePath::ImageFile(path));
                e.setTexture(img);
            }
        ),
        "setStopAdd", &ParticleEmitter::setStopAdd,
        "isActive", &ParticleEmitter::isActive,
        "getColorTo", &ParticleEmitter::getColorTo,
        "getColorFrom", &ParticleEmitter::getColorFrom,
        "getTexture", &ParticleEmitter::getTexture,
        "setGPUMode", &ParticleEmitter::setGPUMode,
        "isGPUMode", &ParticleEmitter::isGPUMode,
        "createAttachedLight", &ParticleEmitter::createAttachedLight,
        "removeAttachedLight", &ParticleEmitter::removeAttachedLight,
        "getAttachedLight", &ParticleEmitter::getAttachedLight
    );

    lua.new_enum("CollisionShape",
       "SIMPLE_SHAPE", SIMPLE_SHAPE,
       "TRIANGLE_MESH_SHAPE", TRIANGLE_MESH_SHAPE
    );

    lua.new_enum("CollisionGroups",
        "Player",          Config::Player,
        "Enemy",           Config::Enemy,
        "ProjectileGroup", Config::Projectile,
        "ProjectileEnemy", Config::ProjectileEnemy,
        "Health",          Config::Health,
        "Weapon",          Config::Weapon,
        "StaticWorld",     Config::StaticWorld,
        "AllFilter",       Config::AllFilter
    );

    lua.new_usertype<TextWriter>(
        "TextWriter",
        "writeTextTTFAutoSize", &TextWriter::WriteTextTTFAutoSize,
        "writeTextTTF", &TextWriter::WriteTextTTF,
        "writeTextTTFMiddleScreen", &TextWriter::WriteTextTTFMiddleScreen,
        "writeTTFCenterHorizontal", &TextWriter::WriteTTFCenterHorizontal,
        "getAlpha", &TextWriter::getAlpha,
        "setAlpha", &TextWriter::setAlpha,
        "setFont", &TextWriter::setFont,
        "beginTextCache", &TextWriter::beginTextCache,
        "endTextCache",   &TextWriter::endTextCache,
        "drawTextCache",  &TextWriter::drawTextCache,
        "writeTextAtlas",      &TextWriter::writeTextAtlas,
        "writeTextAtlasCache", &TextWriter::writeTextAtlasCache,
        "flushTextBatch",      &TextWriter::flushTextBatch,
        "buildGlyphAtlas", &TextWriter::buildGlyphAtlas,
        "getGlyphAtlas",   &TextWriter::getGlyphAtlas
    );

    lua.new_usertype<GlyphAtlas>("GlyphAtlas",
        "isBuilt", &GlyphAtlas::isBuilt,
        "getAtlasTexture", &GlyphAtlas::getAtlasTexture,
        "getLineHeight", &GlyphAtlas::getLineHeight,
        "getAscent", &GlyphAtlas::getAscent
    );

    lua.new_usertype<Image>("Image",
        "createEmpty",  [](int w, int h) { return Image::createEmpty(w, h); },
        "destroy",      &Image::destroy,
        "clearChannel", &Image::clearChannel,
        "fillCircle",   &Image::fillCircle,
        "upload",       &Image::upload
    );

    lua.new_usertype<ShaderBaseCustomOGLCode>("ShaderOpenGLCustom",
            "setDataTypeValue", sol::overload(
                [](ShaderBaseCustomOGLCode& shader, const std::string& name, const sol::object& value) {
                    if (value.is<int>()) {
                        shader.setDataTypeValue(name, static_cast<float>(value.as<int>()));  // Convierte int a float
                    } else if (value.is<double>()) {
                        shader.setDataTypeValue(name, static_cast<float>(value.as<double>()));  // Convierte double a float
                    } else if (value.is<float>()) {
                        shader.setDataTypeValue(name, value.as<float>());
                    } else if (value.is<glm::vec2>()) {
                        shader.setDataTypeValue(name, value.as<glm::vec2>());
                    } else if (value.is<glm::vec3>()) {
                        shader.setDataTypeValue(name, value.as<glm::vec3>());
                    } else if (value.is<glm::vec4>()) {
                        shader.setDataTypeValue(name, value.as<glm::vec4>());
                    } else {
                        std::cerr << "Error: Tipo no soportado en setDataTypeValue para '" << name << "'\n";
                    }
                },
                static_cast<void (ShaderBaseCustomOGLCode::*)(const std::string&, int)>(&ShaderBaseCustomOGLCode::setDataTypeValue),
                static_cast<void (ShaderBaseCustomOGLCode::*)(const std::string&, float)>(&ShaderBaseCustomOGLCode::setDataTypeValue),
                static_cast<void (ShaderBaseCustomOGLCode::*)(const std::string&, glm::vec2)>(&ShaderBaseCustomOGLCode::setDataTypeValue),
                static_cast<void (ShaderBaseCustomOGLCode::*)(const std::string&, glm::vec3)>(&ShaderBaseCustomOGLCode::setDataTypeValue),
                static_cast<void (ShaderBaseCustomOGLCode::*)(const std::string&, glm::vec4)>(&ShaderBaseCustomOGLCode::setDataTypeValue)
            ),
            "setTextureValue", [](ShaderBaseCustomOGLCode& s, const std::string& name, Image* img) {
                s.setDataTypeValue(name, ShaderOpenGLCustomDataValue(img));
            },
            "setEnabled", [](ShaderBaseCustomOGLCode& s, bool v) { s.setEnabled(v); },
            "isEnabled",  [](ShaderBaseCustomOGLCode& s) { return s.isEnabled(); }
    );

    lua.new_usertype<glm::vec2>("vec2",
        sol::constructors<glm::vec2(float, float)>(),
        "x", &glm::vec2::x,
        "y", &glm::vec2::y
    );

    lua.new_usertype<glm::vec3>("vec3",
        sol::constructors<glm::vec3(float, float, float)>(),
        "x", &glm::vec3::x,
        "y", &glm::vec3::y,
        "z", &glm::vec3::z
    );

    lua.new_usertype<glm::vec4>("vec4",
        sol::constructors<glm::vec4(float, float, float, float)>(),
        "x", &glm::vec4::x,
        "y", &glm::vec4::y,
        "z", &glm::vec4::z,
        "w", &glm::vec4::w
    );

    lua.new_usertype<CollisionInfo>("CollisionInfo",
        sol::constructors<CollisionInfo(void*, int, int)>(),
        "with", &CollisionInfo::with,
        "getSource", &CollisionInfo::getSource,
        "getBoneIndexMapping", &CollisionInfo::getBoneIndexMapping,
        "getObject", &CollisionInfo::getObject
    );

    lua.new_usertype<ObjectFactory>("ObjectFactory",
        "Object3D",           &ObjectFactory::CreateObject3D,
        "Image2D",            &ObjectFactory::CreateImage2D,
        "Image2DAnimation",   &ObjectFactory::CreateImage2DAnimation,
        "Image3D",            &ObjectFactory::CreateImage3D,
        "Image3DAnimation",   &ObjectFactory::CreateImage3DAnimation,
        "Image3DAnimation360",&ObjectFactory::CreateImage3DAnimation360,
        "Mesh3D",             &ObjectFactory::CreateMesh3D,
        "Mesh3DAnimation",    &ObjectFactory::CreateMesh3DAnimation,
        "LightPoint",         &ObjectFactory::CreateLightPoint,
        "LightSpot",          &ObjectFactory::CreateLightSpot,
        "ParticleEmitter",    &ObjectFactory::CreateParticleEmitter,
        "Projectile",         &ObjectFactory::CreateProjectile,
        "TextWriter",         &ObjectFactory::CreateTextWriter,
        "ScriptLUA",          &ObjectFactory::CreateScriptLUA,
        "PlayVideoCutscene",  [](ObjectFactory*, const std::string &path) {
            VideoPlayer vp(path);
            vp.play();
        }
    );
}

#endif //BRAKEZA3D_LUAINTEGRATION_H
