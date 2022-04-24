#include "../../include/Physics/Ghost.h"
#include "../../include/Brakeza3D.h"

Ghost::Ghost() {
    this->ghostObject = new btPairCachingGhostObject();
}

void Ghost::makeGhostBody(btDiscreteDynamicsWorld *world, Mesh3D *mesh, int collisionGroup, int collisionMask) {
    auto *convexHull = new btConvexHullShape();

    mesh->updateBoundingBox();
    for (auto & modelTriangle : mesh->modelTriangles) {
        btVector3 a, b, c;
        a = btVector3(modelTriangle->A.x, modelTriangle->A.y, modelTriangle->A.z);
        b = btVector3(modelTriangle->B.x, modelTriangle->B.y, modelTriangle->B.z);
        c = btVector3(modelTriangle->C.x, modelTriangle->C.y, modelTriangle->C.z);

        convexHull->addPoint(a);
        convexHull->addPoint(b);
        convexHull->addPoint(c);
    }

    btTransform transformation;
    transformation.setIdentity();
    transformation.setOrigin(btVector3(0, 0, 0));

    ghostObject->setCollisionShape(new btConvexHullShape(*convexHull));
    ghostObject->setWorldTransform(transformation);
    ghostObject->setUserPointer(dynamic_cast<Ghost *> (this));

    world->addCollisionObject(ghostObject, collisionGroup, collisionMask);
}

bool Ghost::CheckGhost(btPairCachingGhostObject *Ghost) {
    btManifoldArray ManifoldArray;
    btBroadphasePairArray &PairArray = Ghost->getOverlappingPairCache()->getOverlappingPairArray();

    for (int i = 0; i < PairArray.size(); i++) {
        ManifoldArray.clear();

        btBroadphasePair *CollisionPair = Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld()->getPairCache()->findPair(
                PairArray[i].m_pProxy0, PairArray[i].m_pProxy1);

        if (!CollisionPair) {
            continue;
        }

        if (CollisionPair->m_algorithm) {
            CollisionPair->m_algorithm->getAllContactManifolds(ManifoldArray);
        }

        for (int j = 0; j < ManifoldArray.size(); j++) {
            for (int p = 0; p < ManifoldArray[j]->getNumContacts(); p++) {
                const btManifoldPoint &Point = ManifoldArray[j]->getContactPoint(p);

                if (Point.getDistance() < 0.0f) {
                    return true;
                }
            }
        }
    }

    return false;
}

btPairCachingGhostObject *Ghost::getGhostObject() const {
     return ghostObject;
}

void Ghost::removeCollisionObject() const {
    ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld()->removeCollisionObject(getGhostObject());
}

Ghost::~Ghost()
{
    delete ghostObject;
}

