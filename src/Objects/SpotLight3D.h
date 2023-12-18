//
// Created by edu on 11/12/23.
//

#ifndef BRAKEZA3D_SPOTLIGHT3D_H
#define BRAKEZA3D_SPOTLIGHT3D_H


#include "../../include/Objects/LightPoint3D.h"

class SpotLight3D : public LightPoint3D {
public:
    SpotLight3D(
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

    static void setPropertiesFromJSON(cJSON *object, SpotLight3D *o);
};


#endif //BRAKEZA3D_SPOTLIGHT3D_H
