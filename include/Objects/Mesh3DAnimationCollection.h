#ifndef BRAKEDA3D_MESH3DANIMATEDCOLLECTION_H
#define BRAKEDA3D_MESH3DANIMATEDCOLLECTION_H


#include <vector>
#include "Mesh3DAnimation.h"

class Mesh3DAnimationCollection : public Object3D {
public:
    int currentAnimation = -1;
    std::vector<Mesh3DAnimation *> mesh3Danimated;
    M3 rotationFixed = M3::getMatrixRotationForEulerAngles(180, 0, 0);

    Mesh3DAnimation *getCurrentMesh3DAnimated() const;

    void setAnimation(int index);

    void addAnimation(const std::string&, const std::string& modelFilename, float scale, bool remove_at_end);

    void onUpdate();

    void onUpdateCurrentMesh3D();

    const M3 &getRotationFixed() const;

    void setRotationFixed(const M3 &rotationFixed);
};


#endif //BRAKEDA3D_MESH3DANIMATEDCOLLECTION_H
