#ifndef SDL2_3D_ENGINE_OBJECT3D_H
#define SDL2_3D_ENGINE_OBJECT3D_H

#include <string>
#include "Vertex3D.h"
#include "Vector3D.h"
#include "../../include/Render/M3.h"
#include "../Behaviors/Object3DBehavior.h"
#include "Point2D.h"
#include "../Misc/cJSON.h"

class Object3D {

protected:
    Vertex3D position;
    Vertex3D drawOffset;
    M3 rotation;

    Object3DBehavior *motion;
    Object3D *parent;

    bool enabled;
    bool removed;
    bool decal; // Decals exclude UV Coordinates out of [0, 1]
    bool followCamera;
    bool stencilBufferEnabled;

    std::string label;
    float rotX, rotY, rotZ; // For easy management from UI
    bool alphaEnabled;
    float alpha;
public:

    Object3D();

    virtual ~Object3D();

    void setParent(Object3D *object);

    void setLabel(const std::string& label);

    Vertex3D &getPosition();

    M3 getRotation();

    void setPosition(Vertex3D p);

    void addToPosition(Vertex3D p);

    void setRotation(M3 r);

    void setRotation(float, float, float);

    virtual void setEnabled(bool enabled);

    Vertex3D AxisUp();

    Vertex3D AxisDown();

    Vertex3D AxisForward();

    Vertex3D AxisBackwards();

    Vertex3D AxisRight();

    Vertex3D AxisLeft();

    void setScale(float value);

    void setRemoved(bool value);

    void setDecal(bool value);

    void setDrawOffset(Vertex3D v);

    void setFollowCamera(bool followCamera);

    virtual void onUpdate();

    virtual void postUpdate();

    virtual void drawOnUpdateSecondPass();

    bool isRotationFrameEnabled();

    void setRotationFrameEnabled(bool value);

    void setRotationFrame(Vertex3D v);

    void setStencilBufferEnabled(bool stencilBufferEnabled);

    void setAlpha(float alpha);

    void setAlphaEnabled(bool alphaEnabled);

    void setBehavior(Object3DBehavior *motion);

    float &getRotX();

    float &getRotY();

    float &getRotZ();

    [[nodiscard]] Object3DBehavior *getBehavior() const;

    [[nodiscard]] bool &isAlphaEnabled();

    [[nodiscard]] bool &isStencilBufferEnabled();

    [[nodiscard]] Vertex3D &getDrawOffset();

    [[nodiscard]] float &getAlpha();

    [[nodiscard]] bool &isFollowCamera();

    [[nodiscard]] bool isDecal() const;

    [[nodiscard]] float getScale() const;

    [[nodiscard]] bool isRemoved() const;

    [[nodiscard]] bool &isEnabled();

    [[nodiscard]] std::string getLabel() const;

    [[nodiscard]] Object3D *getParent() const;

    virtual void onDrawHostBuffer();

    [[nodiscard]] bool isEnableLights() const;

    void setEnableLights(bool enableLights);

    bool enableLights;
    float scale;
    Vertex3D rotationFrame;
    bool rotationFrameEnabled;

    virtual cJSON *getJSON();

    [[nodiscard]] const Vertex3D &getRotationFrame() const;

    void lookAt(Object3D *o);
};

#endif //SDL2_3D_ENGINE_OBJECT3D_H
