//
// Created by darkhead on 29/12/19.
//

#ifndef BRAKEDA3D_BILLBOARDBODY_H
#define BRAKEDA3D_BILLBOARDBODY_H


#include "../Render/Billboard.h"
#include "Body.h"

class BillboardBody : public Object3D, public Billboard, public Body {

public:
    BillboardBody() = default;

    void integrate() override;

    void onUpdate() override;

    void updateTrianglesCoordinatesAndTexture(Camera3D *cam);

    btRigidBody *makeRigidBody(float, Vertex3D size, btDiscreteDynamicsWorld *);
};


#endif //BRAKEDA3D_BILLBOARDBODY_H
