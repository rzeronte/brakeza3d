//
// Created by eduardo on 14/11/22.
//

#ifndef BRAKEZA3D_FXOUTLINER_H
#define BRAKEZA3D_FXOUTLINER_H

#include "../OpenGL/FXEffectOpenGL.h"
#include "../Misc/Image.h"
#include "../Objects/Mesh3D.h"
#include "../OpenGL/FXEffectOpenGLObject.h"

class FXOutliner : public FXEffectOpenGLObject {
private:
    Color color;
    float size;
public:

    FXOutliner(bool active, Object3D *object, const Color &color, float size);

    void update() override;

    void setColor(Color c);

    void preUpdate() override;

    void postUpdate() override;

    void drawImGuiProperties() override;

    cJSON * getJSON() override;

    void setSize(float size);

    [[nodiscard]] float getSize() const;

    static FXOutliner *create();
};


#endif //BRAKEZA3D_FXOUTLINER_H
