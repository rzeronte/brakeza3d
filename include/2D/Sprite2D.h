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

public:
    explicit Sprite2D(int x, int y, bool removeWhenEnds, TextureAnimated *animation);

    void onUpdate() override;

    [[nodiscard]] TextureAnimated *getAnimation() const;


    void updatePosition(int x, int y);
};


#endif //BRAKEZA3D_SPRITE2D_H
