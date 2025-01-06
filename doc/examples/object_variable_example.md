# Ejemplo de uso de variables de objeto

Igual que los `scripts globales` definen variables de acceso global entre scripts. Los `scripts de objeto`
tambien pueden definir sus propias variables que serán instanciadas en cada objeto.

Los tipos de variables soportados son: `string`, `int`, `float` y `Vertex3D`.

# Ficheros de tipos

Cada script LUA incorpora un fichero de tipos, de esta forma, si tenemos un script LUA llamado `my_script.lua`, también dispondremos de un fichero llamado `my_script.json`

No te preocupes, Brakeza3D creará y manejará automáticamente estos ficheros. Podrás gestionar las variables de un script desde la GUI.

A continuación vemos el contenido de uno de estos ficheros (formato JSON):

```json
{
	"name":	"object_variable.lua",
	"types": [
      {
        "name":	"count",
        "type":	"int",
        "value": 0
      },
      {
        "name":	"state",
        "type":	"int",
        "value": 0
      }
    ]
}

```

Puedes observar como se define una variable de objeto: `count` y `state`, ambas de tipo `int` y con valor `0`. Las usaremos a continuación para ejemplificar su uso.

---

# Scope de las variables

Estas variables son accesibles durante toda la ejecución del script. En este tiempo el programador podrá modificar su valor a tu antojo, sin embargo esta recuperará su valor inicial en cada nueva ejecución.

Las variables definidas en `scripts de objeto` no son únicas, ya que cada objeto **instancia su propia versión** de la variable, aunque también podrás acceder a ellas desde otros scripts como veremos más adelante.

---

Siguiendo con el ejemplo anterior podriamos encontrar el siguiente código LUA:

```
function onUpdate()
    scale = this:getScale()
    if scale > 1 and state == 1 then
        print("Increasing scale > 1 times: " .. count)
        state = 0
        count = count + 1
    end
    if scale <= 1 then
        state = 1
    end
end

```

# Explicación del código

El ejemplo muestra un contador de veces que el objeto ha tenido un factor de escala superior a uno.

Podemos ver como a través de la función `getScale()` del objeto `this` obtenemos la magnitud de escala y como manipulando
las variables de `objeto locales`.

Si vinculamos este script a dos objetos independientes, podremos ver como ejecutan su lógica con instancias de variables separadas, lo que hará
que cada objeto lleve su propia cuenta por separado.

