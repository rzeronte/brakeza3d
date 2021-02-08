//
// Created by darkhead on 8/2/21.
//

#include "Ghost.h"
#include "../Brakeza3D.h"

Ghost::Ghost()
{
    this->ghostObject = new btPairCachingGhostObject();
}

void Ghost::makeGhostBody(Camera3D *cam, btDiscreteDynamicsWorld *world, bool useObjectSpace, Mesh3D* mesh)
{
    this->ghostObject = new btPairCachingGhostObject();

    auto *me = new btConvexHullShape();

    for (int i=0; i < mesh->modelTriangles.size(); i++) {
        mesh->modelTriangles[i]->updateFullVertexSpaces(cam);
        btVector3 a, b, c;
        // Esto solo lo utilizamos para mayas procedentes de triángulos BSP en crudo.
        if (useObjectSpace) {
            a = btVector3(mesh->modelTriangles[i]->Ao.x, mesh->modelTriangles[i]->Ao.y, mesh->modelTriangles[i]->Ao.z );
            b = btVector3(mesh->modelTriangles[i]->Bo.x, mesh->modelTriangles[i]->Bo.y, mesh->modelTriangles[i]->Bo.z );
            c = btVector3(mesh->modelTriangles[i]->Co.x, mesh->modelTriangles[i]->Co.y, mesh->modelTriangles[i]->Co.z );
        } else {
            a = btVector3(mesh->modelTriangles[i]->A.x, mesh->modelTriangles[i]->A.y, mesh->modelTriangles[i]->A.z );
            b = btVector3(mesh->modelTriangles[i]->B.x, mesh->modelTriangles[i]->B.y, mesh->modelTriangles[i]->B.z );
            c = btVector3(mesh->modelTriangles[i]->C.x, mesh->modelTriangles[i]->C.y, mesh->modelTriangles[i]->C.z );
        }
        me->addPoint(a);
        me->addPoint(b);
        me->addPoint(c);
    }

    btTransform trans;
    trans.setIdentity();

    trans.setOrigin(btVector3(0, 0, 0));
    btCollisionShape* shape = new btConvexHullShape(*me);

    ghostObject->setCollisionShape(shape);
    ghostObject->setWorldTransform(trans);

    this->ghostObject->setUserPointer(this);

    world->addCollisionObject(ghostObject, EngineSetup::collisionGroups::BSPHullTrigger, EngineSetup::collisionGroups::CameraTrigger);
}

bool Ghost::CheckGhost(btPairCachingGhostObject* Ghost)
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

bool Ghost::isGhostEnabled()
{
    return this->ghostEnabled;
}

void Ghost::setGhostEnabled(bool enabled)
{
    this->ghostEnabled = enabled;
}
