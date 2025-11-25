#ifndef SDL2_3D_ENGINE_LIGHTPOINT_H
#define SDL2_3D_ENGINE_LIGHTPOINT_H

#include "Object3D.h"
#include "../Render/Frustum.h"
#include "../Render/Color.h"

class LightPoint : public Object3D {
public:
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
    float constant;
    float linear;
    float quadratic;

    LightPoint(glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular, float constant, float linear,float quadratic);
    void setColor(const Color &c);
    void setColorSpecular(const Color &c);
    void onUpdate() override;
    void setConstant(float value);
    void setLinear(float value);
    void setCuadratic(float value);
    const char *getTypeObject() override;
    const char *getTypeIcon() override;
    void drawImGuiProperties() override;
    static LightPoint* create(Vertex3D position);
    void setAmbient(const Color &c);

    friend class LightPointSerializer;
    friend class LightPointGUI;
};

#endif //SDL2_3D_ENGINE_LIGHTPOINT_H
