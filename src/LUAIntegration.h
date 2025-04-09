//
// Created by eduardo on 5/10/23.
//

#ifndef BRAKEZA3D_LUAINTEGRATION_H
#define BRAKEZA3D_LUAINTEGRATION_H

#include "../sol/sol.hpp"
#include "../include/Objects/Vertex3D.h"
#include "../include/Objects/Object3D.h"
#include "../include/Objects/ScriptLUA.h"
#include "../include/Components/ComponentCamera.h"
#include "../include/ComponentsManager.h"
#include "../include/Brakeza3D.h"
#include "../include/2D/Image2D.h"
#include "../include/2D/Image2DAnimation.h"
#include "../include/Objects/Mesh3DAnimation.h"
#include "../include/Objects/Image3D.h"
#include "../include/Objects/BillboardAnimation.h"
#include "../include/Objects/BillboardAnimation8Directions.h"
#include "../include/Objects/ParticleEmitter.h"

void LUAIntegration(sol::state &lua)
{
    lua.new_usertype<Vertex3D>(
        "Vertex3D",
        sol::constructors<Vertex3D(), Vertex3D(float, float, float)>(),
        "x", sol::property(&Vertex3D::x, &Vertex3D::x),
        "y", sol::property(&Vertex3D::y, &Vertex3D::y),
        "z", sol::property(&Vertex3D::z, &Vertex3D::z),
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
            sol::resolve<Vertex3D(Vertex3D const)>(&M3::operator*),
            sol::resolve<M3(const M3&)>(&M3::operator*)
        ),
        "getMatrixRotationForEulerAngles", &M3::getMatrixRotationForEulerAngles,
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
        "addToPosition", &Object3D::addToPosition,
        "getPosition", &Object3D::getPosition,
        "setPosition", &Object3D::setPosition,
        "setRotation", sol::overload(
            static_cast<void (Object3D::*)(M3)>(&Object3D::setRotation)
        ),
        "getTypeObject", &Object3D::getTypeObject,
        "getLabel", &Object3D::getLabel,
        "getRotation", &Object3D::getRotation,
        "setBelongToScene", &Object3D::setBelongToScene,
        "setEnabled", &Object3D::setEnabled,
        "setRemoved", &Object3D::setRemoved,
        "setLabel", &Object3D::setLabel,
        "getScale", &Object3D::getScale,
        "setScale", &Object3D::setScale,
        "getModelMatrix", &Object3D::getModelMatrix,
        "getM3ModelMatrix", &Object3D::getM3ModelMatrix,
        "AxisForward", &Object3D::AxisForward,
        "AxisUp", &Object3D::AxisUp,
        "AxisRight", &Object3D::AxisRight,
        "removeCollisionObject", &Object3D::removeCollisionObject,
        "sleepCollider", &Object3D::sleepCollider,
        "setCollisionsEnabled", &Object3D::setCollisionsEnabled,
        "disableSimulationCollider", &Object3D::disableSimulationCollider,
        "enableSimulationCollider", &Object3D::enableSimulationCollider,
        "disableDeactivationCollider", &Object3D::disableDeactivationCollider,
        "UpdateShapeCollider", &Object3D::UpdateShapeCollider,
        "setScalingCollider", &Object3D::setScalingCollider,
        "setCapsuleColliderSize", &Object3D::setCapsuleColliderSize,
        "moveCollider", &Object3D::moveCollider,
        "isCollisionsEnabled", &Object3D::isCollisionsEnabled,
        "setDrawOffset", &Object3D::setDrawOffset,
        "setupGhostCollider", &Object3D::setupGhostCollider,
        "setupRigidBodyCollider", &Object3D::setupRigidBodyCollider,
        "setColliderStatic", &Object3D::setColliderStatic,
        "applyCentralForce", &Object3D::applyCentralForce,
        "applyCentralImpulse", &Object3D::applyCentralImpulse,
        "applyImpulse", &Object3D::applyImpulse,
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
        "jump", &Object3D::jump,
        "onGround", &Object3D::onGround,
        "getLocalScriptVar", &Object3D::getLocalScriptVar,
        "attachScript", &Object3D::attachScript,
        "lookAt", &Object3D::lookAt,
        "reloadScriptsEnvironment", &Object3D::reloadScriptsEnvironment,
        "getLinearVelocity", &Object3D::getLinearVelocity
    );


    lua.new_usertype<SharedLUAContext>("SharedLUAContext",
    "addPair", &SharedLUAContext::addPair,
        "get", &SharedLUAContext::get,
        "set", &SharedLUAContext::set,
        "remove", &SharedLUAContext::remove
    );

    lua.new_usertype<Component>("Component",
    "setEnabled", &Component::setEnabled,
        "isEnabled", &Component::isEnabled
    );

    lua.new_usertype<ComponentCamera>("ComponentCamera",
        sol::base_classes, sol::bases<Component>(),
        "getCamera", &ComponentCamera::getCamera
    );

    lua.new_usertype<ComponentSound>("ComponentSound",
    sol::base_classes, sol::bases<Component>(),
        "addSound", &ComponentSound::addSound,
        "addMusic", &ComponentSound::addMusic,
        "setMusicVolume", &ComponentSound::setMusicVolume,
        "setSoundsVolume", &ComponentSound::setSoundsVolume,
        "playSound", &ComponentSound::playMusic,
        "stopMusic", &ComponentSound::stopMusic
    );

    lua.new_usertype<ComponentCollisions>("ComponentCollisions",
    sol::base_classes, sol::bases<Component>(),
        "isRayCollisionWith", &ComponentCollisions::isRayCollisionWith,
        "setEnableDebugMode", &ComponentCollisions::setEnableDebugMode
    );

    lua.new_usertype<ComponentRender>("ComponentRender",
    sol::base_classes, sol::bases<Component>(),
        "getSceneLoader", &ComponentRender::getSceneLoader,
        "drawLine",  &ComponentRender::drawLine,
        "setGlobalIlluminationDirection", &ComponentRender::setGlobalIlluminationDirection,
        "setGlobalIlluminationAmbient", &ComponentRender::setGlobalIlluminationAmbient,
        "setGlobalIlluminationDiffuse", &ComponentRender::setGlobalIlluminationDiffuse,
        "setGlobalIlluminationSpecular", &ComponentRender::setGlobalIlluminationSpecular,
        "getSceneShaderByLabel", &ComponentRender::getSceneShaderByLabel
    );

    lua.new_usertype<ComponentScripting>("ComponentRender",
    sol::base_classes, sol::bases<Component>(),
        "addSceneLUAScript", &ComponentScripting::addSceneLUAScript,
        "getGlobalScriptVar",  &ComponentScripting::getGlobalScriptVar
    );

    lua.new_usertype<ComponentInput>("ComponentInput",
    sol::base_classes, sol::bases<Component>(),
        "isKeyEventDown", &ComponentInput::isKeyEventDown,
        "isKeyEventUp", &ComponentInput::isKeyEventUp,
        "isCharPressed", &ComponentInput::isCharPressed,
        "isCharFirstEventDown", &ComponentInput::isCharFirstEventDown,
        "isMouseMotion", &ComponentInput::isMouseMotion,
        "isClickLeft", &ComponentInput::isClickLeft,
        "isClickRight", &ComponentInput::isClickRight,
        "isEnabled", &ComponentInput::isEnabled,
        "isAnyControllerButtonPressed", &ComponentInput::isAnyControllerButtonPressed,
        "getRelativeRendererMouseX", &ComponentInput::getRelativeRendererMouseX,
        "getRelativeRendererMouseY", &ComponentInput::getRelativeRendererMouseY,
        "getMouseMotionXRel", &ComponentInput::getMouseMotionXRel,
        "getMouseMotionYRel", &ComponentInput::getMouseMotionYRel,
        "isLeftMouseButtonPressed", &ComponentInput::isLeftMouseButtonPressed,
        "isRightMouseButtonPressed", &ComponentInput::isRightMouseButtonPressed,
        "isGameControllerEnabled", &ComponentInput::isGameControllerEnabled,
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

    lua.new_usertype<ComponentsManager>("ComponentsManager",
    "getComponentWindow", &ComponentsManager::getComponentWindow,
        "getComponentRender", &ComponentsManager::getComponentRender,
        "getComponentCamera", &ComponentsManager::getComponentCamera,
        "getComponentCollisions", &ComponentsManager::getComponentCollisions,
        "getComponentInput", &ComponentsManager::getComponentInput
    );

    lua.new_usertype<Brakeza3D>("Brakeza3D",
        "getDeltaTime", &Brakeza3D::getDeltaTime,
        "getDeltaTimeMicro",  &Brakeza3D::getDeltaTimeMicro,
        "getExecutionTime", &Brakeza3D::getExecutionTime,
        "uniqueObjectLabel", &Brakeza3D::uniqueObjectLabel,
        "shutdown", &Brakeza3D::shutdown,
        "addObject3D",  &Brakeza3D::addObject3D,
        "getSceneObjectByLabel",&Brakeza3D::getSceneObjectByLabel
    );

    lua.new_usertype<Camera3D>("Camera3D",
       sol::base_classes, sol::bases<Object3D>(),
        "getM3ViewMatrix", &Camera3D::getM3ViewMatrix,
       "setFOV", &Camera3D::setFOV,
       "getM3ProjectionMatrix", &Camera3D::getM3ProjectionMatrix,
       "setRotationFromEulerAngles", &Camera3D::setRotationFromEulerAngles
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
        "makeTravelCubesGrid", &Grid3D::makeTravelCubesGrid
    );

    lua.new_usertype<Mesh3D>("Mesh3D",
    sol::base_classes, sol::bases<Object3D>(),
        "AssimpLoadGeometryFromFile", &Mesh3D::AssimpLoadGeometryFromFile,
        "buildGrid3D", &Mesh3D::buildGrid3D,
        "buildOctree", &Mesh3D::buildOctree,
        "fillGrid3DFromGeometry", &Mesh3D::fillGrid3DFromGeometry,
        "getOctree", &Mesh3D::getOctree,
        "getGrid3D", &Mesh3D::getGrid3D,
        "create", sol::factories([](Vertex3D position, const std::string& imageFile) {
            return Mesh3D::create(position, imageFile);
        })
    );

    lua.new_usertype<Mesh3DAnimation>("Mesh3DAnimation",
    sol::base_classes, sol::bases<Mesh3D, Object3D>(),
        "create", sol::factories([](Vertex3D position, const std::string& imageFile) {
            return Mesh3DAnimation::create(position,imageFile);
        }),
        "setIndexCurrentAnimation", &Mesh3DAnimation::setIndexCurrentAnimation,
        "setAnimationByName", &Mesh3DAnimation::setAnimationByName,
        "setAnimationSpeed", &Mesh3DAnimation::setAnimationSpeed,
        "isAnimationEnds", &Mesh3DAnimation::isAnimationEnds,
        "setLoop", &Mesh3DAnimation::setLoop,
        "isLoop", &Mesh3DAnimation::isLoop
    );

    lua.new_usertype<BillboardAnimation>("BillboardAnimation",
    sol::base_classes, sol::bases<Object3D>(),
        "create", sol::factories([](Vertex3D position, float w, float h, const std::string& imageFile, int spriteW, int spriteH, int numFrames, int fps) {
            return BillboardAnimation::create(position, w, h, imageFile, spriteW, spriteH, numFrames, fps);
        }),
        "addAnimation", &BillboardAnimation::addAnimation,
        "setAnimation", &BillboardAnimation::setAnimation,
        "updateBillboardSize", &BillboardAnimation::updateBillboardSize
    );

    lua.new_usertype<BillboardAnimation8Directions>("BillboardAnimation8Directions",
    sol::base_classes, sol::bases<Object3D>(),
        "create", sol::factories([](Vertex3D position, float w, float h, const std::string& spriteFolder, int numFrames, int fps) {
          return BillboardAnimation8Directions::create(position, w, h, spriteFolder, numFrames, fps);
        })
    );

    lua.new_usertype<ScriptLUATypeData>("ScriptLUATypeData",
    "name", &ScriptLUATypeData::name,
        "value", &ScriptLUATypeData::value
    );

    lua.new_usertype<SceneLoader>("SceneLoader",
    "clearScene", &SceneLoader::clearScene,
        "saveScene", &SceneLoader::saveScene,
        "loadScene", &SceneLoader::loadScene
    );

    lua.new_usertype<FXBase>("FXEffectBase",
                             "getLabel", &FXBase::getLabel,
                             "setLabel", &FXBase::setLabel,
                             "isEnabled", &FXBase::isEnabled,
                             "setEnabled", &FXBase::setEnabled
    );

    lua.new_usertype<FXOpenGL>("ShaderOpenCL",
                               sol::base_classes, sol::bases<FXBase>(),
                               "getTypesJSON", &FXOpenGL::getJSON
    );

    lua.new_usertype<FXEffectOpenGLObject>("ObjectShaderOpenCL",
    sol::base_classes, sol::bases<FXOpenGL>(),
        "setObject", &FXEffectOpenGLObject::setObject,
        "getObject", &FXEffectOpenGLObject::getObject
    );

    lua.new_usertype<FXOutliner>("FXOutliner",
        sol::base_classes, sol::bases<FXOpenGL>(),
        "create", &FXOutliner::create,
        "setSize", &FXOutliner::setSize,
        "getSize", &FXOutliner::getSize
    );

    lua.new_usertype<Image2D>("Image2D",
    sol::base_classes, sol::bases<Object3D>(),
        "create", sol::factories([](int x, int y, const std::string& imageFile) {
            return Image2D::create(x, y, imageFile);
        }),
        "updatePosition", &Image2D::updatePosition
    );

    lua.new_usertype<Image2DAnimation>("Image2DAnimation",
    sol::base_classes, sol::bases<Object3D>(),
        "create", sol::factories([](int x, int y, const std::string& imageFile, int w, int h, int frames, int fps) {
            return Image2DAnimation::create(x, y, imageFile, w, h, frames, fps);
        }),
        "updatePosition", &Image2DAnimation::updatePosition
    );

    lua.new_usertype<Image3D>("Image3D",
       sol::base_classes, sol::bases<Object3D>(),
       "create", sol::factories([](Vertex3D p, float w, float h, const std::string& imageFile) {
            return Image3D::create(p, w, h, imageFile);
        })
    );

    lua.new_usertype<LightPoint3D>("LightPoint3D",
    sol::base_classes, sol::bases<Object3D>(),
        "setConstant", &LightPoint3D::setConstant,
        "setLinear", &LightPoint3D::setLinear,
        "setCuadratic", &LightPoint3D::setCuadratic,
        "setColor", &LightPoint3D::setColor,
        "setColorSpecular", &LightPoint3D::setColorSpecular,
        "setAmbient", &LightPoint3D::setAmbient,
        "create", sol::factories([](Vertex3D p) {
            return LightPoint3D::create(p);
        })
    );

    lua.new_usertype<SpotLight3D>("SpotLight3D",
    sol::base_classes, sol::bases<LightPoint3D, Object3D>(),
        "setCutOff", &SpotLight3D::setCutOff,
        "setOuterCutOff", &SpotLight3D::setOuterCutOff,
        "setDirection", &SpotLight3D::setDirection,
        "create", sol::factories([](Vertex3D position, Vertex3D direction) {
            return SpotLight3D::create(position, direction);
        })
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
        "updateFileTypes", &ScriptLUA::updateFileTypes,
        "getCode", &ScriptLUA::getCode,
        "dataTypesFileFor", &ScriptLUA::dataTypesFileFor,
        "create", sol::factories([](const std::string& scriptFile) {
            return ScriptLUA::create(scriptFile);
        })
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
        "create", sol::factories([](
              Vertex3D position,
              float ttl,
              Color cf,
              Color ct,
              ParticlesContext context,
              const std::string& imageFile
        ) {
            return ParticleEmitter::create(position, ttl, cf, ct, context, imageFile);
        })
    );

    lua.new_enum("CollisionShape",
       "SIMPLE_SHAPE", SIMPLE_SHAPE,
       "TRIANGLE_MESH_SHAPE", TRIANGLE_MESH_SHAPE
    );

    lua.new_usertype<TextWriter>(
        "TextWriter",
        "writeTextTTFAutoSize", &TextWriter::writeTextTTFAutoSize,
        "writeTextTTF", &TextWriter::writeTextTTF,
        "writeTextTTFMiddleScreen", &TextWriter::writeTextTTFMiddleScreen,
        "writeTTFCenterHorizontal", &TextWriter::writeTTFCenterHorizontal,
        "getAlpha", &TextWriter::getAlpha,
        "setAlpha", &TextWriter::setAlpha,
        "setFont", &TextWriter::setFont,
        "create", sol::factories([](const std::string& fontFile) {
            return TextWriter::create(fontFile);
        })
    );

    lua.new_usertype<ShaderOpenGLCustom>("ShaderOpenGLCustom",
            "setDataTypeValue", sol::overload(
                [](ShaderOpenGLCustom& shader, const std::string& name, const sol::object& value) {
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
                static_cast<void (ShaderOpenGLCustom::*)(const std::string&, int)>(&ShaderOpenGLCustom::setDataTypeValue),
                static_cast<void (ShaderOpenGLCustom::*)(const std::string&, float)>(&ShaderOpenGLCustom::setDataTypeValue),
                static_cast<void (ShaderOpenGLCustom::*)(const std::string&, glm::vec2)>(&ShaderOpenGLCustom::setDataTypeValue),
                static_cast<void (ShaderOpenGLCustom::*)(const std::string&, glm::vec3)>(&ShaderOpenGLCustom::setDataTypeValue),
                static_cast<void (ShaderOpenGLCustom::*)(const std::string&, glm::vec4)>(&ShaderOpenGLCustom::setDataTypeValue)
            )
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
}

#endif //BRAKEZA3D_LUAINTEGRATION_H
