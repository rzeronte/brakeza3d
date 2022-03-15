#ifndef BRAKEDA3D_COMPONENTSMANAGER_H
#define BRAKEDA3D_COMPONENTSMANAGER_H

#include <iostream>
#include "Components/Component.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentCollisions.h"
#include "Components/ComponentHUD.h"
#include "Components/ComponentRender.h"
#include "Components/ComponentInput.h"
#include "Components/ComponentGame.h"
#include "Components/ComponentGameInput.h"
#include "Components/ComponentMenu.h"
#include "Components/ComponentConsole.h"

enum ComponentID {
    COMPONENT_WINDOW = 0,
    COMPONENT_CAMERA = 1,
    COMPONENT_COLLISIONS = 2,
    COMPONENT_INPUT = 3,
    COMPONENT_SOUND = 4,
    COMPONENT_CONSOLE = 5,
    COMPONENT_RENDER = 6,
    COMPONENT_MENU = 7,
    COMPONENT_HUD = 8,
    COMPONENT_GAME = 9,
    COMPONENT_GAME_INPUT = 10
};

class ComponentsManager {
public:
    std::vector<Component *> components;
    std::vector<Object3D *> *sceneObjects;

    static ComponentsManager *get();

    static ComponentsManager *instance;

    void registerComponent(Component *component, const std::string& label);

    void configureComponents();

    std::vector<Object3D *> *getSceneObjects() const;

    void setSceneObjects(std::vector<Object3D *> *sceneObjects);

    ComponentCamera *
    getComponentCamera() { return static_cast<ComponentCamera *>(components[ComponentID::COMPONENT_CAMERA]); };

    ComponentCollisions *
    getComponentCollisions() { return static_cast<ComponentCollisions *>(components[ComponentID::COMPONENT_COLLISIONS]); };

    ComponentMenu *getComponentMenu() { return static_cast<ComponentMenu *>(components[ComponentID::COMPONENT_MENU]); };

    ComponentHUD *getComponentHUD() { return static_cast<ComponentHUD *>(components[ComponentID::COMPONENT_HUD]); };

    ComponentWindow *
    getComponentWindow() { return static_cast<ComponentWindow *>(components[ComponentID::COMPONENT_WINDOW]); };

    ComponentRender *
    getComponentRender() { return static_cast<ComponentRender *>(components[ComponentID::COMPONENT_RENDER]); };

    ComponentInput *
    getComponentInput() { return static_cast<ComponentInput *>(components[ComponentID::COMPONENT_INPUT]); };

    ComponentGame *getComponentGame() { return static_cast<ComponentGame *>(components[ComponentID::COMPONENT_GAME]); };

    ComponentGameInput *
    getComponentGameInput() { return static_cast<ComponentGameInput *>(components[ComponentID::COMPONENT_GAME_INPUT]); };

    ComponentConsole *
    getComponentConsole() { return static_cast<ComponentConsole *>(components[ComponentID::COMPONENT_CONSOLE]); };

};


#endif //BRAKEDA3D_COMPONENTSMANAGER_H
