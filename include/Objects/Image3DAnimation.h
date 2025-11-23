
#ifndef BRAKEDA3D_ANIMATEDSPRITE_H
#define BRAKEDA3D_ANIMATEDSPRITE_H

#include "Object3D.h"
#include "../Components/Camera3D.h"
#include "../2D/TextureAnimated.h"
#include "Image3D.h"
#include <vector>

class Image3DAnimation : public Object3D
{
    float width;
    float height;
    int currentAnimationIndex;
    int currentFramesVariableToCreateAnimation = 0;
    int currentWidthVariableToCreateAnimation = 0;
    int currentHeightVariableToCreateAnimation = 0;
    bool autoRemoveAfterAnimation;
    bool sharedTextures;

    std::vector<TextureAnimated*> animations;
    std::string currentSpriteFileVariableToCreateAnimation;
    Image3D *billboard;
public:
    Image3DAnimation(Vertex3D &position, float w, float h);

    ~Image3DAnimation() override;

    const char *getTypeObject() override;
    const char *getTypeIcon() override;
    void onUpdate() override;
    void setAutoRemoveAfterAnimation(bool autoRemoveAfterAnimation);
    void linkTextureAnimation(Image3DAnimation *);
    void addAnimation(const std::string& sprite, int w, int h, int numFrames, int fps);
    void setAnimation(int);
    void updateTexture();
    void updateTrianglesCoordinatesAndTexture();
    void drawImGuiProperties() override;
    void updateBillboardSize() const;
    TextureAnimated *getCurrentTextureAnimation() const;
    cJSON *getJSON(Image3DAnimation *object);
    [[nodiscard]] bool isAutoRemoveAfterAnimation() const;
    static void createFromJSON(cJSON *object);
    static void setPropertiesFromJSON(cJSON *object, Image3DAnimation *o);
    static Image3DAnimation* create(
        Vertex3D position,
        float width,
        float height,
        const std::string &imageFile,
        int spriteWidth,
        int spriteHeight,
        int frames,
        int fps
    );
};


#endif //BRAKEDA3D_ANIMATEDSPRITE_H
