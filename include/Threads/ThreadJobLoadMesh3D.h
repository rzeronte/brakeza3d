#ifndef BRAKEZA3D_JOBREADMESHSHADERS_H
#define BRAKEZA3D_JOBREADMESHSHADERS_H

#include "ThreadJobBase.h"
#include "../Brakeza.h"
#include "../3D/Mesh3D.h"
#include <string>

#include "../include/Serializers/Mesh3DSerializer.h"

class ThreadJobLoadMesh3D: public ThreadJobBase
{
    Mesh3D *mesh = nullptr;
    cJSON *json = nullptr;
public:

    ThreadJobLoadMesh3D(Mesh3D *mesh, cJSON *sceneJSON)
    :
        mesh(mesh),
        json(cJSON_Duplicate(sceneJSON, 1))
    {
        function = [this](){ fnProcess(); };
        callback = [this](){ fnCallback(); };
    }

    void fnProcess()
    {
        if (!mesh) {
            Logging::Error("[ThreadJobLoadMesh3D] Pointer object is empty!");
            return;
        }

        Mesh3DSerializer::ApplyGeometryFromFile(mesh, json);
        Mesh3DSerializer::ApplyShadersCreation(mesh, json);
        Mesh3DSerializer::ApplyShadersBackground(mesh);

        Logging::Message("[ThreadJobLoadMesh3D] Process END");
    }

    void fnCallback()
    {
        if (!mesh) {
            Logging::Error("[ThreadJobLoadMesh3D] Pointer was destroyed before callback");
            return;
        }

        mesh->FillOGLBuffers();

        Mesh3DSerializer::ApplyCustomShadersMainThread(mesh);
        Mesh3DSerializer::ApplyCollider(mesh, json);

        Brakeza::get()->AddObject3D(mesh, mesh->getName());

        Logging::Message("[ThreadJobLoadMesh3D] Callback END");
    }

    ~ThreadJobLoadMesh3D()
    {
        if (json) {
            cJSON_Delete(json);
            json = nullptr;
        }
    }
};

#endif //BRAKEZA3D_JOBREADMESHSHADERS_H