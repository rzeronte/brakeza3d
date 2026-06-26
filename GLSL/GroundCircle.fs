#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform vec4      circleColor;
uniform float     thickness;
uniform sampler2D gPosition;
uniform vec2      screenSize;
uniform float     circleWorldY;
uniform bool      uFilled;

void main()
{
    // Samplear posición de la escena en este píxel de pantalla
    vec2 screenUV = gl_FragCoord.xy / screenSize;
    vec3 fragPos  = texture(gPosition, screenUV).rgb;

    // Si hay geometría en este píxel y está por encima del plano del círculo → el personaje tapa al círculo
    if (dot(fragPos, fragPos) > 0.001 && fragPos.y > circleWorldY + 0.05) {
        discard;
    }

    vec2  uv   = TexCoord * 2.0 - 1.0;
    float dist = length(uv);

    float alpha;
    if (uFilled) {
        // Disco suave: máximo en centro, se desvanece en el borde
        alpha = 1.0 - smoothstep(0.2, 1.0, dist);
    } else {
        float center    = 0.85;
        float halfWidth = thickness * 0.5;
        float fadeWidth = 0.10;
        float inner     = center - halfWidth;
        float outer     = center + halfWidth;
        alpha = (1.0 - smoothstep(outer, outer + fadeWidth, dist))
              * smoothstep(inner - fadeWidth, inner, dist);
    }

    if (alpha < 0.01) discard;
    FragColor = vec4(circleColor.rgb, circleColor.a * alpha);
}
