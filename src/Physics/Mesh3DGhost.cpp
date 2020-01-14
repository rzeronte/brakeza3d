//
// Created by darkhead on 2019-05-31.
//

#include "../../headers/Physics/Mesh3DGhost.h"
#include <btBulletDynamicsCommon.h>
#include "../../headers/Render/Logging.h"
#include "../../headers/Render/Transforms.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "../../headers/Brakeza3D.h"

Mesh3DGhost::Mesh3DGhost()
{
    this->m_ghostObject = new btPairCachingGhostObject();
    BSPEntityIndex = -1;
}

void Mesh3DGhost::makeGhostBody(Camera3D *cam, btDiscreteDynamicsWorld *world, bool useObjectSpace)
{
    this->m_ghostObject = new btPairCachingGhostObject();

    btConvexHullShape* me = new btConvexHullShape();

    for (int i=0; i < this->modelTriangles.size(); i++) {
        this->modelTriangles[i]->updateFullVertexSpaces(cam);
        btVector3 a, b, c;
        // Esto solo lo utilizamos para mayas procedentes de triángulos BSP en crudo.
        if (useObjectSpace) {
            a = btVector3(this->modelTriangles[i]->Ao.x, this->modelTriangles[i]->Ao.y, this->modelTriangles[i]->Ao.z );
            b = btVector3(this->modelTriangles[i]->Bo.x, this->modelTriangles[i]->Bo.y, this->modelTriangles[i]->Bo.z );
            c = btVector3(this->modelTriangles[i]->Co.x, this->modelTriangles[i]->Co.y, this->modelTriangles[i]->Co.z );
        } else {
            a = btVector3(this->modelTriangles[i]->A.x, this->modelTriangles[i]->A.y, this->modelTriangles[i]->A.z );
            b = btVector3(this->modelTriangles[i]->B.x, this->modelTriangles[i]->B.y, this->modelTriangles[i]->B.z );
            c = btVector3(this->modelTriangles[i]->C.x, this->modelTriangles[i]->C.y, this->modelTriangles[i]->C.z );
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

    world->addCollisionObject(m_ghostObject, EngineSetup::collisionGroups::BSPHullTrigger, EngineSetup::collisionGroups::CameraTrigger);
}

bool Mesh3DGhost::CheckGhost(btPairCachingGhostObject* Ghost)
{
    btManifoldArray ManifoldArray;
    btBroadphasePairArray& PairArray = Ghost->getOverlappingPairCache()->getOverlappingPairArray();

    for (int i = 0; i < PairArray.size(); i++) {
        ManifoldArray.clear();

        btBroadphasePair* CollisionPair = Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld()->getPairCache()->findPair(PairArray[i].m_pProxy0, PairArray[i].m_pProxy1);

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