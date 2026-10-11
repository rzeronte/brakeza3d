#version 330 core
// Dibujado de UI por lotes (ShaderOGLUIBatch): muchos quads en un solo glDrawArrays.
// Por vértice: posición en px del destino, UV, color (tinte rgb + alfa ya multiplicados) e índice de
// la textura del lote (0..7).
layout (location = 0) in vec4 posUV;   // xy = posición (px, origen arriba-izquierda), zw = UV
layout (location = 1) in vec4 color;   // tinte.rgb, alfa final
layout (location = 2) in float texIndex;

out vec2 TexCoords;
out vec4 VertexColor;
flat out int TexIndex;

uniform mat4 projection;

void main()
{
    TexCoords   = posUV.zw;
    VertexColor = color;
    TexIndex    = int(texIndex + 0.5);
    gl_Position = projection * vec4(posUV.xy, 0.0, 1.0);
}
