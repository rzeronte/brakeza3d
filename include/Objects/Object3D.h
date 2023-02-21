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

    [[nodiscard]] std::string getLabel() const;

    void setLabel(const std::string& label);

    Vertex3D &getPosition();

    M3 getRotation();

    void setPosition(Vertex3D p);
    void addToPosition(Vertex3D p);

    void setRotation(M3 r);
    void setRotation(float, float, float);

    [[nodiscard]] bool isEnabled() const;

    virtual void setEnabled(bool enabled);

    Vertex3D AxisUp();

    Vertex3D AxisDown();

    Vertex3D AxisForward();

    Vertex3D AxisBackwards();

    Vertex3D AxisRight();

    Vertex3D AxisLeft();

    [[nodiscard]] float getScale() const;

    void setScale(float value);

    [[nodiscard]] bool isRemoved() const;

    void setRemoved(bool value);

    [[nodiscard]] bool isDecal() const;

    void setDecal(bool value);

    void setDrawOffset(Vertex3D v);

    [[nodiscard]] Vertex3D getDrawOffset() const;

    [[nodiscard]] bool isFollowCamera() const;

    void setFollowCamera(bool followCamera);

    virtual void onUpdate();
    virtual void postUpdate();

    bool isRotationFrameEnabled();
    void setRotationFrameEnabled(bool value);
    void setRotationFrame(Vertex3D v);

    [[nodiscard]] bool *getStencilBuffer() const;

    [[nodiscard]] bool isStencilBufferEnabled() const;

    void setStencilBufferEnabled(bool stencilBufferEnabled);

    void initializeStencilBuffer();
    void setStencilBuffer(int x, int y, bool value);
    void setStencilBuffer(int index, bool value);

    void clearStencilBuffer();

    [[nodiscard]] bool getStencilBufferValue(int i) const;

    [[nodiscard]] bool getStencilBufferValue(int x, int y) const;

    [[nodiscard]] EnemyBehavior *getBehavior() const;

    void setBehavior(EnemyBehavior *motion);

    [[nodiscard]] float getAlpha() const;

    void setAlpha(float alpha);

    [[nodiscard]] bool isAlphaEnabled() const;

    void setAlphaEnabled(bool alphaEnabled);

    bool *stencilBuffer;
};

#endif //SDL2_3D_ENGINE_OBJECT3D_H
