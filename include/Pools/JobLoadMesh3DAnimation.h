#ifndef BRAKEZA3D_JOBREADMESHSHADERS_H
#define BRAKEZA3D_JOBREADMESHSHADERS_H

#include "PendingJob.h"
#include "../Brakeza.h"
#include "../3D/Mesh3DAnimation.h"
#include <string>

#include "../include/Serializers/Mesh3DSerializer.h"

class JobLoadMesh3DAnimation: public PendingJob
{
    Mesh3DAnimation *mesh = nullptr;
    cJSON *json = nullptr;
public:
    JobLoadMesh3DAnimation(Mesh3DAnimation *m, cJSON *sceneJSON)
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
            Logging::Error("[Pools] Mesh pointer is null");
            return;
        }

        Mesh3DAnimationSerializer::ApplyGeometryAnimationFromFile(mesh, json);

        Mesh3DSerializer::ApplyShadersCreation(mesh, json);
        Mesh3DSerializer::ApplyShadersBackground(mesh);

        Logging::Message("[Pools] JobLoadMesh3D::fnProcess END");
    }

    void fnCallback()
    {
        if (!mesh) {
            Logging::Error("[Pools] Mesh was destroyed before callback");
            return;
        }

        mesh->FillOGLBuffers();
        mesh->UpdateOGLTextures();
        mesh->FillAnimationBoneDataOGLBuffers();
        mesh->UpdateBonesFinalTransformations(0);

        Mesh3DSerializer::ApplyCustomShadersMainThread(mesh);
        Mesh3DSerializer::ApplyCollider(mesh, json);

        Brakeza::get()->addObject3D(mesh, mesh->getName());

        Logging::Message("[Pools] JobLoadMesh3D::fnCallback END");
    }

    ~JobLoadMesh3DAnimation() {
        if (json) {
            cJSON_Delete(json);
            json = nullptr;
        }
    }
};

#endif //BRAKEZA3D_JOBREADMESHSHADERS_H