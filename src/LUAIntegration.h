//
// Created by eduardo on 5/10/23.
//

#ifndef BRAKEZA3D_LUAINTEGRATION_H
#define BRAKEZA3D_LUAINTEGRATION_H

#include "../sol/sol.hpp"
#include "../include/Objects/Vertex3D.h"
#include "../include/Objects/Object3D.h"
#include "../include/Components/ComponentCamera.h"
#include "../include/ComponentsManager.h"
#include "../include/Brakeza3D.h"

void LUAIntegration(sol::state &lua)
{
    lua.new_usertype<Vertex3D>(
        "Vertex3D",
        sol::constructors<Vertex3D(), Vertex3D(float, float, float)>(),
        "x", sol::property(&Vertex3D::x, &Vertex3D::x),
        "y", sol::property(&Vertex3D::y, &Vertex3D::y),
        "z", sol::property(&Vertex3D::z, &Vertex3D::z),
        "__add", sol::overload(&Vertex3D::operator+),
        "getScaled", sol::overload(
                static_cast<Vertex3D (Vertex3D::*)(float) const>(&Vertex3D::getScaled),
                static_cast<Vertex3D (Vertex3D::*)(float, float, float) const>(&Vertex3D::getScaled)
        )
    );

    lua.new_usertype<M3>(
            "M3",
            sol::constructors<M3()>(),
            "__mul", sol::overload(
                    sol::resolve<Vertex3D(Vertex3D const)>(&M3::operator*)
            ),
    "getMatrixRotationForEulerAngles", &M3::getMatrixRotationForEulerAngles
    );

    lua.new_usertype<Object3D>(
            "Object3D",  // Nombre del tipo en Lua
            // Lista de funciones y propiedades que deseas exponer
            "addToPosition", &Object3D::addToPosition,
            "getPosition", &Object3D::getPosition,
            "setPosition", &Object3D::setPosition,
            "setRotation", sol::overload(
                    static_cast<void (Object3D::*)(M3)>(&Object3D::setRotation)  // Ajusta el tipo de argumento
            ),
            "getTypeObject", &Object3D::getTypeObject,
            "getLabel", &Object3D::getLabel,
            "setBelongToScene", &Object3D::setBelongToScene,
            "setRotationFrame", &Object3D::setRotationFrame,
            "setStencilBufferEnabled", &Object3D::setStencilBufferEnabled,
            "setRotationFrameEnabled", &Object3D::setRotationFrameEnabled,
            "setEnabled", &Object3D::setEnabled,
            "setRemoved", &Object3D::setRemoved,
            "setLabel", &Object3D::setLabel,
            "getScale", &Object3D::getScale,
            "setScale", &Object3D::setScale,
            "AxisForward", &Object3D::AxisForward,
            "getLocalScriptVar", &Object3D::getLocalScriptVar
    );


    lua.new_usertype<SharedLUAContext>("SharedLUAContext",
                               "prueba", &SharedLUAContext::prueba,
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

    lua.new_usertype<ComponentRender>("ComponentRender",
                                      sol::base_classes, sol::bases<Component>(),
                                      "getSceneLoader", &ComponentRender::getSceneLoader
    );

    lua.new_usertype<ComponentInput>("ComponentInput",
                                     sol::base_classes, sol::bases<Component>(),
                                     "isKeyEventDown", &ComponentInput::isKeyEventDown,
                                     "isKeyEventUp", &ComponentInput::isKeyEventUp,
                                     "isCharPressed", &ComponentInput::isCharPressed,
                                     "isClickLeft", &ComponentInput::isClickLeft,
                                     "isClickRight", &ComponentInput::isClickRight,
                                     "isEnabled", &ComponentInput::isEnabled,
                                     "isAnyControllerButtonPressed", &ComponentInput::isAnyControllerButtonPressed,
                                     "isLeftMouseButtonPressed", &ComponentInput::isLeftMouseButtonPressed,
                                     "isRightMouseButtonPressed", &ComponentInput::isRightMouseButtonPressed

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
                                "getExecutionTime", &Brakeza3D::getExecutionTime,
                                "uniqueObjectLabel", &Brakeza3D::uniqueObjectLabel,
                                "LoadDemo", &Brakeza3D::LoadDemo,
                                "addObject3D",  &Brakeza3D::addObject3D,
                                "getSceneObjectById",&Brakeza3D::getSceneObjectById,
                                "getSceneObjectByLabel",&Brakeza3D::getSceneObjectByLabel
    );

    lua.new_usertype<Camera3D>("Camera3D",
                               sol::base_classes, sol::bases<Object3D>()
    );

    lua.new_usertype<Mesh3D>("Mesh3D",
                               sol::constructors<Mesh3D(), Mesh3D()>(),
                               sol::base_classes, sol::bases<Object3D>(),
                             "AssimpLoadGeometryFromFile", &Mesh3D::AssimpLoadGeometryFromFile,
                             "addMesh3DShader", &Mesh3D::addMesh3DShader,
                             "create", &Mesh3D::create

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

    lua.new_usertype<FXEffectBase>("FXEffectBase",
                             "getLabel", &FXEffectBase::getLabel,
                             "setLabel", &FXEffectBase::setLabel,
                             "isEnabled", &FXEffectBase::isEnabled,
                             "setEnabled", &FXEffectBase::setEnabled
    );

    lua.new_usertype<FXEffectOpenGL>("ShaderOpenCL",
                                     sol::base_classes, sol::bases<FXEffectBase>(),
                                     "getTypesJSON", &FXEffectOpenGL::getJSON
    );
    lua.new_usertype<FXEffectOpenGLObject>("ObjectShaderOpenCL",
                                           sol::base_classes, sol::bases<FXEffectOpenGL>(),
                                           "setObject", &FXEffectOpenGLObject::setObject,
                                           "getObject", &FXEffectOpenGLObject::getObject
    );

    lua.new_usertype<FXOutliner>("FXOutliner",
                                 sol::base_classes, sol::bases<FXEffectOpenGL>(),
                                 "create", &FXOutliner::create,
                                 "setSize", &FXOutliner::setSize,
                                 "getSize", &FXOutliner::getSize
    );
}

#endif //BRAKEZA3D_LUAINTEGRATION_H
