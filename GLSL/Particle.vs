
#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 squareVertices;
layout(location = 1) in vec4 xyzs; // Position of the center of the particule and size of the square
layout(location = 2) in vec4 color; // Position of the center of the particule and size of the square
layout(location = 3) in float angle; // rotación de la partícula en el plano de la pantalla (radianes)

// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec4 particlecolor;

// Values that stay constant for the whole mesh.
uniform vec3 CameraRight_worldspace;
uniform vec3 CameraUp_worldspace;
uniform mat4 VP; // Model-View-Projection matrix, but without the Model (the position is in BillboardPos; the orientation depends on the camera)
// 0 = billboard (original). Distinto de 0: el quad se queda en el plano generado por flatRight/flatUp (no mira a la cámara)
uniform int  orientation;
uniform vec3 flatRight;
uniform vec3 flatUp;
uniform vec3 cameraPos;   // posición de la cámara en el mundo (solo VERTICAL_BILLBOARD)
// Balanceo de las partículas planas (orientation != 0): cada una se ladea sobre sus dos ejes, con fase propia (su ángulo).
// flutterAngle = inclinación máxima en grados (0 = sin balanceo, comportamiento anterior); flutterSpeed = ciclos de vaivén por radián girado
uniform float flutterAngle;
uniform float flutterSpeed;

void main()
{
    float particleSize = xyzs.w; // because we encoded it this way.
    vec3 particleCenter_wordspace = xyzs.xyz;

    // Giro del quad sobre sí mismo antes de orientarlo a la cámara (UV sin girar: gira la textura)
    float c = cos(angle), s = sin(angle);
    vec2 corner = vec2(c * squareVertices.x - s * squareVertices.y,
                       s * squareVertices.x + c * squareVertices.y);

    vec3 axisRight = CameraRight_worldspace;
    vec3 axisUp    = CameraUp_worldspace;
    if (orientation == 3) {
        // VERTICAL_BILLBOARD: el eje largo del quad sigue la vertical del mundo proyectada sobre el plano de la pantalla y el
        // ancho va perpendicular: siempre mira a la cámara (nunca de canto, ni con vista cenital). Se acorta con la
        // perspectiva (vista cenital = casi un punto, como una gota que cae hacia la cámara), con un mínimo para que no desaparezca
        // dirección REAL de esta gota a la cámara (no la del eje de la cámara): con perspectiva la trayectoria en pantalla
        // se abre desde el punto de fuga y la raya tiene que seguirla, como un proyectil
        vec3 toCam = normalize(cameraPos - particleCenter_wordspace);
        vec3 pr    = vec3(0.0, 1.0, 0.0) - toCam * toCam.y;
        float len  = length(pr);
        vec3 vUp   = len > 1e-4 ? pr / len : CameraUp_worldspace;
        axisRight  = normalize(cross(toCam, vUp));
        axisUp     = vUp * (0.5 + 0.5 * len);
    } else if (orientation != 0) {
        axisRight = flatRight;
        axisUp    = flatUp;
    }

    vec3 offset = axisRight * corner.x + axisUp * corner.y;
    if (orientation != 0 && orientation != 3 && flutterAngle > 0.0) {
        // Fase = ángulo de la partícula: continuo y propio de cada una (nace aleatorio, gira a su velocidad). No usar
        // gl_InstanceID: en la ruta CPU el orden del buffer cambia cada frame (SortParticles) y la fase saltaría
        float amp = radians(flutterAngle);
        float a1 = sin(angle * flutterSpeed) * amp;
        float a2 = sin(angle * flutterSpeed * 0.77 + 1.7) * amp;
        vec3 n = normalize(cross(flatRight, flatUp));
        offset = axisRight * corner.x * cos(a1) + axisUp * corner.y * cos(a2)
               + n * (corner.x * sin(a1) + corner.y * sin(a2));
    }

    vec3 vertexPosition_worldspace = particleCenter_wordspace + offset * particleSize;

    // Output position of the vertex
    gl_Position = VP * vec4(vertexPosition_worldspace, 1.0f);

    // UV of the vertex. No special space for this one.
    UV = squareVertices.xy + vec2(0.5, 0.5);
    particlecolor = color;
}

