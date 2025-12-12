//
// Created by Eduardo on 11/12/2025.
//

#ifndef BRAKEZA3D_JOBMESHFILLBUFFERS_H
#define BRAKEZA3D_JOBMESHFILLBUFFERS_H

#include "PendingJob.h"
#include "../3D/Mesh3D.h"
#include "../Components/ComponentRender.h"

class JobMeshFillBuffers : public PendingJob
{
    Mesh3D *mesh = nullptr;
public:
    JobMeshFillBuffers(Mesh3D *mesh)
    :
        PendingJob([this](){ process(); }, [this]() { callback(); }),
        mesh(mesh)
    {
    }

    void process()
    {
        //Logging::Message("%s ", mesh->getName().c_str());
    }

    void callback()
    {
        mesh->FillBuffers();
        mesh->UpdateOGLTextures();
        Brakeza::get()->addObject3D(mesh, mesh->getName());
        Logging::Message("%s ", mesh->getName().c_str());
    }
};


#endif //BRAKEZA3D_JOBMESHFILLBUFFERS_H