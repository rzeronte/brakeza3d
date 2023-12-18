//
// Created by eduardo on 13/08/23.
//

#ifndef BRAKEZA3D_SPRITE2D_H
#define BRAKEZA3D_SPRITE2D_H


#include "TextureAnimated.h"
#include "../Objects/Object3D.h"

class Sprite2D : public Object3D {
    int x;
    int y;
    bool removeWhenEnds;
    TextureAnimated *animation;
    Counter *ttl;

public:
    explicit Sprite2D(int x, int y, bool removeWhenEnds, TextureAnimated *animation);
    explicit Sprite2D(int x, int y, float ttl, TextureAnimated *animation);

    void onUpdate() override;

    [[nodiscard]] TextureAnimated *getAnimation() const;

    void updatePosition(int x, int y);

    const char *getTypeObject() override;

    const char *getTypeIcon() override;
};


#endif //BRAKEZA3D_SPRITE2D_H
