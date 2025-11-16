#ifndef BRAKEDA3D_COMPONENTSMANAGER_H
#define BRAKEDA3D_COMPONENTSMANAGER_H

#include "Components/Component.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentCollisions.h"
#include "Components/ComponentRender.h"
#include "Components/ComponentInput.h"
#include "Components/ComponentSound.h"
#include "Components/ComponentWindow.h"
#include "Components/ComponentScripting.h"

enum ComponentID {
    COMPONENT_WINDOW = 0,
    COMPONENT_SCRIPTING = 1,
    COMPONENT_CAMERA = 2,
    COMPONENT_COLLISIONS = 3,
    COMPONENT_INPUT = 4,
    COMPONENT_SOUND = 5,
    COMPONENT_RENDER = 6
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

    ComponentScripting *getComponentScripting() {
        return dynamic_cast<ComponentScripting *>(components[ComponentID::COMPONENT_SCRIPTING]);
    };
};


#endif //BRAKEDA3D_COMPONENTSMANAGER_H
