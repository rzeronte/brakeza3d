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

class ComponentsManager {
public:
    std::vector<Component *> components;

    ComponentsManager();
    static ComponentsManager* get();
    static ComponentsManager* instance;

    const std::vector<Component *> &getComponents() const;
    void registerComponent(Component *component, std::vector<Object3D *> *sceneObjects);
    void configureComponents();

    ComponentCamera     *getComponentCamera() { return dynamic_cast<ComponentCamera *>(components.at(EngineSetup::ComponentID::COMPONENT_CAMERA)); };
    ComponentCollisions *getComponentCollisions() { return dynamic_cast<ComponentCollisions *>(components.at(EngineSetup::ComponentID::COMPONENT_COLLISIONS)); };
    ComponentMenu       *getComponentMenu() { return dynamic_cast<ComponentMenu *>(components.at(EngineSetup::ComponentID::COMPONENT_MENU)); };
    ComponentWeapons    *getComponentWeapons() { return dynamic_cast<ComponentWeapons *>(components.at(EngineSetup::ComponentID::COMPONENT_WEAPONS)); };
    ComponentBSP        *getComponentBSP() { return dynamic_cast<ComponentBSP *>(components.at(EngineSetup::ComponentID::COMPONENT_BSP)); };
    ComponentHUD        *getComponentHUD() { return dynamic_cast<ComponentHUD *>(components.at(EngineSetup::ComponentID::COMPONENT_HUD)); };
    ComponentWindow     *getComponentWindow() { return dynamic_cast<ComponentWindow *>(components.at(EngineSetup::ComponentID::COMPONENT_WINDOW)); };
    ComponentGUI        *getComponentGUI() { return dynamic_cast<ComponentGUI *>(components.at(EngineSetup::ComponentID::COMPONENT_GUI)); };
    ComponentRender     *getComponentRender() { return dynamic_cast<ComponentRender *>(components.at(EngineSetup::ComponentID::COMPONENT_RENDER)); };
    ComponentInput      *getComponentInput() { return dynamic_cast<ComponentInput *>(components.at(EngineSetup::ComponentID::COMPONENT_INPUT)); };
    ComponentGame       *getComponentGame() { return dynamic_cast<ComponentGame *>(components.at(EngineSetup::ComponentID::COMPONENT_GAME)); };
    ComponentGameInput  *getComponentGameInput() { return dynamic_cast<ComponentGameInput *>(components.at(EngineSetup::ComponentID::COMPONENT_GAMEI_NPUT)); };
};


#endif //BRAKEDA3D_COMPONENTSMANAGER_H
