//
// Created by darkhead on 12/1/20.
//

#include "../headers/ComponentsManager.h"

ComponentsManager* ComponentsManager::instance = 0;

ComponentsManager::ComponentsManager() {

}

ComponentsManager* ComponentsManager::get()
{
    if (instance == 0) {
        instance = new ComponentsManager();
    }

    return instance;
}

const std::vector<Component *> &ComponentsManager::getComponents() const {
    return components;
}

void ComponentsManager::registerComponent(Component *component, std::vector<Object3D *> *sceneObjects)
{

    component->setComponents( &components );
    component->setSceneObjects( sceneObjects );
    components.push_back( component );

    std::cout << "Register component ("<< components.size()  << ")" << std::endl;
}

