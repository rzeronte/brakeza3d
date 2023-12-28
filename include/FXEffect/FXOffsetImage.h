//
// Created by eduardo on 14/11/22.
//

#ifndef BRAKEZA3D_FXOFFSETIMAGE_H
#define BRAKEZA3D_FXOFFSETIMAGE_H


#include <CL/cl.h>
#include "../Misc/Image.h"
#include "../OpenGL/FXEffectOpenGL.h"

class FXOffsetImage: public FXEffectOpenGL {
    Image image;
    float alpha;
    int useOffset;

    float offsetX;
    float offsetY;
public:
    explicit FXOffsetImage(const std::string& filename);

    void update(float increaseOffsetX, float increaseOffsetY);

    void executeKernelOpenCL(float increaseOffsetX, float increaseOffsetY);

    void resetOffsets();

    void setUseOffset(bool value);

    [[nodiscard]] bool isUseOffset() const;

    void limitOffset();

    Image &getImage();

    void update() override;

    void postUpdate() override;

    void preUpdate() override;

    void drawImGuiProperties() override;

    cJSON *getJSON() override;

    void setOffsets(float x, float y);
};


#endif //BRAKEZA3D_FXOFFSETIMAGE_H
