#version 330 core

in vec2 TexCoords;

uniform sampler2D sceneTexture;

uniform float shockwaveTime;
uniform float shockwaveFocalPointX;
uniform float shockwaveFocalPointY;

vec3 shockParams = vec3(10, 0.8, 0.1);
float shockwaveSpeed = 2.0;

out vec4 FragColor;

void main()
{
    vec2 center = vec2(shockwaveFocalPointX, -shockwaveFocalPointY);

    vec2 uv = TexCoords.xy;
    vec2 texCoord = uv;

    float dist = distance(uv, center)* shockwaveSpeed;
    if ((dist <= (shockwaveTime + shockParams.z)) && (dist >= (shockwaveTime - shockParams.z)))
    {
        float diff = (dist - shockwaveTime);
        float powDiff = 1.0 - pow(abs(diff * shockParams.x), shockParams.y);

        // Aplicar el efecto de la onda de choque a las coordenadas de textura en las direcciones X e Y
        float diffTimeX = diff * powDiff;
        float diffTimeY = diff * powDiff;

        vec2 diffUV = normalize(uv - center); // Multiplicar por la velocidad
        texCoord.x = uv.x + (diffUV.x * diffTimeX);
        texCoord.y = (uv.y + (diffUV.y * diffTimeY));
    }

    FragColor = texture(sceneTexture, texCoord);
}
