#ifndef SDL2_3D_ENGINE_LIGHTPOINT_H
#define SDL2_3D_ENGINE_LIGHTPOINT_H

#include "Object3D.h"
#include "../Render/Frustum.h"
#include "../Components/Camera3D.h"
#include "../../imgui/imgui.h"
#include "../Render/Fragment.h"
#include "../Misc/Color.h"

class LightPoint3D : public Object3D {
public:

    LightPoint3D(
        float power,
        float kc,
        float kl,
        float kq,
        float specularComponent,
        Color c,
        Color specularityColor
    );

    void syncFrustum();

    void clearShadowMappingBuffer() const;

    [[nodiscard]] float getShadowMappingBuffer(int x, int y) const;

    void setShadowMappingBuffer(int x, int y, float value) const;

    Color color;
    Color specularColor;

    float kc = 1;   // constant attenuation
    float kl = 0;   // linear attenuation
    float kq = 0;   // quadratic attenuation

    float p = 100;

    Frustum *frustum;

    float *shadowMappingBuffer;
    int sizeBuffer;

    float specularComponent = 20;

    bool showDeepMapping;
    bool showFrustum;

    void setColor(Color c);
    void setColorSpecularity(Color c);

    Color mixColor(Color colorTexture, Vertex3D Q, Fragment *);

    void onUpdate();

    void setPower(float p);

    void setConstant(float value);
    void setLinear(float value);
    void setCuadratic(float value);

    Vertex3D getHalfWay(Vertex3D v);
    void setSpecularComponent(float m);

    void drawDeepMap(int pos_x, int pos_y) const;

    void onDrawHostBuffer() override;

    const char *getTypeObject() override;

    const char *getTypeIcon() override;
};


#endif //SDL2_3D_ENGINE_LIGHTPOINT_H
