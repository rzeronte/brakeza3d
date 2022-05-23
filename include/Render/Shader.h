#ifndef BRAKEDA3D_SHADER_H
#define BRAKEDA3D_SHADER_H


#include <string>
#include "../Misc/Timer.h"
#include "../Objects/Vector2D.h"
#include "../Objects/Vertex3D.h"
#include "../Misc/Color.h"

#define PI 3.142857

class Shader {
public:
    Shader();

    uint32_t *videoBuffer;
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

    virtual void update();

    bool isEnabled() const;

    void setEnabled(bool enabled);

    bool enabled;

    const std::string &getLabel() const;

    void setLabel(const std::string &label);

    void setPhaseRender(int type);

    int getPhaseRender() const;

    virtual ~Shader();

    static float fract(float x);

    static float mix(float a, float b, float f);
    static Vertex3D mix(Vertex3D a, Vertex3D b, float f);
    static Color mix(Color &a, Color &b, float f);

    static float smoothstep(float edge0, float edge1, float x);

    static float step(float limit, float value);

};


#endif //BRAKEDA3D_SHADER_H
