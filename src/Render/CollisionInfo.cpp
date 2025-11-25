//
// Created by Eduardo on 06/04/2025.
//

#include "../../include/Render/CollisionInfo.h"

CollisionInfo::CollisionInfo(
    void *with,
    int source,
    int boneIndexMapping
)
:
    with(with),
    source(source),
    boneIndexMapping(boneIndexMapping)
{

}

int CollisionInfo::getSource() const {
    return source;
}

int CollisionInfo::getBoneIndexMapping() const {
    return boneIndexMapping;
}

Object3D *CollisionInfo::getObject() const {
    return (Object3D*) with;
}
