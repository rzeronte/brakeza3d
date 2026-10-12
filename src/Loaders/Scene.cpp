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
            obj->DisableSimulationCollider();
        }
    } else {
        for (auto* obj : objects) {
            auto it = savedEnabledStates.find(obj);
            const bool wasEnabled = (it != savedEnabledStates.end() ? it->second : true);
            obj->setEnabled(wasEnabled);
            if (wasEnabled) obj->EnableSimulationCollider();
        }
        savedEnabledStates.clear();
    }

    // Scene shaders belonging to this scene
    for (auto* shader : Components::get()->Render()->getSceneShaders()) {
        if (shader->getScene() != this) continue;
        if (hidden) {
            savedShaderStates[shader] = shader->isEnabled();
            shader->setEnabled(false);
        } else {
            auto it = savedShaderStates.find(shader);
            shader->setEnabled(it != savedShaderStates.end() ? it->second : false);
        }
    }
    if (!hidden) savedShaderStates.clear();
}

void Scene::addObject(Object3D *obj)
{
    objects.push_back(obj);
}

void Scene::removeObject(Object3D *obj)
{
    objects.erase(std::remove(objects.begin(), objects.end(), obj), objects.end());
}

// ── Carpetas ──────────────────────────────────────────────────────────────────

const SceneFolder* Scene::getFolder(int id) const
{
    for (const auto &f : folders) if (f.id == id) return &f;
    return nullptr;
}

int Scene::createFolder(const std::string &folderName, int parent)
{
    if (parent != 0 && getFolder(parent) == nullptr) parent = 0;
    SceneFolder f;
    f.id = nextFolderId++;
    f.name = folderName.empty() ? "New folder" : folderName;
    f.parent = parent;
    folders.push_back(f);
    return f.id;
}

void Scene::renameFolder(int id, const std::string &folderName)
{
    if (folderName.empty()) return;
    for (auto &f : folders) if (f.id == id) f.name = folderName;
}

bool Scene::isFolderInside(int id, int ancestor) const
{
    // Sube por los padres desde id; el límite evita colgarse si un JSON trae un ciclo
    for (int guard = 0; id != 0 && guard < 1024; guard++) {
        if (id == ancestor) return true;
        const SceneFolder *f = getFolder(id);
        if (f == nullptr) return false;
        id = f->parent;
    }
    return false;
}

bool Scene::moveFolder(int id, int newParent)
{
    if (getFolder(id) == nullptr) return false;
    if (newParent != 0 && (getFolder(newParent) == nullptr || isFolderInside(newParent, id))) return false;
    for (auto &f : folders) if (f.id == id) f.parent = newParent;
    return true;
}

void Scene::deleteFolder(int id)
{
    const SceneFolder *f = getFolder(id);
    if (f == nullptr) return;
    const int parent = f->parent;
    for (auto &other : folders) if (other.parent == id) other.parent = parent;
    for (auto *obj : objects) if (obj->getFolderId() == id) obj->setFolderId(parent);
    folders.erase(std::remove_if(folders.begin(), folders.end(),
        [id](const SceneFolder &x) { return x.id == id; }), folders.end());
}

void Scene::setFolders(const std::vector<SceneFolder> &list)
{
    folders = list;
    nextFolderId = 1;
    for (const auto &f : folders) nextFolderId = std::max(nextFolderId, f.id + 1);
}

void Scene::moveObjectToFolder(Object3D *obj, int folderId)
{
    if (obj == nullptr) return;
    if (folderId != 0 && getFolder(folderId) == nullptr) return;
    if (obj->getScene() != this) {
        if (obj->getScene() != nullptr) obj->getScene()->removeObject(obj);
        obj->setScene(this);
        addObject(obj);
    }
    obj->setFolderId(folderId);
}
