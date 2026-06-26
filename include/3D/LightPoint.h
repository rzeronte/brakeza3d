#ifndef SDL2_3D_ENGINE_LIGHTPOINT_H
#define SDL2_3D_ENGINE_LIGHTPOINT_H

#include "Object3D.h"
#include "../Render/Color.h"

class LightPoint : public Object3D {
public:
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
    float constant;
    float linear;
    float quadratic;

    bool  frustumCullingEnabled = false;
    float frustumCullingOffset  = 0.0f;

    void setFrustumCullingEnabled(bool v)   { frustumCullingEnabled = v; }
    void setFrustumCullingOffset(float v)   { frustumCullingOffset  = v; }
    [[nodiscard]] bool  getFrustumCullingEnabled() const { return frustumCullingEnabled; }
    [[nodiscard]] float getFrustumCullingOffset()  const { return frustumCullingOffset;  }

    LightPoint(glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular, float constant, float linear,float quadratic);
    void setColor(const Color &c);
    void setColorSpecular(const Color &c);
    void onUpdate() override;
    void setConstant(float value);
    void setLinear(float value);
    void setCuadratic(float value);
    [[nodiscard]] float getRadius() const;
    ObjectType getTypeObject() const override;
    GUIType::Sheet getIcon() override;
    void DrawPropertiesGUI() override;
    void setAmbient(const Color &c);

    friend class LightPointSerializer;
    friend class LightPointGUI;
};

#endif //SDL2_3D_ENGINE_LIGHTPOINT_H
