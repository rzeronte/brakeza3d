//
// Created by darkhead on 2019-05-31.
//

#include "../../headers/Objects/Mesh3DGhost.h"
#include <btBulletDynamicsCommon.h>
#include "../../headers/Render/Logging.h"
#include "../../headers/Render/Transforms.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "../../headers/Render/Engine.h"

extern Engine *brakeza3D;

Mesh3DGhost::Mesh3DGhost()
{
    this->m_ghostObject = new btPairCachingGhostObject();
}

void Mesh3DGhost::makeGhostBody(Camera3D *cam, btDiscreteDynamicsWorld *world, bool useObjectSpace)
{
    this->m_ghostObject = new btPairCachingGhostObject();

    btConvexHullShape* me = new btConvexHullShape();

    for (int i=0; i < this->n_triangles; i++) {
        this->model_triangles[i].updateVertexSpaces(cam);
        btVector3 a, b, c;
        // Esto solo lo utilizamos para mayas procedentes de triángulos BSP en crudo.
        if (useObjectSpace) {
            a = btVector3( this->model_triangles[i].Ao.x, this->model_triangles[i].Ao.y, this->model_triangles[i].Ao.z );
            b = btVector3( this->model_triangles[i].Bo.x, this->model_triangles[i].Bo.y, this->model_triangles[i].Bo.z );
            c = btVector3( this->model_triangles[i].Co.x, this->model_triangles[i].Co.y, this->model_triangles[i].Co.z );
        } else {
            a = btVector3( this->model_triangles[i].A.x, this->model_triangles[i].A.y, this->model_triangles[i].A.z );
            b = btVector3( this->model_triangles[i].B.x, this->model_triangles[i].B.y, this->model_triangles[i].B.z );
            c = btVector3( this->model_triangles[i].C.x, this->model_triangles[i].C.y, this->model_triangles[i].C.z );
        }
        me->addPoint(a);
        me->addPoint(b);
        me->addPoint(c);
    }

    btTransform trans;
    trans.setIdentity();

    trans.setOrigin(btVector3(0, 0, 0));
    btCollisionShape* shape = new btConvexHullShape(*me);

    m_ghostObject->setCollisionShape(shape);
    m_ghostObject->setWorldTransform(trans);

    this->m_ghostObject->setUserPointer(this);

    world->addCollisionObject(m_ghostObject, collisionGroups::BSPHullTrigger, collisionGroups::CameraTrigger);
}

bool Mesh3DGhost::CheckGhost(btPairCachingGhostObject* Ghost)
{
    btManifoldArray ManifoldArray;
    btBroadphasePairArray& PairArray = Ghost->getOverlappingPairCache()->getOverlappingPairArray();

    for (int i = 0; i < PairArray.size(); i++) {
        ManifoldArray.clear();

        btBroadphasePair* CollisionPair = brakeza3D->dynamicsWorld->getPairCache()->findPair(PairArray[i].m_pProxy0, PairArray[i].m_pProxy1);

        if (!CollisionPair) {
            continue;
        }

        if (CollisionPair->m_algorithm) {
            CollisionPair->m_algorithm->getAllContactManifolds(ManifoldArray);
        }

        for (int j = 0; j < ManifoldArray.size(); j++) {
            for (int p = 0; p < ManifoldArray[j]->getNumContacts(); p++) {
                const btManifoldPoint& Point = ManifoldArray[j]->getContactPoint(p);

                if (Point.getDistance() < 0.0f) {
                    return true;
                }
            }
        }
    }

    return false;
}