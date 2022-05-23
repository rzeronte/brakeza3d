//
// Created by eduardo on 10/05/22.
//

#ifndef BRAKEDA3D_SHADERBACKGROUNDGAME_H
#define BRAKEDA3D_SHADERBACKGROUNDGAME_H


#include "../../../include/Render/Shader.h"
#include "../../../include/Misc/Image.h"
#include "../../../include/Objects/Point2D.h"
#include "../../../include/Objects/Vector2D.h"

class ShaderBackgroundGame: public Shader {
    Image *channel1;
    uint32_t *palette;
public:
    ShaderBackgroundGame();

    void update() override;

    ~ShaderBackgroundGame() override;

    float plot(Vector2D st, float thickness);

    float plot(Vector2D st, float pct, float thickness);

    void demo01();

    void demo02();

    void demo03();

    void demo04();

    void makeColorPalette();
};


#endif //BRAKEDA3D_SHADERBACKGROUNDGAME_H
