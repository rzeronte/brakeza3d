#ifndef SDL2_3D_ENGINE_LIGHTPOINT_H
#define SDL2_3D_ENGINE_LIGHTPOINT_H

#include "Object3D.h"
#include "../Render/Frustum.h"
#include "../Components/Camera3D.h"
#include "../../imgui/imgui.h"
#include "../Misc/Color.h"

class LightPoint3D : public Object3D {
public:
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
    float constant;
    float linear;
    float quadratic;

    LightPoint3D(
        glm::vec4 ambient,
        glm::vec4 diffuse,
        glm::vec4 specular,
        float constant,
        float linear,
        float quadratic
    );

    void setColor(Color c);

    void setColorSpecular(Color c);

    void onUpdate() override;

    void setConstant(float value);

    void setLinear(float value);

    void setCuadratic(float value);

    const char *getTypeObject() override;

    const char *getTypeIcon() override;

    void drawImGuiProperties() override;

    cJSON *getJSON() override;

    static void createFromJSON(cJSON *object);

    static void setPropertiesFromJSON(cJSON *object, LightPoint3D *o);

    static LightPoint3D* create(Vertex3D position);

    void setAmbient(Color value);
};

#endif //SDL2_3D_ENGINE_LIGHTPOINT_H
