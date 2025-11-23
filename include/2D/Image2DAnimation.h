//
// Created by eduardo on 13/08/23.
//

#ifndef BRAKEZA3D_IMAGE2DANIMATION_H
#define BRAKEZA3D_IMAGE2DANIMATION_H


#include "TextureAnimated.h"
#include "../Objects/Object3D.h"

class Image2DAnimation : public Object3D {
    int x;
    int y;
    bool removeWhenEnds;
    TextureAnimated *animation;
    Counter *ttl;
public:
    explicit Image2DAnimation(int x, int y, bool removeWhenEnds, TextureAnimated *animation);
    explicit Image2DAnimation(int x, int y, float ttl, TextureAnimated *animation);

    void onUpdate() override;

    [[nodiscard]] TextureAnimated *getAnimation() const;

    void updatePosition(int x, int y);

    const char *getTypeObject() override;

    const char *getTypeIcon() override;

    void drawImGuiProperties() override;

    cJSON *getJSON(Image2DAnimation* object);

    static void createFromJSON(cJSON *object);

    static void setPropertiesFromJSON(cJSON *object, Image2DAnimation *o);

    static Image2DAnimation* create(int x, int y, const std::string& imageFile, int w, int h, int frames, int fps);

};


#endif //BRAKEZA3D_IMAGE2DANIMATION_H
