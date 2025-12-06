
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

    TypeObject getTypeObject() const override;
    GUIType::Sheet getIcon() override;
    void onUpdate() override;
    void setAutoRemoveAfterAnimation(bool autoRemoveAfterAnimation);
    void LinkTextureIntoAnotherImage3DAnimation(const Image3DAnimation *);
    void addAnimation(const std::string& sprite, int w, int h, int numFrames, int fps);
    void setAnimation(int);
    void updateTexture();
    void updateTrianglesCoordinatesAndTexture();
    void DrawPropertiesGUI() override;
    void updateBillboardSize() const;
    [[nodiscard]] TextureAnimated *getCurrentTextureAnimation() const;
    [[nodiscard]] bool isAutoRemoveAfterAnimation() const;
    static Image3DAnimation* create(
        const Vertex3D &position,
        float width,
        float height,
        const std::string &imageFile,
        int spriteWidth,
        int spriteHeight,
        int frames,
        int fps
    );

    friend class Image3DAnimationSerializer;
    friend class Image3DAnimationGUI;
};


#endif //BRAKEDA3D_ANIMATEDSPRITE_H
