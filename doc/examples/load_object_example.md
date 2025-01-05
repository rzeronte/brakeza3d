# Ejemplo de carga de un objeto

Principalmente un mundo en tres dimensiones alberga, precisamente, objetos en tres dimensiones, comunmente llamados modelos o mallas.

El objeto principal que representa un objeto en tres dimensiones, con geometría visible, es decir, triángulos, es `Mesh3D`.

Brakeza3D trabaja con la libreria Assimp para carga de modelos, por lo que en principio podremos cargar cualquier formato de modelo que permita esta, sin embargo, en estos ejemplos, vamos a enfocarnos exclusivamente al formato 'FBX'. 

En la carpeta `assets/models/` encontrarás algunos ejemplos de prueba.

El código para cargar una escena es el siguiente:

```
function onStart()
    ...
    eye = Mesh3D:create()
    eye:setEnabled(true)
    eye:setBelongToScene(false)
    eye:setPosition(Vertex3D.new(0, 0, 600))
    eye:setScale(0.5)
    eye:AssimpLoadGeometryFromFile("../assets/models/eye.fbx")

    brakeza:addObject3D(eye, "my_eye_model")
    ...
end
```

Si el fichero `FBX` es correcto, se producirá la carga del modelo en el mundo. Encontrarás información sobre el proceso de carga en la pestaña de logs.

```
Loading scene: %s"
```


# Explicación del código

Lo primero, creamos el objeto de tipo `Mesh3D` con el código `Mesh3D:create()`.

Un objeto tipo `Mesh3D` dispone de multitud de métodos que iremos conociendo, la mayoría se entienden fácilmente por su nombre.

Nos encontramos el uso métodos comunes como `setEnabled`, `setPosition`, `setScale`.

Además, en este caso, el método `setBelongToScene` es utilizado para indicar que este objeto no es original de la escena. Esto es importante, ya que los objetos no pertenecientes a la escena, son eliminados en cada inicio de ejecución de los scripts.

El siguiente paso es usar el método `AssimpLoadGeometryFromFile`, proporcionándole la ruta al fichero `FBX`, para la carga del modelo.

Finalmente añadimos el objeto creado al mundo mediante `brakeza:addObject3D(eye, "my_eye_model")`, donde `my_eye_model` será el nombre del modelo en el mundo.

