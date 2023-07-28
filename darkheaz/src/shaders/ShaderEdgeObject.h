//
// Created by eduardo on 14/11/22.
//

#ifndef BRAKEZA3D_SHADEREDGEOBJECT_H
#define BRAKEZA3D_SHADEREDGEOBJECT_H


#include "../../../include/Render/ShaderOpenCL.h"
#include "../../../include/Misc/Image.h"
#include "../../../include/Objects/Object3D.h"
#include "../../../include/Objects/Mesh3D.h"

class ShaderEdgeObject : public ShaderOpenCL {
private:
    Mesh3D* object;
    Color color;
public:
    explicit ShaderEdgeObject(bool active, Color c);

    void update() override;

    ~ShaderEdgeObject() override;

    void executeKernelOpenCL();

    void setColor(Color c);

    void setObject(Mesh3D *object);
};


#endif //BRAKEZA3D_SHADEREDGEOBJECT_H
