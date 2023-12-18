#ifndef BRAKEDA3D_SHADER_H
#define BRAKEDA3D_SHADER_H


#include <string>
#include "../Misc/Timer.h"
#include "../Objects/Vector2D.h"
#include "../Objects/Vertex3D.h"
#include "../Misc/Color.h"

class Shader {
protected:

    bool enabled;

    int w;
    int h;

    Vector2D resolution;

    float deltaTime;
    float last_ticks;
    float current_ticks;
    float executionTime;

    Timer t;

    std::string label;
public:
    explicit Shader(bool enabled);

    virtual void update();

    void setEnabled(bool enabled);

    void setLabel(const std::string &label);

    static float step(float limit, float value);

    [[nodiscard]] const std::string &getLabel() const;

    [[nodiscard]] bool isEnabled() const;

};


#endif //BRAKEDA3D_SHADER_H
