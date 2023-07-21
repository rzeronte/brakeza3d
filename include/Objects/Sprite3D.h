
#ifndef BRAKEDA3D_ANIMATEDSPRITE_H
#define BRAKEDA3D_ANIMATEDSPRITE_H

#include "Object3D.h"
#include "../Components/Camera3D.h"
#include "../EngineSetup.h"
#include "../Misc/Timer.h"
#include "../2D/TextureAnimated.h"
#include "../Render/Billboard.h"
#include "../Misc/Counter.h"
#include "../Render/MeshOpenCLRenderer.h"
#include <vector>

class Sprite3D : public Object3D {
    Billboard *billboard;
private:
    int currentAnimationIndex;
    float step;

    Counter counter;
    std::vector<TextureAnimated*> animations;

    bool autoRemoveAfterAnimation;
    bool sharedTextures;

    MeshOpenCLRenderer *openClRenderer;
public:
    Sprite3D(float width, float height);

    ~Sprite3D() override;

    void onUpdate() override;

    void postUpdate() override;

    void updateStep();

    void setAutoRemoveAfterAnimation(bool autoRemoveAfterAnimation);

    void linkTextureAnimation(Sprite3D *);

    TextureAnimated *getCurrentTextureAnimation();

    void addAnimation(const std::string&, int numFrames, int fps);

    void setAnimation(int);

    void updateTexture();

    void updateTrianglesCoordinatesAndTexture();

    [[nodiscard]] bool isAutoRemoveAfterAnimation() const;

    MeshOpenCLRenderer *getOpenClRenderer() const;
};


#endif //BRAKEDA3D_ANIMATEDSPRITE_H
