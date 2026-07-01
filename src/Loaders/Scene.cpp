//
// Created by darkh on 22/01/2026.
//

#include "../../include/Loaders/Scene.h"
#include "../../include/3D/Object3D.h"
#include "../../include/Components/Components.h"
#include "../../include/OpenGL/Base/ShaderBaseCustom.h"
#include <algorithm>

void Scene::setActive(bool value)
{
    active = value;
    for (auto *obj : objects) {
        obj->setEnabled(value);
        if (value)
            obj->EnableSimulationCollider();
        else
            obj->DisableSimulationCollider();
    }
}

void Scene::setHidden(bool value)
{
    if (hidden == value) return;
    hidden = value;

    // Objects
    if (hidden) {
        savedEnabledStates.clear();
        for (auto* obj : objects) {
            savedEnabledStates[obj] = obj->isEnabled();
            obj->setEnabled(false);
        }
    } else {
        for (auto* obj : objects) {
            auto it = savedEnabledStates.find(obj);
            obj->setEnabled(it != savedEnabledStates.end() ? it->second : true);
        }
        savedEnabledStates.clear();
    }

    // Scene shaders belonging to this scene
    for (auto* shader : Components::get()->Render()->getSceneShaders()) {
        if (shader->getScene() == this) {
            shader->setEnabled(!hidden);
        }
    }
}

void Scene::addObject(Object3D *obj)
{
    objects.push_back(obj);
}

void Scene::removeObject(Object3D *obj)
{
    objects.erase(std::remove(objects.begin(), objects.end(), obj), objects.end());
}
