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

    Mesh3DAnimated *getCurrentMesh3DAnimated() const;
    void setCurrentAnimation(int index);
    void addAnimation(std::string, std::string modelFilename, float scale);

    void onUpdate();
};


#endif //BRAKEDA3D_MESH3DANIMATEDCOLLECTION_H
