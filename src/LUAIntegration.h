//
// Created by eduardo on 5/10/23.
//

#ifndef BRAKEZA3D_LUAINTEGRATION_H
#define BRAKEZA3D_LUAINTEGRATION_H


#include "../sol/sol.hpp"
#include "../include/Objects/Vertex3D.h"
#include "../include/Objects/Object3D.h"
#include "../ProjectDarkHeaZ/Player.h"
#include "../include/Components/ComponentCamera.h"
#include "../include/ComponentsManager.h"
#include "../include/Brakeza3D.h"

void LUAIntegration(sol::state &lua)
{
    lua.open_libraries(sol::lib::base);

    lua.new_usertype<Vertex3D>("Vertex3D",
                               sol::constructors<Vertex3D(), Vertex3D(float, float, float)>());

    lua.new_usertype<Object3D>("Object",
                               "addToPosition", &Object3D::addToPosition,
                               "getPosition", &Object3D::getPosition,
                               "setPosition", &Object3D::setPosition,
                               "getTypeObject", &Object3D::getTypeObject,
                               "getLabel", &Object3D::getLabel
    );

    lua.new_usertype<Component>("Component",
                            "setEnabled", &Component::setEnabled,
                            "isEnabled", &Component::isEnabled
    );

    lua.new_usertype<ComponentCamera>("ComponentCamera",
                                      sol::base_classes, sol::bases<Component>(),
                                      "getCamera", &ComponentCamera::getCamera
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
                                        "getComponentGame", &ComponentsManager::getComponentGame,
                                        "getComponentCollisions", &ComponentsManager::getComponentCollisions,
                                        "getComponentInput", &ComponentsManager::getComponentInput,
                                        "getComponentGameInput", &ComponentsManager::getComponentGameInput,
                                        "getComponentHUD", &ComponentsManager::getComponentHUD,
                                        "getComponentMenu", &ComponentsManager::getComponentMenu
    );


    lua.new_usertype<Brakeza3D>("Brakeza3D",
                                "getDeltaTime", &Brakeza3D::getDeltaTime,
                                "getExecutionTime", &Brakeza3D::getExecutionTime,
                                "addObject3D",  &Brakeza3D::addObject3D
    );

    lua.new_usertype<Camera3D>("Camera3D",
                               sol::base_classes, sol::bases<Object3D>()
    );

    lua.new_usertype<Player>("Player",
                             "isStucked", &Player::isStucked,
                             sol::base_classes, sol::bases<Object3D>()
    );

}

#endif //BRAKEZA3D_LUAINTEGRATION_H
