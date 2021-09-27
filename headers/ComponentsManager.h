#ifndef BRAKEDA3D_COMPONENTSMANAGER_H
#define BRAKEDA3D_COMPONENTSMANAGER_H

#include <iostream>
#include "Components/Component.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentCollisions.h"
#include "GUI/GUIManager.h"
#include "Components/ComponentBSP.h"
#include "Components/ComponentHUD.h"
#include "Components/ComponentGUI.h"
#include "Components/ComponentRender.h"
#include "Components/ComponentInput.h"
#include "Components/ComponentGame.h"
#include "Components/ComponentGameInput.h"
#include "Components/ComponentMenu.h"

enum ComponentID {
    COMPONENT_WINDOW = 0,
    COMPONENT_CAMERA = 1,
    COMPONENT_COLLISIONS = 2,
    COMPONENT_INPUT = 3,
    COMPONENT_BSP = 4,
    COMPONENT_SOUND = 5,
    COMPONENT_RENDER = 6,
    COMPONENT_GUI = 7,
    COMPONENT_MENU = 8,
    COMPONENT_WEAPONS = 9,
    COMPONENT_HUD = 10,
    COMPONENT_GAME = 11,
    COMPONENT_GAME_INPUT = 12
};

class ComponentsManager {
public:
    std::vector<Component *> components;
    std::vector<Object3D *> *sceneObjects;

    static ComponentsManager *get();

    static ComponentsManager *instance;

    void registerComponent(Component *component, std::string label);

    void configureComponents();

    std::vector<Object3D *> *getSceneObjects() const;

    void setSceneObjects(std::vector<Object3D *> *sceneObjects);

    ComponentCamera *
    getComponentCamera() { return static_cast<ComponentCamera *>(components[ComponentID::COMPONENT_CAMERA]); };

    ComponentCollisions *
    getComponentCollisions() { return static_cast<ComponentCollisions *>(components[ComponentID::COMPONENT_COLLISIONS]); };

    ComponentMenu *getComponentMenu() { return static_cast<ComponentMenu *>(components[ComponentID::COMPONENT_MENU]); };

    ComponentWeapons *
    getComponentWeapons() { return static_cast<ComponentWeapons *>(components[ComponentID::COMPONENT_WEAPONS]); };

    ComponentBSP *getComponentBSP() { return static_cast<ComponentBSP *>(components[ComponentID::COMPONENT_BSP]); };

    ComponentHUD *getComponentHUD() { return static_cast<ComponentHUD *>(components[ComponentID::COMPONENT_HUD]); };

    ComponentWindow *
    getComponentWindow() { return static_cast<ComponentWindow *>(components[ComponentID::COMPONENT_WINDOW]); };

    ComponentGUI *getComponentGUI() { return static_cast<ComponentGUI *>(components[ComponentID::COMPONENT_GUI]); };

    ComponentRender *
    getComponentRender() { return static_cast<ComponentRender *>(components[ComponentID::COMPONENT_RENDER]); };

    ComponentInput *
    getComponentInput() { return static_cast<ComponentInput *>(components[ComponentID::COMPONENT_INPUT]); };

    ComponentGame *getComponentGame() { return static_cast<ComponentGame *>(components[ComponentID::COMPONENT_GAME]); };

    ComponentGameInput *
    getComponentGameInput() { return static_cast<ComponentGameInput *>(components[ComponentID::COMPONENT_GAME_INPUT]); };
};


#endif //BRAKEDA3D_COMPONENTSMANAGER_H
