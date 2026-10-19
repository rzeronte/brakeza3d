//
// Created by eduardo on 3/10/23.
//
#ifndef BRAKEZA3D_SCRIPTLUA_H
#define BRAKEZA3D_SCRIPTLUA_H

#include <atomic>
#include <cstdint>
#include <map>
#include <unordered_map>
#include <string>
#include <utility>
#include <mutex>
#include "cJSON.h"
#include "../../sol/sol.hpp"
#include "../3D/Vertex3D.h"
#include "../OpenGL/Base/SharedOpenGLStructs.h"

typedef std::variant<int, float, Vertex3D, std::string> LUADataValue;

enum class LUADataType {
    INT,
    FLOAT,
    VERTEX3D,
    STRING
};

struct LUATypeInfo {
    LUADataType type;
    std::string label;
};

// LUADataTypesMapping es un set fijo de claves precargado en estatico (ver .cpp), pero se
// consulta concurrentemente: ScriptLUA::AddDataType (invocado desde el constructor de ScriptLUA
// durante Object3DSerializer::ApplyJsonToObject) corre en un worker thread del ThreadPool cuando
// ThreadJobLoadObject::fnProcess deserializa un objeto con scripts, a la vez que el hilo principal
// llama InitEnvironment/ReloadEnvironment/ReloadGlobals/applyTypeToEnvironment sobre otros scripts
// ya en marcha. operator[] inserta si la clave no existe -- mutación concurrente de un std::map
// corrompe el árbol (mismo patrón que GLSLTypeMapping, ver ShaderCustomOGLCodeTypes.h).
// LUADataTypesMappingMutex + GetLUATypeInfo() serializan todo acceso; usar SIEMPRE estas dos en
// vez de tocar LUADataTypesMapping directamente.
extern std::map<std::string, LUATypeInfo> LUADataTypesMapping;
extern std::mutex LUADataTypesMappingMutex;

LUATypeInfo GetLUATypeInfo(const std::string& key);

struct ScriptLUATypeData {
    ScriptLUATypeData(const char *name, const char *type, LUADataValue value)
    : name(name), type(type), value(value)
    {
    }

    std::string name;
    std::string type;
    LUADataValue value;

    static std::string toString(LUADataValue value) {
        return std::visit([](const auto& arg) -> std::string {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, int>) {
                return std::to_string(arg);
            } else if constexpr (std::is_same_v<T, float>) {
                return std::to_string(arg);
            } else if constexpr (std::is_same_v<T, Vertex3D>) {
                return std::string("(x: " + std::to_string(arg.x) + ", y: " + std::to_string(arg.y) + ", z: " + std::to_string(arg.z));
            } else if constexpr (std::is_same_v<T, const char*>) {
                return "Es un string";
            } else {
                return "Unknown type";
            }
        }, value);
    }
};

struct ScriptMetaInfo {
    std::string name;
    std::string codeFile;
    std::string typesFile;
    int ticksPerSecond = 0;
    std::string information;
};

class Scene;

// Callback de un entorno de OBJETO (onUpdate / postUpdate) ya resuelto: se busca por nombre una vez y se reutiliza cada frame,
// igual que ya hacen los scripts globales con globalFuncCache. Lo guarda el Object3D que posee el entorno, y es ese objeto
// quien lo invalida (Object3D::invalidateScriptCallbacks) cuando algo redefine funciones EN SU entorno: el resto de la
// escena no se entera. Limitación: si un script reasigna su propio onUpdate en pleno juego, el cambio no se ve hasta la
// siguiente recarga del entorno.
//
// IMPORTANTE: solo guarda un lua_State* y un entero (referencia del registro de Lua), NO un sol::protected_function. Los tipos
// de sol dentro de Object3D no tienen el mismo tamaño en todas las unidades de compilación (depende de en qué orden se incluyan
// las macros SOL_*), y eso desplazaba los miembros de las subclases: el código de los bindings de Lua escribía en otro offset
// (heap corrupto en el RTS al abrir, 2026-10-14). Con tipos triviales el tamaño es el mismo en todas partes.
struct ScriptCallbackSlot
{
    lua_State *L = nullptr;
    int ref = LUA_NOREF;      // luaL_ref de la función en el registro; LUA_NOREF = no existe o aún sin resolver
    bool resolved = false;

