#include "../include/ComponentsManager.h"

ComponentsManager *ComponentsManager::instance = nullptr;

ComponentsManager *ComponentsManager::get() {
    if (instance == nullptr) {
        instance = new ComponentsManager();
    }

    return instance;
}

void ComponentsManager::registerComponent(Component *component, const std::string& label) {
    component->setComponents(&components);
    component->setId(components.size());
    component->setLabel(label);
    component->setSceneObjects(sceneObjects);

    components.push_back(component);

    std::cout << "Register component (" << component->getId() << ")" << std::endl;
}

void ComponentsManager::configureComponents() {
    getComponentBSP()->setCamera(getComponentCamera()->getCamera());

    getComponentCollisions()->setBSPMap(getComponentBSP()->getBSP());
    getComponentCollisions()->setCamera(getComponentCamera()->getCamera());
    getComponentCollisions()->setVisibleTriangles(getComponentRender()->getVisibleTriangles());

    getComponentBSP()->getBSP()->setFrameTriangles(&getComponentRender()->getFrameTriangles());

    getComponentGUI()->setRenderer(getComponentWindow()->renderer);
    getComponentGUI()->setWindow(getComponentWindow()->window);
    getComponentGUI()->setContextOpenGl(&getComponentWindow()->contextOpenGL);
}

std::vector<Object3D *> *ComponentsManager::getSceneObjects() const {
    return sceneObjects;
}

void ComponentsManager::setSceneObjects(std::vector<Object3D *> *sceneObjects) {
    ComponentsManager::sceneObjects = sceneObjects;
}


