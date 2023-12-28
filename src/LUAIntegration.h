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
    lua.new_usertype<Vertex3D>("Vertex3D",
                               sol::constructors<Vertex3D(), Vertex3D(float, float, float)>());

    lua.new_usertype<Object3D>("Object3D",
                               "addToPosition", &Object3D::addToPosition,
                               "getPosition", &Object3D::getPosition,
                               "setPosition", &Object3D::setPosition,
                               "getTypeObject", &Object3D::getTypeObject,
                               "getLabel", &Object3D::getLabel,
                               "setBelongToScene", &Object3D::getLabel,
                               "setRotationFrame", &Object3D::setRotationFrame,
                               "setStencilBufferEnabled", &Object3D::setStencilBufferEnabled,
                               "setRotationFrameEnabled", &Object3D::setRotationFrameEnabled,
                               "setEnabled", &Object3D::setEnabled,
                               "setRemoved", &Object3D::setRemoved,
                               "setLabel", &Object3D::setLabel,
                               "setScale", &Object3D::setScale
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
                                     "getJSON", &FXEffectOpenGL::getJSON
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
