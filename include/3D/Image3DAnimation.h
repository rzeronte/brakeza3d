
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
    // NOTA: el billboard facing tiene que aplicarse sobre `this` (Image3DAnimation), no sobre
    // `billboard` -- el render en onUpdate() usa this->getModelMatrix() y solo toma de
    // `billboard` los buffers de vértices/UV/normales planos, así que rotar `billboard` (lo que
    // hacía el viejo setTowardsCamera, delegando en Image3D::setTowardsCamera) no tenía ningún
    // efecto visual: billboard->onUpdate() (el único sitio que consulta ese flag) nunca se
    // llama. Ver feedback "el sprite se ve de canto en vista cenital" (2026-08-21).
    bool towardsCamera = false;
    // "foreground" (default, comportamiento de siempre: capa 2D plana sin profundidad, dibujada
    // encima de TODA la escena 3D sin importar posición real) o "scene" (comparte el depth buffer
    // real del G-Buffer -- se oculta correctamente detrás de geometría opaca). Ver setRenderFB().
    std::string renderFB = "foreground";
public:
    Image3DAnimation(const Vertex3D &position, float w, float h);
    ~Image3DAnimation() override;

    void onUpdate() override;
    void postUpdate() override;
    void setAutoRemoveAfterAnimation(bool autoRemoveAfterAnimation);
    void LinkTextureIntoAnotherImage3DAnimation(const Image3DAnimation *);
    void CreateAnimation(const std::string& sprite, int w, int h, int numFrames, int fps);
    void CreateAnimationFromCache(const std::string& sprite, int w, int h, int numFrames, int fps);
    void setAnimation(int);
    void UpdateTexture();
    void UpdateTrianglesCoordinatesAndTexture();
    void DrawPropertiesGUI() override;
    void UpdateBillboardSize() const;
    void LoadAnimationFiles() const;
    void setTowardsCamera(bool value);
    void setSize(float w, float h);
    // "foreground" (por defecto, sin test de profundidad, siempre encima) o "scene" (test de
    // profundidad contra la geometría 3D real -- para efectos que deben poder quedar ocultos
    // detrás de vehículos/edificios, p.ej. el charco de sangre). No escribe profundidad propia en
    // ningún caso, para que varios sprites semitransparentes solapados no se tapen entre sí.
    void setRenderFB(const std::string &fb);

    ObjectType getTypeObject() const override                               { return ObjectType::Image3DAnimation; }
    GUIType::Sheet getIcon() override                                       { return IconObject::IMAGE_3D_ANIMATION; }
    [[nodiscard]] TextureAnimated *getCurrentTextureAnimation() const       { return this->animations[currentAnimationIndex]; }
    [[nodiscard]] bool isAutoRemoveAfterAnimation() const                   { return autoRemoveAfterAnimation; }
    [[nodiscard]] std::vector<TextureAnimated *> getAnimations() const      { return animations; }

    friend class Image3DAnimationSerializer;
    friend class Image3DAnimationGUI;
};


#endif //BRAKEDA3D_ANIMATEDSPRITE_H
