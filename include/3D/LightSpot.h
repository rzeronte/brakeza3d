//
// Created by edu on 11/12/23.
//

#ifndef BRAKEZA3D_SPOTLIGHT3D_H
#define BRAKEZA3D_SPOTLIGHT3D_H

#include <GL/glew.h>

#include "Cone3D.h"
#include "LightPoint.h"

class LightSpot : public LightPoint
{
    float cutOff = 0.f;
    float outerCutOff = 0.f;
    bool showDebugCone = false;

    GLuint vertexBuffer = 0;
    GLuint normalBuffer = 0;
    GLuint uvBuffer = 0;

    Cone3D cone;
public:
    LightSpot(
        const glm::vec4 &ambient,
        const glm::vec4 &diffuse,
        const glm::vec4 &specular,
        float constant,
        float linear,
        float quadratic,
        float cutOff,
        float outerCutOff
    );
    GUIType::Sheet getIcon() override;
    ObjectType getTypeObject() const override;
    void DrawPropertiesGUI() override;
    void RenderDebugCone(float radians, const Color &c);
    void setCutOff(float value);
    void setOuterCutOff(float value);
    void onUpdate() override;
    void crearBuffersCono();
    void clearConeBuffers();
    [[nodiscard]] glm::mat4 getLightSpaceMatrix() const;
    [[nodiscard]] float getCutOff() const;
    [[nodiscard]] float getOuterCutOff() const;

    glm::vec4 getDirection() const;

    friend class LightSpotSerializer;
    friend class LightSpotGUI;
};

#endif //BRAKEZA3D_SPOTLIGHT3D_H
