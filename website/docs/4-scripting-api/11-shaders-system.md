---
sidebar_position: 12
title: Shaders system
description: Shaders system API in Brakeza3D including uniforms, GLSL templates, and internal shader reference.
---

# Shaders system
---

## Shader Variables (uniforms)
---

You can also configure the variables (uniforms) that each shader will receive from the UI.

You can create your own variables primitives vars:
    - int
    - float
    - vec2
    - vec3
    - vec4
    - texture

Or you can select system uniforms:

     - delta_time (float)
     - execution_time (float)
     - scene (texture).

Additionally, geometry shaders (which only apply to Mesh3D) allow you to use system types such as diffuse
or specular, with textures belonging to the particular model.


### Postprocessing Shader Template
---

Below are empty templates for the postprocessing shader.

VertexShader:

```lua
#version 330 core

layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 projection;

void main()
{
    TexCoords = vec2(vertex.z, -vertex.w);
    gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
}
```

FragmentShader:

```lua
#version 330 core

in vec2 TexCoords;

//out vec4 FragColor;

void main()
{
   //FragColor = texture(sceneTexture, TexCoords);
}
```


### Geometry Shader Template
---

Below are empty templates for the geometry shader.

VertexShader:

```lua
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}
```

FragmentShader:

```lua
#version 330 core

//uniform sampler2D diffuse;

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    //FragColor = texture(diffuse, TexCoords);
}
```


## Internal Shaders
---
You can find the shaders that **Brakeza3D** uses internally in the `/GLSL` folder.

### Mesh/Object Shaders

| Shader    | Files                       | Description                                         |
|-----------|-----------------------------|-----------------------------------------------------|
| Render    | Render.vs / Render.fs       | Mesh3D forward rendering                            |
| Outliner  | Outliner.vs / Outliner.fs   | Outliner for a selected object                      |
| Color     | Color.vs / Color.fs         | Renders a Mesh3D with a given color                 |
| Wireframe | Wireframe.vs / Wireframe.fs | Renders vertices of a Mesh3D                        |
| Points    | Points.vs / Points.fs       | Renders vertices of a Mesh3D                        |
| Line3D    | Line3D.vs / Line3D.fs       | Renders 3D lines                                    |
| Line      | Line.vs / Line.fs           | Draws a 2D line on screen                           |
| Image     | Image.vs / Image.fs         | Renders an image on screen                          |
| Particle  | Particle.vs / Particle.fs   | Draws a particle on screen; used by ParticleEmitter |
| Shading   | Shading.vs / Shading.fs     | Renders an object to a buffer to be used as a mask  |

---

### Postprocessing / Scene Shaders

| Shader               | Files                                             | Description                                         |
|----------------------|---------------------------------------------------|-----------------------------------------------------|
| FOG                  | FOG.vs / Fog.fs                                   | Atmospheric fog effect                              |
| DepthMap             | DepthMap.vs / DepthMap.fs                         | Draws the depth map of the scene                    |
| Tint                 | Tint.vs / Tint.fs                                 | Colors the screen with a given color and alpha      |
| BonesTransforms      | BonesTransforms.vs                                | Bone transform calculations for skeletal animation  |
| GBuffer              | GBuffer.vs / GBuffer.fs                           | G-buffer pass for deferred rendering                |
| LightingPass         | LightingPass.vs / LightingPass.fs                 | Handles lighting computations for deferred pipeline |
| ShadowPass           | ShadowPass.vs / ShadowPass.fs                     | Shadow mapping pass                                 |
| ShadowPassDebugLight | ShadowPassDebugLight.vs / ShadowPassDebugLight.fs | Shadow debugging visualization                      |
| Grid                 | Grid.vs / Grid.fs                                 | Debug grid overlay on the ground plane              |
| GroundCircle         | GroundCircle.vs / GroundCircle.fs                 | Selection circle projected on the ground, masked by G-Buffer geometry |
| GroundDecal          | GroundDecal.vs / GroundDecal.fs                   | Decal texture projected onto the ground under an object |
| AxisQuad             | AxisQuad.vs / AxisQuad.fs                         | Flat quad aligned to a world axis (used for markers/overlays) |
| Rect                 | Rect.fs                                           | Filled rectangle for 2D UI drawing                 |

If you know what you’re doing, you can freely manipulate the shaders as you see fit!

---

## Scripting API for Shaders
---

You can retrieve and modify scene shaders at runtime from Lua using `ComponentRender`.

```lua
local shader = Components:Render():getSceneShaderByLabel("MyShader")
shader:setEnabled(true)
shader:setDataTypeValue("myUniform", value)
```

### ShaderBaseCustomOGLCode methods

| Method | Description |
|--------|-------------|
| `setEnabled(bool)` | Enables or disables the shader in the pipeline |
| `isEnabled()` | Returns whether the shader is currently active |
| `setDataTypeValue(name, value)` | Sets a uniform value by name (float, int, vec2, vec3, vec4) |
| `setTextureValue(name, texture)` | Assigns a texture to the named sampler uniform |