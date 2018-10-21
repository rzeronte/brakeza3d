#ifndef SDL2_3D_ENGINE_OBJECT3D_H
#define SDL2_3D_ENGINE_OBJECT3D_H

#include <string>
#include "Vertex3D.h"
#include "Vector3D.h"

class Billboard;

class Object3D {
public:
    bool enabled;
    Vertex3D position;
    Vertex3D rotation;

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
    Vertex3D* getRotation();

    void setPosition(Vertex3D p);
    void setRotation(Vertex3D r);

    void updateAxis();

    bool isHandleKeyboard() const;
    void setHandleKeyboard(bool handleKeyboard);

    bool isEnabled() const;

    void setEnabled(bool enabled);

};

#endif //SDL2_3D_ENGINE_OBJECT3D_H
