#version 330 core

layout (location = 0) in vec4 aPosition;
layout (location = 1) in float aActive;
layout (location = 2) in float aTimeToLive;
layout (location = 3) in float aTimeLiving;

struct OCParticlesContext {
    float gravity;
    float step_add_particle;
    float particle_lifespan;
    float smoke_angle_range;
    float min_velocity;
    float max_velocity;
    float alpha_min;
    float alpha_max;
    float position_noise;
    float velocity_noise;
    float deceleration_factor;
};

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 colorFrom;
uniform vec3 colorTo;

out vec4 fragColor;
out float drawParticle;

void main()
{
    gl_Position = projection * view * vec4(aPosition.xyz, 1.0);

    gl_PointSize = 2.0f;
    fragColor = mix(vec4(colorFrom, 1), vec4(colorTo, 1), aTimeLiving / aTimeToLive);
    drawParticle = aActive;
}
