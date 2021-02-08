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
    BSPEntityIndex = -1;
}

void Mesh3DGhost::integrate() {

}
