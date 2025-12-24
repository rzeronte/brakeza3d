#ifndef BRAKEZA3D_THREADJOBLOADMESH3DANIMATION_H
#define BRAKEZA3D_THREADJOBLOADMESH3DANIMATION_H

#include "ThreadJobBase.h"
#include "../Brakeza.h"
#include "../3D/Mesh3DAnimation.h"
#include <string>
#include "../include/Serializers/Mesh3DSerializer.h"
#include "../include/Serializers/Mesh3DAnimationSerializer.h"

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
            LOG_ERROR("[ThreadJobLoadMesh3DAnimation] Mesh pointer is null");
            return;
        }

        Mesh3DAnimationSerializer::ApplyGeometryAnimationFromFile(mesh, json);

        Mesh3DSerializer::ApplyShadersCreation(mesh, json);
        Mesh3DSerializer::ApplyShadersBackground(mesh);

        LOG_MESSAGE("[ThreadJobLoadMesh3DAnimation] Process END");
    }

    void fnCallback()
    {
        if (!mesh) {
            LOG_ERROR("[ThreadJobLoadMesh3DAnimation] Pointer was destroyed before callback");
            return;
        }

        mesh->FillOGLBuffers();
        mesh->FillAnimationBoneDataOGLBuffers();
        mesh->UpdateBonesFinalTransformations(0);

        Mesh3DSerializer::ApplyCustomShadersMainThread(mesh);
        Mesh3DSerializer::ApplyCollider(mesh, json);

        Brakeza::get()->AddObject3D(mesh, mesh->getName());

        LOG_MESSAGE("[ThreadJobLoadMesh3DAnimation] Callback END");
    }

    ~ThreadJobLoadMesh3DAnimation() {
        if (json) {
            cJSON_Delete(json);
            json = nullptr;
        }
    }
};

#endif