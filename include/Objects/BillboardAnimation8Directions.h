
#ifndef BRAKEDA3D_SPRITE_H
#define BRAKEDA3D_SPRITE_H

#include "Object3D.h"
#include "../Misc/Timer.h"
#include "../2D/TextureAnimatedDirectional.h"
#include "../Components/Camera3D.h"
#include "Triangle3D.h"
#include "../Render/Billboard.h"
#include "../Misc/Counter.h"

class BillboardAnimation8Directions : public Object3D {
    Billboard *billboard;
    std::string currentSpriteFileVariableToCreateAnimation;
    int currentFramesVariableToCreateAnimation = 0;
public:
    float width;
    float height;

    Counter *counterAnimations;
    float step = 0;

    int currentAnimation = 0;

    std::vector<TextureAnimatedDirectional *> animations;

    BillboardAnimation8Directions(float width, float height);

    [[nodiscard]] Billboard *getBillboard() const;

    void addAnimationDirectional2D(const std::string&, int frames, int fps, bool zeroDirection, int maxTimes);

    void updateTextureFromCameraAngle(Object3D *, Camera3D *);

    void setAnimation(int);

    static int getDirectionForAngle(float enemyAngle);

    void updateStep();

    void updateTrianglesCoordinates(Camera3D *cam);

    TextureAnimatedDirectional *getCurrentTextureAnimationDirectional();

    void onUpdate() override;

    void drawImGuiProperties() override;

    const char *getTypeObject() override;

    const char *getTypeIcon() override;

    cJSON *getJSON() override;

    static void createFromJSON(cJSON *object);

    static void setPropertiesFromJSON(cJSON *object, BillboardAnimation8Directions *o);

    void updateBillboardSize();
};


#endif //BRAKEDA3D_SPRITE_H
