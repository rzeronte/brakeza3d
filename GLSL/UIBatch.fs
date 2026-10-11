#version 330 core
// Mismo resultado que Image.fs con tintColor/alpha (y que Rect.fs con la textura blanca), pero con el
// color por vértice: color = vec4(tinte.rgb, alfa) * textura.
// Hasta 8 texturas por tanda. GLSL 330 solo permite indexar un array de samplers con una expresión
// constante, así que se elige con un switch (válido en cualquier GPU; indexar con TexIndex no lo es).
in vec2 TexCoords;
in vec4 VertexColor;
flat in int TexIndex;

uniform sampler2D images[8];

out vec4 color;

void main()
{
    vec4 t;
    switch (TexIndex) {
        case 0:  t = texture(images[0], TexCoords); break;
        case 1:  t = texture(images[1], TexCoords); break;
        case 2:  t = texture(images[2], TexCoords); break;
        case 3:  t = texture(images[3], TexCoords); break;
        case 4:  t = texture(images[4], TexCoords); break;
        case 5:  t = texture(images[5], TexCoords); break;
        case 6:  t = texture(images[6], TexCoords); break;
        default: t = texture(images[7], TexCoords); break;
    }
    color = VertexColor * t;
}
