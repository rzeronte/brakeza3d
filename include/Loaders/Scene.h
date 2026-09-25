//
// Created by darkh on 22/01/2026.
//

#ifndef BRAKEZA3D_SCENE_H
#define BRAKEZA3D_SCENE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>

#include "SceneChecker.h"

class Object3D;
class ShaderBaseCustom;

class Scene
{
public:
    explicit Scene(const std::string &file_path)
        : filePath(file_path)
        , name(std::filesystem::path(file_path).stem().string())
    {
    }

    void setActive(bool value);
    void setHidden(bool value);
    void addObject(Object3D *obj);
    void removeObject(Object3D *obj);

    [[nodiscard]] std::string getFilePath() const           { return filePath; }
    [[nodiscard]] std::string getName() const               { return name; }
    [[nodiscard]] bool isActive() const                     { return active; }
    [[nodiscard]] bool isHidden() const                     { return hidden; }
    [[nodiscard]] SceneChecker getChecker() const           { return checker; }
    [[nodiscard]] std::vector<Object3D*>& getObjects()      { return objects; }

    // Deferred-delete: mientras haya ThreadJob (ThreadJobLoadObject/ReadSceneScript/
    // ReadSceneShaders) todavía en vuelo con un puntero crudo a esta Scene, no se puede
    // liberar -- su fnCallback() corre en un salto async posterior y haría use-after-free
    // sobre `scene` si ya se ha borrado (ver ComponentScripting::retireScene). Solo se
    // tocan desde el hilo principal (constructor/destructor del job, ambos corren ahí --
    // ver ThreadPool::enqueueWithMainThreadCallback), así que no hace falta atómico.
    void retainForJob()                                     { ++pendingJobRefCount; }
    void releaseFromJob()                                   { --pendingJobRefCount; }
    [[nodiscard]] bool hasPendingJobs() const                { return pendingJobRefCount > 0; }

    // Object3D::~Object3D() (src/3D/Object3D.cpp) llama a scene->removeObject(this) si el
    // objeto tenía una escena asignada -- y ese delete va diferido un frame completo
    // (ComponentRender::DeleteRemovedObjects: marcar removed=true en el frame N, delete real
    // en el frame N+1/N+2). Si esta Scene ya se borró para entonces (p.ej. porque
    // hasPendingJobs() dio false justo antes de que esos objetos hubieran terminado de
    // destruirse), ese removeObject() es un use-after-free sobre la propia Scene. No basta
    // con vigilar los ThreadJob -- también hay que esperar a que `objects` se quede vacío.
    [[nodiscard]] bool hasLiveObjects() const                { return !objects.empty(); }
    [[nodiscard]] bool canBeDeletedNow() const                { return !hasPendingJobs() && !hasLiveObjects(); }

private:
    SceneChecker checker;
    std::string filePath;
    std::string name;
    bool active = true;
    bool hidden = false;
    int pendingJobRefCount = 0;
    std::unordered_map<Object3D*, bool>        savedEnabledStates;
    std::unordered_map<ShaderBaseCustom*, bool> savedShaderStates;
    std::vector<Object3D*> objects;
};

#endif //BRAKEZA3D_SCENE_H
