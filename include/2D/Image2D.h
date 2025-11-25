//
// Created by Eduardo on 12/01/2025.
//

#ifndef BRAKEZA3D_IMAGE2D_H
#define BRAKEZA3D_IMAGE2D_H


#include "../Render/Image.h"
#include "../3D/Object3D.h"

class Image2D : public Object3D
{
    int x = 0;
    int y = 0;
    Image *image = nullptr;
public:
    explicit Image2D(int x, int y, Image * image);
    void onUpdate() override;
    void updatePosition(int x, int y);
    void drawImGuiProperties() override;
    const char *getTypeObject() override;
    const char *getTypeIcon() override;
    static Image2D* create(int x, int y, const std::string& imageFile);

    friend class Image2DSerializer;
    friend class Image2DGUI;
};


#endif //BRAKEZA3D_IMAGE2D_H
