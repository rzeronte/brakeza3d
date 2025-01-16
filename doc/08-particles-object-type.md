> [Back to index](https://github.com/rzeronte/brakeza3d/blob/master/doc/00-index.md)

# Emisores de partículas

Brakeza3D incorpora un objeto dedicado a emitir partículas, nos referimos a `ParticleEmitter`.

Es un emisor de partículas emite imagenes 2D según la configuración dada.

Para configurar un `ParticleEmitter` debes configurar una serie de parámetros:

- `Posición`: Posición 3D del emisor de partículas.
- `Tiempo de vida`: TTL del emisor de partículas en segundos.
- `Color inicio`: (Opcional). Color inicial de la partícula al nacer.
- `Color final`: (Opcional). Color final de la partícula al morir.
- `Contexto`: Contexto del emisor de partículas.
- `Imagen`: Fichero de imagen.

El argumento más destacable es el `contexto`. El `contexto` incluye multitud de parámetros
que podremos manipular para alterar el comportamiento de nuestras partículas. Los contextos
de emisores de partículas se implementan a través de un objeto: ``ParticlesContext``:

Las propiedades de un ``ParticlesContext`` son:

- `gravity (float)`: Factor gravitatorio de las partículas.
- `particlesByFrame (float)`: Número de partículas creadas por segundo
- `particleLifespan (float)`: Tiempo de vida de cada partícula.
- `angleRange (int)`: Ángulo del cono de emisión de partículas.
- `minVelocity (int)`: Velocidad mínima de la partícula al nacer.
- `maxVelocity (int)`: Velocidad máxima de la particula al ser nacer.
- `alphaMin (int)`: Canal alpha mínimo de la partícula al nacer.
- `alphaMax (int)`: Canal alpha máximo de la partícula al nacer.
- `positionNoise (int)`: Ruido en la posición de nacimiento de la partícula.
- `velocityNoise (int)`: Ruido en el movimiento de la partícula.
- `decelerationFactor (float)`: Factor de deceleración de la partícula.

Podrás manipular las propiedades de los ``ParticlesContext`` desde la UI.

Ejempl de creación de un emisor de partículas desde código:

```
    print("Load ParticleEmitter")
    particles = ParticleEmitter.create(
        Vertex3D.new(10, 10, 10),       -- position
        100,                            -- ttl
        Color.new(255, 255, 0, 255),    -- color from
        Color.new(255, 255, 255, 255),  -- color to
        ParticlesContext.new(
            0.0,            -- gravity
            2,              -- particlesByFrame              
            1.0,            -- particleLifespan
            25.0,           -- angleRange
            1,              -- minVelocity
            10,             -- maxVelocity
            125.0,          -- alphaMin
            255.0,          -- alphaMax
            5,              -- positionNoise
            10,             -- velocityNoise
            0.99            -- decelerationFactor
        ),
        "../assets/images/logo_small.png"       --image
    )
```
