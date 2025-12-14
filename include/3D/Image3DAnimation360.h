
#ifndef BRAKEDA3D_SPRITE_H
#define BRAKEDA3D_SPRITE_H

#include "Object3D.h"
#include "../Render/TextureAnimatedDirectional.h"
#include "../Components/Camera3D.h"
#include "../Render/Triangle3D.h"
#include "Image3D.h"
#include "../Misc/Counter.h"

class Image3DAnimation360 : public Object3D
{
    Image3D *billboard;
    std::string currentSpriteFileVariableToCreateAnimation;
    int currentFramesVariableToCreateAnimation = 0;

public:
    float width;
    float height;
    Counter *counterAnimations;
    float step = 0;
    int currentIndexAnimation = -1;
    std::vector<TextureAnimatedDirectional *> animations;

    Image3DAnimation360(const Vertex3D &position, float width, float height);

    void onUpdate() override;
    void DrawPropertiesGUI() override;
    void UpdateBillboardSize() const;
    void LoadAnimations() const;
    void UpdateStep();
    void CreateAnimationDirectional2D(const std::string&, int frames, int fps, bool zeroDirection, int maxTimes);
    void UpdateTextureFromCameraAngle(Object3D *, Camera3D *) const;
    void UpdateTrianglesCoordinates(Camera3D *cam);
    void CreateAnimationDirectional2D(TextureAnimatedDirectional *directionalTex, bool zeroDirection);

    void setAnimation(int);

    TextureAnimatedDirectional *getCurrentTextureAnimationDirectional() const   { return this->animations[currentIndexAnimation]; }
    ObjectType getTypeObject() const override                                   { return ObjectType::Image3DAnimation360; }
    GUIType::Sheet getIcon() override                                           { return IconObject::IMAGE_3D_ANIMATION_360; }
    static int getDirectionForAngle(float enemyAngle);


    friend class Image3DAnimation360Serializer;
    friend class Image3DAnimation360GUI;
};

#endif //BRAKEDA3D_SPRITE_H
