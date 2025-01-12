//
// Created by Eduardo on 12/01/2025.
//

#ifndef BRAKEZA3D_IMAGE2D_H
#define BRAKEZA3D_IMAGE2D_H


#include "../Misc/Image.h"
#include "../Objects/Object3D.h"

class Image2D : public Object3D {
    int x;
    int y;
    Image *image;
public:
    explicit Image2D(int x, int y, Image * image);

    void onUpdate() override;

    void updatePosition(int x, int y);

    const char *getTypeObject() override;

    const char *getTypeIcon() override;

    void drawImGuiProperties() override;

    cJSON *getJSON() override;

    static void createFromJSON(cJSON *object);

    static void setPropertiesFromJSON(cJSON *object, Image2D *o);

};


#endif //BRAKEZA3D_IMAGE2D_H
