#version 330 core

in vec2 texCoord;
out vec4 pixelColor;

uniform sampler2D image;
uniform vec3 lineColor;

const float borderThickness = 0.002;  // Ajusta según la resolución de tu imagen
const float edgeThreshold = 0.6;      // Ajusta según la intensidad del borde que quieres detectar

void main() {
        float sobelX[9] = float[](
                -1, 0, 1,
                -2, 0, 2,
                -1, 0, 1
        );
        float sobelY[9] = float[](
                -1, -2, -1,
                0, 0, 0,
                1, 2, 1
        );

        float edgeX = 0.0;
        float edgeY = 0.0;

        for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                        vec3 samp = texture(image, texCoord + vec2(j, i) * borderThickness).rgb;
                        int index = (i + 1) * 3 + (j + 1);
                        edgeX += dot(samp, vec3(sobelX[index]));
                        edgeY += dot(samp, vec3(sobelY[index]));
                }
        }

        float edge = length(vec2(edgeX, edgeY));

        if (edge > edgeThreshold) {
                pixelColor = vec4(lineColor, 1.0); // Borde rojo
        } else {
                discard; //pixelColor = vec4(texture(image, texCoord).rgb, 1.0);
        }
}
