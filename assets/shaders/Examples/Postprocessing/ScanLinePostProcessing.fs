#version 330 core

in vec2 TexCoords;

uniform sampler2D sceneTexture;
uniform float intensity;   // Controla la visibilidad del efecto
uniform float et;          // Execution time in seconds
uniform float speed;       // Speed of the scanlines movement
uniform float heightLine;  // Grosor de las líneas (valores más altos = líneas más espaciadas)

out vec4 FragColor;

void main()
{
    // Movimiento vertical de las scanlines
    float movingOffset = et * speed; // Controla la velocidad del desplazamiento

    // Altura de las líneas: un valor más bajo genera líneas más juntas
    float frequency = 100.0 / heightLine;

    // Generamos las scanlines con el grosor ajustable
    float scanline = sin((TexCoords.y + movingOffset) * frequency) * 0.5 + 0.5;

    // Ajusta la intensidad del efecto
    scanline = mix(1.0, scanline, intensity);

    // Muestra el color original y aplica el efecto
    vec4 color = texture(sceneTexture, TexCoords);
    FragColor = color * scanline; // Oscurece el color en función del patrón de scanlines
}
