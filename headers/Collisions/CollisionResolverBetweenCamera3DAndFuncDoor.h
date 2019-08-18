
#ifndef BRAKEDA3D_COLLISIONRESOLVERBETWEENCAMERA3DANDFUNCDOOR_H
#define BRAKEDA3D_COLLISIONRESOLVERBETWEENCAMERA3DANDFUNCDOOR_H

#include "CollisionResolver.h"
#include "../Render/Logging.h"

class CollisionResolverBetweenCamera3DAndFuncDoor : public CollisionResolver {
public:
    Mesh3DBody *mesh;
    Camera3D *camera;

    CollisionResolverBetweenCamera3DAndFuncDoor(Object3D *objA, Object3D *objB, BSPMap *bspMap) : CollisionResolver(objA, objB, bspMap)
    {
        this->mesh   = this->getMesh3D();
        this->camera = this->getCamera();
    }

    void dispatch()
    {
        Logging::getInstance()->Log("CollisionResolverBetweenCamera3DAndFuncDoor");
    }

    Camera3D* getCamera()
    {
        if (objA->getLabel() == EngineSetup::getInstance()->cameraNameIdentifier) {
            Camera3D *camera = dynamic_cast<Camera3D*> (this->objA);
            return camera;
        }

        if (objB->getLabel() == EngineSetup::getInstance()->cameraNameIdentifier) {
            Camera3D *camera = dynamic_cast<Camera3D*> (this->objB);
            return camera;
        }
    }

    Mesh3DBody* getMesh3D()
    {
        Mesh3DBody *meshA = dynamic_cast<Mesh3DBody*> (this->objA);
        if (meshA != NULL) {
            return meshA;
        }

        Mesh3DBody *meshB = dynamic_cast<Mesh3DBody*> (this->objB);
        if (meshB != NULL) {
            return meshB;
        }
    }
};

#endif //BRAKEDA3D_COLLISIONRESOLVERBETWEENCAMERA3DANDFUNCDOOR_H
