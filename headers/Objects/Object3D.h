#ifndef SDL2_3D_ENGINE_OBJECT3D_H
#define SDL2_3D_ENGINE_OBJECT3D_H

#include <string>
#include "Vertex3D.h"
#include "Vector3D.h"
#include "../../src/Render/Rotation3D.h"

class Billboard;

class Object3D {
    Vertex3D position;
    Rotation3D rotation;
public:
    bool enabled;

    Vector3D forward;
    Vector3D up;
    Vector3D right;

    float scale;

    std::string label;
    bool handleKeyboard;

    Object3D();
    virtual ~Object3D() = default;

    std::string getLabel() const;
    void setLabel(const std::string label);

    Vertex3D* getPosition();
    Rotation3D* getRotation();

    void setPosition(Vertex3D p);
    void setRotation(Rotation3D r);

    void updateAxis();

    bool isHandleKeyboard() const;
    void setHandleKeyboard(bool handleKeyboard);

    bool isEnabled() const;

    void setEnabled(bool enabled);

};

#endif //SDL2_3D_ENGINE_OBJECT3D_H
