# Ejemplo de uso de variables globales

Para la implementación de tu lógica necesitarás crear variables que sean accesibles entre scripts. En Brakeza3D, los scripts permiten definir variables que estarán a disposición del programador
en tiempo de ejecución.

Actualmente los tipos de variables soportados son: `string`, `int`, `float` y `Vertex3D`.

# Ficheros de tipos

Cada script LUA incorpora un fichero de tipos, de esta forma, si tenemos un script LUA llamado `my_script.lua`, también dispondremos de un fichero llamado `my_script.json`

No te preocupes, Brakeza3D creará y manejará automáticamente estos ficheros. Podrás gestionar las variables de un script desde la GUI.

A continuación vemos el contenido de uno de estos ficheros (formato JSON):

```json
{
	"name":	"global_script_example.lua",
	"types": [
        {
          "name": "var1",
          "type": "string",
          "value": "hello my friend!"
        },
        {
          "name": "var2",
          "type": "int",
          "value": 10
        }      
    ]
}

```

Puedes observar como se definen dos variables:
- `var1`, de tipo `string` y con valor `hello my friend!`
- `var2`, de tipo `int` y con valor `10`

---

# Scope de las variables

Estas variables son accesibles durante toda la ejecución del script. En este tiempo el programador podrá modificar su valor a tu antojo, sin embargo esta recuperará su valor inicial en cada nueva ejecución.

Es importante entender que las variables definidas en `scripts globales` serán **únicas** de forma **global** y podrán ser accesibles por otros scripts.

Sin embargo las variables definidas en `scripts de objeto` no son únicas, ya que cada objeto **instancia su propia versión** de la variable, aunque también podrás acceder a ellas desde otros scripts.



---

Siguiendo con el ejemplo anterior podriamos encontrar el siguiente código LUA:

```
function onUpdate()
    var1 = var1 .. "!"
    print("Value of var1: " .. var1)

    var2 = var2 + 1
    print("Value of var2: " .. var2)
end

```

# Explicación del código

En este ejemplo, se observa como en cada frame de ejecución se modifica el valor de la variable `var1` y `var2`.

Podriamos observar como las variables modifican su valor en tiempo real en la pestaña de logs hasta que la ejecución sea parada/pausada.

