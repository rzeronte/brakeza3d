#ifndef BRAKEDA3D_SHADER_H
#define BRAKEDA3D_SHADER_H

#include <string>
#include "../Misc/Timer.h"
#include "../Objects/Vector2D.h"
#include "../Objects/Vertex3D.h"
#include "../Misc/Color.h"

class FXBase {
protected:
    bool enabled;

    float deltaTime;
    float last_ticks;
    float current_ticks;
    float executionTime;

    Timer t;

    std::string label;
public:
    explicit FXBase(bool enabled)
    :
        enabled(enabled),
        deltaTime(0),
        last_ticks(0),
        current_ticks(0),
        executionTime(0)
    {
        t.start();
    }

    virtual void update()
    {
        current_ticks = (float) this->t.getTicks();
        deltaTime = current_ticks - last_ticks;
        last_ticks = current_ticks;
        executionTime += deltaTime / 1000.f;
    }

    [[nodiscard]] bool isEnabled() const {
        return enabled;
    }

    void setEnabled(bool enabled) {
        FXBase::enabled = enabled;
    }

    [[nodiscard]] const std::string &getLabel() const {
        return label;
    }

    void setLabel(const std::string &label) {
        FXBase::label = label;
    }

    [[nodiscard]] float getExecutionTime() const {
        return executionTime;
    }
};


#endif //BRAKEDA3D_SHADER_H
