
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
    int currentAnimation = 0;
    std::vector<TextureAnimatedDirectional *> animations;

    Image3DAnimation360(const Vertex3D &position, float width, float height);
    void addAnimationDirectional2D(const std::string&, int frames, int fps, bool zeroDirection, int maxTimes);
    void updateTextureFromCameraAngle(Object3D *, Camera3D *) const;
    void setAnimation(int);
    void updateStep();
    void UpdateTrianglesCoordinates(Camera3D *cam);
    void onUpdate() override;
    void DrawPropertiesGUI() override;
    void updateBillboardSize() const;
    TextureAnimatedDirectional *getCurrentTextureAnimationDirectional() const;
    ObjectType getTypeObject() const override;
    GUIType::Sheet getIcon() override;
    static int getDirectionForAngle(float enemyAngle);
    static Image3DAnimation360* create(
        const Vertex3D &position,
        float width,
        float height,
        const std::string &folderSprite,
        int frames,
        int fps
    );

    friend class Image3DAnimation360Serializer;
    friend class Image3DAnimation360GUI;
};

#endif //BRAKEDA3D_SPRITE_H
