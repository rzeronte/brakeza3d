#ifndef SDL2_3D_ENGINE_OBJECT3D_H
#define SDL2_3D_ENGINE_OBJECT3D_H

#include <string>
#include "Vertex3D.h"
#include "Vector3D.h"
#include "../../include/Render/M3.h"
#include "../../darkheaz/src/enemies/EnemyBehavior.h"
#include "Point2D.h"

class Object3D {

protected:
    Vertex3D position;
    Vertex3D drawOffset;
    Vertex3D rotationFrame;
    M3 rotation;

    EnemyBehavior *motion;
    Object3D *parent;
    bool *stencilBuffer;

    bool enabled;
    bool removed;
    float scale;
    bool decal; // Decals exclude UV Coordinates out of [0, 1]
    bool followCamera;
    bool stencilBufferEnabled;
    bool rotationFrameEnabled;

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

    float &getRotX();

    float &getRotY();

    float &getRotZ();

    [[nodiscard]] bool getStencilBufferValue(int i) const;

    [[nodiscard]] bool getStencilBufferValue(int x, int y) const;

    [[nodiscard]] EnemyBehavior *getBehavior() const;

    [[nodiscard]] bool &isAlphaEnabled();

    [[nodiscard]] bool *getStencilBuffer() const;

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


};

#endif //SDL2_3D_ENGINE_OBJECT3D_H
