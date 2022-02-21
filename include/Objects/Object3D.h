#ifndef SDL2_3D_ENGINE_OBJECT3D_H
#define SDL2_3D_ENGINE_OBJECT3D_H

#include <string>
#include "Vertex3D.h"
#include "Vector3D.h"
#include "../../include/Render/M3.h"

class Object3D {
    Vertex3D position;
    M3 rotation;

public:
    float rotX, rotY, rotZ; // For easy management from UI
    Object3D *parent;

    Object3D *getParent() const;

    void setParent(Object3D *parent);

    bool enabled;
    bool removed;
    bool followCamera;

    Vertex3D drawOffset;
    Vertex3D rotationFrame;
    bool rotationFrameEnabled;
    float scale;

    bool decal;     // Decals exclude UV Coordinates out of [0, 1]

    std::string label;

    Object3D();

    virtual ~Object3D() = default;

    std::string getLabel() const;

    void setLabel(const std::string& label);

    Vertex3D &getPosition();

    M3 getRotation();

    void setPosition(Vertex3D p);
    void addToPosition(Vertex3D &);

    void setRotation(M3 r);
    void setRotation(float, float, float);

    bool isEnabled() const;

    void setEnabled(bool enabled);

    Vertex3D AxisUp();

    Vertex3D AxisDown();

    Vertex3D AxisForward();

    Vertex3D AxisBackwards();

    Vertex3D AxisRight();

    Vertex3D AxisLeft();

    float getScale() const;

    void setScale(float scale);

    bool isRemoved() const;

    void setRemoved(bool removed);

    bool isDecal() const;

    void setDecal(bool decal);

    void setDrawOffset(Vertex3D v);

    Vertex3D getDrawOffset() const;

    bool isFollowCamera() const;

    void setFollowCamera(bool followCamera);

    virtual void onUpdate();

    bool isRotationFrameEnabled();
    void setRotationFrameEnabled(bool value);
    void setRotationFrame(Vertex3D v);
};

#endif //SDL2_3D_ENGINE_OBJECT3D_H
