#ifndef SDL2_3D_ENGINE_OBJECT3D_H
#define SDL2_3D_ENGINE_OBJECT3D_H

#include <string>
#include "Vertex3D.h"
#include "Vector3D.h"
#include "../../include/Render/M3.h"
#include "../../darkheaz/src/enemies/EnemyBehavior.h"
#include "Point2D.h"

class Object3D {
    Vertex3D position;
    M3 rotation;

protected:
    EnemyBehavior *motion;
public:
    float rotX, rotY, rotZ; // For easy management from UI
    Object3D *parent;
    bool alphaEnabled;
    float alpha;

    [[nodiscard]] Object3D *getParent() const;

    void setParent(Object3D *object);

    bool enabled;
    bool removed;
    bool decal;         // Decals exclude UV Coordinates out of [0, 1]
    bool followCamera;
    bool stencilBufferEnabled;

    Vertex3D drawOffset;
    Vertex3D rotationFrame;
    bool rotationFrameEnabled;
    float scale;

    std::string label;

    Object3D();

    virtual ~Object3D();

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

    bool isRotationFrameEnabled();
    void setRotationFrameEnabled(bool value);
    void setRotationFrame(Vertex3D v);

    void setStencilBufferEnabled(bool stencilBufferEnabled);

    void initializeStencilBuffer();

    void setStencilBuffer(int x, int y, bool value);

    void setStencilBuffer(int index, bool value);

    void clearStencilBuffer();

    void setAlpha(float alpha);

    void setAlphaEnabled(bool alphaEnabled);

    void setBehavior(EnemyBehavior *motion);

    [[nodiscard]] bool getStencilBufferValue(int i) const;

    [[nodiscard]] bool getStencilBufferValue(int x, int y) const;

    [[nodiscard]] EnemyBehavior *getBehavior() const;

    [[nodiscard]] float getAlpha() const;

    [[nodiscard]] bool isAlphaEnabled() const;

    [[nodiscard]] bool *getStencilBuffer() const;

    [[nodiscard]] bool isStencilBufferEnabled() const;

    [[nodiscard]] Vertex3D getDrawOffset() const;

    [[nodiscard]] bool isFollowCamera() const;

    [[nodiscard]] bool isDecal() const;

    [[nodiscard]] float getScale() const;

    [[nodiscard]] bool isRemoved() const;

    [[nodiscard]] bool isEnabled() const;

    [[nodiscard]] std::string getLabel() const;

    bool *stencilBuffer;
};

#endif //SDL2_3D_ENGINE_OBJECT3D_H
