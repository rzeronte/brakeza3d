//
// Created by Eduardo on 11/12/2025.
//

#ifndef BRAKEZA3D_JOBREADMESHSHADERS_H
#define BRAKEZA3D_JOBREADMESHSHADERS_H

#include "JobMeshFillBuffers.h"
#include "PendingJob.h"
#include "../Brakeza.h"
#include "../3D/Mesh3D.h"

class JobMesh3DLoadGeometry: public PendingJob
{
    Mesh3D *mesh = nullptr;
    cJSON *json = nullptr;
public:
    JobMesh3DLoadGeometry(Mesh3D *mesh, cJSON *sceneJSON)
    :
        PendingJob([this](){ process(); }, [this]() { callback(); }),
        mesh(mesh),
        json(cJSON_Duplicate(sceneJSON, 1))
    {
    }

    void process()
    {
        Logging::Message("[Pools] JobMesh3DLoadGeometry process");
        mesh->AssimpLoadGeometryFromFile(cJSON_GetObjectItemCaseSensitive(json, "model")->valuestring);
    }

    void callback()
    {
        //ComponentRender::FillOGLBuffers(mesh->getMeshData());
        //mesh->UpdateOGLTextures();

        auto job = std::make_shared<JobMeshFillBuffers>(mesh);
        Brakeza::get()->getPoolManager().getIOPool().enqueueWithMainThreadCallback(job);
    }
};


#endif //BRAKEZA3D_JOBREADMESHSHADERS_H