#ifndef BRAKEZA3D_JOBREADMESHSHADERS_H
#define BRAKEZA3D_JOBREADMESHSHADERS_H

#include "ThreadJobBase.h"
#include "../Brakeza.h"
#include "../3D/Mesh3DAnimation.h"
#include <string>

#include "../include/Serializers/Mesh3DSerializer.h"

class ThreadJobLoadMesh3DAnimation: public ThreadJobBase
{
    Mesh3DAnimation *mesh = nullptr;
    cJSON *json = nullptr;
public:
    ThreadJobLoadMesh3DAnimation(Mesh3DAnimation *m, cJSON *sceneJSON)
    :
        mesh(m),
        json(cJSON_Duplicate(sceneJSON, 1))
    {
        function = [this](){ fnProcess(); };
        callback = [this](){ fnCallback(); };
    }

    void fnProcess()
    {
        if (!mesh) {
            Logging::Error("[ThreadJobLoadMesh3DAnimation] Mesh pointer is null");
            return;
        }

        Mesh3DAnimationSerializer::ApplyGeometryAnimationFromFile(mesh, json);

        Mesh3DSerializer::ApplyShadersCreation(mesh, json);
        Mesh3DSerializer::ApplyShadersBackground(mesh);

        Logging::Message("[ThreadJobLoadMesh3DAnimation] Process END");
    }

    void fnCallback()
    {
        if (!mesh) {
            Logging::Error("[ThreadJobLoadMesh3DAnimation] Pointer was destroyed before callback");
            return;
        }

        mesh->FillOGLBuffers();
        mesh->UpdateOGLTextures();
        mesh->FillAnimationBoneDataOGLBuffers();
        mesh->UpdateBonesFinalTransformations(0);

        Mesh3DSerializer::ApplyCustomShadersMainThread(mesh);
        Mesh3DSerializer::ApplyCollider(mesh, json);

        Brakeza::get()->AddObject3D(mesh, mesh->getName());

        Logging::Message("[ThreadJobLoadMesh3DAnimation] Callback END");
    }

    ~ThreadJobLoadMesh3DAnimation() {
        if (json) {
            cJSON_Delete(json);
            json = nullptr;
        }
    }
};

#endif //BRAKEZA3D_JOBREADMESHSHADERS_H