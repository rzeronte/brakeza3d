//
// Created by eduardo on 13/08/23.
//

#ifndef BRAKEZA3D_IMAGE2DANIMATION_H
#define BRAKEZA3D_IMAGE2DANIMATION_H


#include "../Render/TextureAnimated.h"
#include "../3D/Object3D.h"

class Image2DAnimation : public Object3D
{
    int x;
    int y;
    bool removeWhenEnds;
    TextureAnimated *animation;
    Counter *ttl;
public:
    explicit Image2DAnimation(int x, int y, bool removeWhenEnds, TextureAnimated *animation);
    explicit Image2DAnimation(int x, int y, float ttl, TextureAnimated *animation);

    TypeObject getTypeObject() const override;
    GUIType::Sheet getIcon() override;
    void onUpdate() override;
    void updatePosition(int x, int y);
    void DrawPropertiesGUI() override;
    [[nodiscard]] TextureAnimated *getAnimation() const;
    [[nodiscard]] int getX() const;
    [[nodiscard]] int getY() const;
    static Image2DAnimation* create(int x, int y, const std::string& sheet, int w, int h, int frames, int fps);

    friend class Image2DAnimationSerializer;
    friend class Image2DAnimationGUI;
};


#endif //BRAKEZA3D_IMAGE2DANIMATION_H