    ScriptCallbackSlot() = default;
    // Copiar un objeto no copia la referencia: la copia empieza sin resolver
    ScriptCallbackSlot(const ScriptCallbackSlot &) {}
    ScriptCallbackSlot &operator=(const ScriptCallbackSlot &) { invalidate(); return *this; }
    ~ScriptCallbackSlot() { invalidate(); }

    void invalidate()
    {
        if (L != nullptr && ref != LUA_NOREF && ref != LUA_REFNIL) luaL_unref(L, LUA_REGISTRYINDEX, ref);
        L = nullptr;
        ref = LUA_NOREF;
        resolved = false;
    }
};

class ScriptLUA
{
    bool paused = false;
    bool globalLoaded = false;
    sol::environment globalEnvironment;
    std::unordered_map<std::string, sol::protected_function> globalFuncCache;
    int ticksPerSecond = 0;
    mutable float tickAccumulator = 0.0f;   // tiempo desde la última ejecución de onUpdate
    mutable float tickDelta = 0.0f;         // delta que ve el script en la ejecución actual
    mutable bool  tickedThisFrame = true;   // el último onUpdate se ejecutó (postUpdate le sigue)

    void ensureGlobalEnvironment();
    [[nodiscard]] bool ShouldTick(const std::string& func) const;

public:
    std::string content;
    std::vector<ScriptLUATypeData> dataTypes;
    std::vector<ScriptLUATypeData> dataTypesDefaultValues;

    std::string scriptFilename;
    std::string fileTypes;
    std::string name;
    std::string information;
    ScriptType type = SCRIPT_GLOBAL;
    Scene* scene = nullptr;

    explicit ScriptLUA(const std::string& name, const std::string& codeFile, const std::string &typesFile);
    ScriptLUA(const std::string& name, const std::string &codeFile, const std::string &typesFile, const cJSON *types);

    bool hasDataType(const char *name, const char *type) const;
    void RunEnvironment(sol::environment &environment, const std::string& func, std::optional<sol::object> arg = std::nullopt, ScriptCallbackSlot *slot = nullptr) const;
    void RunGlobal(const std::string& func);
    void AddDataType(const char *name, const char *type, cJSON *value);
    void ProcessFileTypes();
    void RemoveDataType(const ScriptLUATypeData& data);
    void UpdateFileTypes() const;
    void ReloadEnvironment(sol::environment &environment);
    void ReloadScriptCode();
    void getCode(const std::string &script);
    void InitEnvironment(sol::environment &environment);

    void UpdateScriptCodeWith(const std::string &content) const;
    void ReloadGlobals();
    void DrawImGuiProperties();
    void AddDataTypeEmpty(const char *name, const char *type);

    void applyTypeToEnvironment(const ScriptLUATypeData& type);

    sol::environment &getGlobalEnvironment()                                          { return globalEnvironment; }
    [[nodiscard]] ScriptType getType() const                                        { return type; }
    [[nodiscard]] bool isPaused() const                                             { return paused; }
    [[nodiscard]] const std::vector<ScriptLUATypeData> &getDataTypes() const        { return dataTypes; }
    [[nodiscard]] const std::string &getScriptFilename() const                      { return scriptFilename; }
    [[nodiscard]] const std::string &getTypesFile() const                           { return fileTypes; }
    [[nodiscard]] std::string getName() const                                       { return name; }
    [[nodiscard]] cJSON *getTypesJSON() const;

    static std::string dataTypesFileFor(std::string basicString);
    static std::string RemoveFilenameExtension(std::string& filename);

    void setName(const std::string &name);
    void setPaused(bool value);
    void setGlobalLoaded(bool value);
    void setDataTypesFromJSON(const cJSON *typesJSON);

    void setType(ScriptType value);
    [[nodiscard]] Scene* getScene() const          { return scene; }
    void setScene(Scene* s)                        { scene = s; }
    [[nodiscard]] int getTicksPerSecond() const    { return ticksPerSecond; }
    void setTicksPerSecond(int tps)                { ticksPerSecond = (tps < 0 ? 0 : tps); tickAccumulator = 0.0f; }
    // ¿Se ejecutó onUpdate en este frame? (sin límite de ticks: siempre, salvo en pausa). Para el profiler.
    [[nodiscard]] bool didTickThisFrame() const    { return !paused && (ticksPerSecond <= 0 || tickedThisFrame); }

    friend class ScriptLuaGUI;
};


#endif //BRAKEZA3D_SCRIPTLUA_H
