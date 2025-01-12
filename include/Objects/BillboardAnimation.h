
#ifndef BRAKEDA3D_ANIMATEDSPRITE_H
#define BRAKEDA3D_ANIMATEDSPRITE_H

#include "Object3D.h"
#include "../Components/Camera3D.h"
#include "../EngineSetup.h"
#include "../Misc/Timer.h"
#include "../2D/TextureAnimated.h"
#include "../Render/Billboard.h"
#include "../Misc/Counter.h"
#include <vector>

class BillboardAnimation : public Object3D {
    Billboard *billboard;
private:
    float width;
    float height;
    int currentAnimationIndex;

    std::vector<TextureAnimated*> animations;

    bool autoRemoveAfterAnimation;
    bool sharedTextures;

    std::string currentSpriteFileVariableToCreateAnimation;
    int currentFramesVariableToCreateAnimation = 0;
    int currentWidthVariableToCreateAnimation = 0;
    int currentHeightVariableToCreateAnimation = 0;
public:
    BillboardAnimation(float width, float height);

    ~BillboardAnimation() override;

    void onUpdate() override;

    void setAutoRemoveAfterAnimation(bool autoRemoveAfterAnimation);

    void linkTextureAnimation(BillboardAnimation *);

    TextureAnimated *getCurrentTextureAnimation();

    void addAnimation(const std::string& spriteSheetFile, int spriteWidth, int spriteHeight, int numFrames, int fps);

    void setAnimation(int);

    void updateTexture();

    void updateTrianglesCoordinatesAndTexture();

    [[nodiscard]] bool isAutoRemoveAfterAnimation() const;

    const char *getTypeObject() override;

    const char *getTypeIcon() override;

    void drawImGuiProperties() override;

    cJSON *getJSON() override;

    static void createFromJSON(cJSON *object);

    static void setPropertiesFromJSON(cJSON *object, BillboardAnimation *o);

    void updateBillboardSize();
};


#endif //BRAKEDA3D_ANIMATEDSPRITE_H
