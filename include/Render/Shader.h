#ifndef BRAKEDA3D_SHADER_H
#define BRAKEDA3D_SHADER_H


#include <string>
#include "../Misc/Timer.h"
#include "../Objects/Vector2D.h"
#include "../Objects/Vertex3D.h"
#include "../Misc/Color.h"

#define PI 3.142857

class Shader {
protected:

    uint32_t *videoBuffer;
    bool enabled;

    int w;
    int h;

    int bufferSize;
    Vector2D resolution;

    int phaseRender;

    float deltaTime = 0;
    float last_ticks = 0;
    float current_ticks = 0;
    float executionTime = 0;

    Timer t;

    std::string label;
public:
    Shader();

    virtual ~Shader();

    virtual void update();

    void setEnabled(bool enabled);

    void setLabel(const std::string &label);

    void setPhaseRender(int type);

    static float fract(float x);

    static float mix(float a, float b, float f);

    static Vertex3D mix(Vertex3D a, Vertex3D b, float f);

    static Color mix(Color &a, Color &b, float f);

    static float smoothStep(float edge0, float edge1, float x);

    static float step(float limit, float value);

    void flipToVideo();

    [[nodiscard]] int getPhaseRender() const;

    [[nodiscard]] const std::string &getLabel() const;

    [[nodiscard]] bool isEnabled() const;

};


#endif //BRAKEDA3D_SHADER_H
