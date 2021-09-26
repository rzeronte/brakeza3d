//
// Created by darkhead on 8/1/20.
//

#include "../../headers/Components/Component.h"

Component::Component() : SETUP(EngineSetup::getInstance()), BUFFERS(EngineBuffers::getInstance()) {
}

std::vector<Component *> *Component::getComponents() const {
    return components;
}

void Component::setComponents(std::vector<Component *> *components) {
    Component::components = components;
}

Component *Component::getComponentById(int id) {
    return this->components->at(id);
}

void Component::setSceneObjects(std::vector<Object3D *> *sceneObjects) {
    Component::sceneObjects = sceneObjects;
}

std::vector<Object3D *> *Component::getSceneObjects() const {
    return sceneObjects;
}

int Component::getId() const {
    return id;
}

void Component::setId(int id) {
    Component::id = id;
}

const std::string &Component::getLabel() const {
    return label;
}

void Component::setLabel(const std::string &label) {
    Component::label = label;
}
