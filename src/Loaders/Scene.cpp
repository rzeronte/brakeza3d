//
// Created by darkh on 22/01/2026.
//

#include "../../include/Loaders/Scene.h"
#include "../../include/3D/Object3D.h"
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

void Scene::addObject(Object3D *obj)
{
    objects.push_back(obj);
}

void Scene::removeObject(Object3D *obj)
{
    objects.erase(std::remove(objects.begin(), objects.end(), obj), objects.end());
}
