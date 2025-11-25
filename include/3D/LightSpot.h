//
// Created by edu on 11/12/23.
//

#ifndef BRAKEZA3D_SPOTLIGHT3D_H
#define BRAKEZA3D_SPOTLIGHT3D_H

#include "LightPoint.h"

class LightSpot : public LightPoint
{
    glm::vec4 direction;
    float cutOff;
    float outerCutOff;

public:
    LightSpot(
        const glm::vec4 &direction,
        const glm::vec4 &ambient,
        const glm::vec4 &diffuse,
        const glm::vec4 &specular,
        float constant,
        float linear,
        float quadratic,
        float cutOff,
        float outerCutOff
    );
    const char *getTypeIcon() override;
    const char *getTypeObject() override;
    void drawImGuiProperties() override;
    void setCutOff(float value);
    void setOuterCutOff(float value);
    void onUpdate() override;
    void setDirection(const Vertex3D &d);
    glm::mat4 getLightSpaceMatrix();
    [[nodiscard]] float getCutOff() const;
    [[nodiscard]] float getOuterCutOff() const;
    static LightSpot* create(const Vertex3D &position, const Vertex3D &direction);

    friend class LightSpotSerializer;
    friend class LightSpotGUI;
};

#endif //BRAKEZA3D_SPOTLIGHT3D_H
