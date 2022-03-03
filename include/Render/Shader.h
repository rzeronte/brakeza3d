#ifndef BRAKEDA3D_SHADER_H
#define BRAKEDA3D_SHADER_H


#include <string>
#include "../Misc/Timer.h"

class Shader {
public:
    Shader();

    uint32_t *videoBuffer;
    int w;
    int h;

    int phaseRender;

    float deltaTime = 0;
    float last_ticks = 0;
    float current_ticks = 0;
    float executionTime = 0;

    Timer t;

    std::string label;

    virtual void onUpdate();

    bool isEnabled() const;

    void setEnabled(bool enabled);

    bool enabled;

    const std::string &getLabel() const;

    void setLabel(const std::string &label);

    void setPhaseRender(int type);

    int getPhaseRender() const;
};


#endif //BRAKEDA3D_SHADER_H
