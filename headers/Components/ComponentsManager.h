#ifndef BRAKEDA3D_COMPONENTSMANAGER_H
#define BRAKEDA3D_COMPONENTSMANAGER_H

#include <iostream>
#include "Component.h"
#include "ComponentCamera.h"
#include "ComponentCollisions.h"
#include "../GUI/GUIManager.h"
#include "ComponentBSP.h"
#include "ComponentHUD.h"
#include "ComponentGUI.h"
#include "ComponentRender.h"

class ComponentsManager {
public:
    std::vector<Component *> components;

    ComponentsManager();
    static ComponentsManager* get();
    static ComponentsManager* instance;

    const std::vector<Component *> &getComponents() const;
    void registerComponent(Component *component, std::vector<Object3D *> *sceneObjects);

    ComponentCamera     *getComponentCamera() { return dynamic_cast<ComponentCamera *>(components.at(EngineSetup::ComponentID::COMPONENT_CAMERA)); };
    ComponentCollisions *getComponentCollisions() { return dynamic_cast<ComponentCollisions *>(components.at(EngineSetup::ComponentID::COMPONENT_COLLISIONS)); };
    ComponentMenu       *getComponentMenu() { return dynamic_cast<ComponentMenu *>(components.at(EngineSetup::ComponentID::COMPONENT_MENU)); };
    ComponentWeapons    *getComponentWeapons() { return dynamic_cast<ComponentWeapons *>(components.at(EngineSetup::ComponentID::COMPONENT_WEAPONS)); };
    ComponentBSP        *getComponentBSP() { return dynamic_cast<ComponentBSP *>(components.at(EngineSetup::ComponentID::COMPONENT_BSP)); };
    ComponentHUD        *getComponentHUD() { return dynamic_cast<ComponentHUD *>(components.at(EngineSetup::ComponentID::COMPONENT_HUD)); };
    ComponentWindow     *getComponentWindow() { return dynamic_cast<ComponentWindow *>(components.at(EngineSetup::ComponentID::COMPONENT_WINDOW)); };
    ComponentGUI        *getComponentGUI() { return dynamic_cast<ComponentGUI *>(components.at(EngineSetup::ComponentID::COMPONENT_GUI)); };
    ComponentRender     *getComponentRender() { return dynamic_cast<ComponentRender *>(components.at(EngineSetup::ComponentID::COMPONENT_RENDER)); };
};


#endif //BRAKEDA3D_COMPONENTSMANAGER_H
