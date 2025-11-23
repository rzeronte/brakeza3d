//
// Created by edu on 11/12/23.
//

#ifndef BRAKEZA3D_SPOTLIGHT3D_H
#define BRAKEZA3D_SPOTLIGHT3D_H


#include "LightPoint.h"

class LightSpot : public LightPoint {
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

    void drawImGuiProperties() override;

    cJSON *getJSON() override;

    const char *getTypeObject() override;

    void setCutOff(float cutOff);

    void setOuterCutOff(float outerCutOff);

    void onUpdate() override;

    static void createFromJSON(cJSON *object);

    glm::vec4 direction;
    float cutOff;
    float outerCutOff;

    static void setPropertiesFromJSON(cJSON *object, LightSpot *o);

    static LightSpot* create(const Vertex3D &position, const Vertex3D &direction);

    void setDirection(Vertex3D d);

    glm::mat4 getLightSpaceMatrix();
};


#endif //BRAKEZA3D_SPOTLIGHT3D_H
