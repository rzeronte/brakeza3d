//
// Created by darkhead on 3/4/20.
//

#ifndef BRAKEDA3D_MESH3DANIMATEDCOLLECTION_H
#define BRAKEDA3D_MESH3DANIMATEDCOLLECTION_H


#include <vector>
#include "Mesh3DAnimated.h"

class Mesh3DAnimatedCollection: public Object3D
{
public:
    int currentAnimation = -1;
    std::vector<Mesh3DAnimated*> mesh3Danimated;
    M3 rotationFixed = M3::getMatrixRotationForEulerAngles(180, 0, 0);

    Mesh3DAnimated *getCurrentMesh3DAnimated() const;
    void setAnimation(int index);
    void addAnimation(std::string, std::string modelFilename, float scale, bool remove_at_end);

    void onUpdate();
    std::vector<Mesh3DAnimated *> copyFrom(Mesh3DAnimatedCollection *);
};


#endif //BRAKEDA3D_MESH3DANIMATEDCOLLECTION_H
