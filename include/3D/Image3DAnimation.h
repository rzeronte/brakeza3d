
#ifndef BRAKEDA3D_ANIMATEDSPRITE_H
#define BRAKEDA3D_ANIMATEDSPRITE_H

#include "Object3D.h"
#include "../Render/TextureAnimated.h"
#include "Image3D.h"
#include <vector>

class Image3DAnimation : public Object3D
{
    float width = 0;
    float height = 0;
    int currentAnimationIndex = -1;
    int currentFramesVariableToCreateAnimation = 0;
    int currentWidthVariableToCreateAnimation = 0;
    int currentHeightVariableToCreateAnimation = 0;
    bool autoRemoveAfterAnimation = false;
    bool sharedTextures = false;

    std::vector<TextureAnimated*> animations;
    std::string currentSpriteFileVariableToCreateAnimation;
    Image3D *billboard = nullptr;
public:
    Image3DAnimation(const Vertex3D &position, float w, float h);
    ~Image3DAnimation() override;

    void onUpdate() override;
    void setAutoRemoveAfterAnimation(bool autoRemoveAfterAnimation);
    void LinkTextureIntoAnotherImage3DAnimation(const Image3DAnimation *);
    void CreateAnimation(const std::string& sprite, int w, int h, int numFrames, int fps);
    void setAnimation(int);
    void UpdateTexture();
    void UpdateTrianglesCoordinatesAndTexture();
    void DrawPropertiesGUI() override;
    void UpdateBillboardSize() const;
    void LoadAnimationFiles() const;

    ObjectType getTypeObject() const override                               { return ObjectType::Image3DAnimation; }
    GUIType::Sheet getIcon() override                                       { return IconObject::IMAGE_3D_ANIMATION; }
    [[nodiscard]] TextureAnimated *getCurrentTextureAnimation() const       { return this->animations[currentAnimationIndex]; }
    [[nodiscard]] bool isAutoRemoveAfterAnimation() const                   { return autoRemoveAfterAnimation; }
    [[nodiscard]] std::vector<TextureAnimated *> getAnimations() const      { return animations; }

    friend class Image3DAnimationSerializer;
    friend class Image3DAnimationGUI;
};


#endif //BRAKEDA3D_ANIMATEDSPRITE_H
