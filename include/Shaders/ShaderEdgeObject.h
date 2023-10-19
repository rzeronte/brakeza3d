//
// Created by eduardo on 14/11/22.
//

#ifndef BRAKEZA3D_SHADEREDGEOBJECT_H
#define BRAKEZA3D_SHADEREDGEOBJECT_H

#include "../Render/ShaderOpenCL.h"
#include "../Misc/Image.h"
#include "../Objects/Object3D.h"
#include "../Objects/Mesh3D.h"
#include "../Render/ObjectShaderOpenCL.h"

class ShaderEdgeObject : public ObjectShaderOpenCL {
private:
    Color color;
    float size;
public:
    explicit ShaderEdgeObject(bool active, Color c, float size);

    void update() override;

    void executeKernelOpenCL();

    void setColor(Color c);

    void preUpdate() override;

    void postUpdate() override;

    void drawImGuiProperties() override;

    ~ShaderEdgeObject() override;

    cJSON * getJSON() override;

    void setSize(float size);

    [[nodiscard]] float getSize() const;

    static ShaderEdgeObject *create();
};


#endif //BRAKEZA3D_SHADEREDGEOBJECT_H
