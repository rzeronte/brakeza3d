#ifndef BRAKEDA3D_COMPONENTSMANAGER_H
#define BRAKEDA3D_COMPONENTSMANAGER_H

#include <iostream>
#include "Components/Component.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentCollisions.h"
#include "Components/ComponentRender.h"
#include "Components/ComponentInput.h"
#include "Components/ComponentSound.h"
#include "Components/ComponentWindow.h"

enum ComponentID {
    COMPONENT_WINDOW = 0,
    COMPONENT_CAMERA = 1,
    COMPONENT_COLLISIONS = 2,
    COMPONENT_INPUT = 3,
    COMPONENT_SOUND = 4,
    COMPONENT_RENDER = 5,
    COMPONENT_MENU = 6,
    COMPONENT_GAME = 7,
    COMPONENT_HUD = 8,
    COMPONENT_GAME_INPUT = 9
};

class ComponentsManager {
public:
    std::vector<Component *> components;

    virtual ~ComponentsManager();

    static ComponentsManager *get();

    static ComponentsManager *instance;

    void registerComponent(Component *component, const std::string& label);

    ComponentCamera *getComponentCamera() {
        return dynamic_cast<ComponentCamera *>(components[ComponentID::COMPONENT_CAMERA]);
    };

    ComponentCollisions *getComponentCollisions() {
        return dynamic_cast<ComponentCollisions *>(components[ComponentID::COMPONENT_COLLISIONS]);
    };

    ComponentWindow *getComponentWindow() {
        return dynamic_cast<ComponentWindow *>(components[ComponentID::COMPONENT_WINDOW]);
    };

    ComponentRender *getComponentRender() {
        return dynamic_cast<ComponentRender *>(components[ComponentID::COMPONENT_RENDER]);
    };

    ComponentInput *getComponentInput() {
        return dynamic_cast<ComponentInput *>(components[ComponentID::COMPONENT_INPUT]);
    };

    ComponentSound *getComponentSound() {
        return dynamic_cast<ComponentSound *>(components[ComponentID::COMPONENT_SOUND]);
    };

    // -- Custom components here --

};


#endif //BRAKEDA3D_COMPONENTSMANAGER_H
