#ifndef BRAKEZA3D_THREADJOBREADOBJECTSCRIPT_H
#define BRAKEZA3D_THREADJOBREADOBJECTSCRIPT_H

#include "ThreadJobBase.h"
#include "../3D/Object3D.h"
#include "../Misc/ScriptLUA.h"
#include "../Misc/Tools.h"
#include "../GUI/Objects/ScriptLuaGUI.h"

// Carga un script de objeto en segundo plano (worker: I/O de ficheros)
// y lo aplica al objeto en el callback de hilo principal (antes de que corra Lua).
// Usar desde Lua via obj:AttachScriptAsync("path/to/Script.json")
class ThreadJobReadObjectScript : public ThreadJobBase
{
    Object3D*   obj    = nullptr;
    std::string jsonPath;
    ScriptLUA*  script = nullptr;

public:
    ThreadJobReadObjectScript(Object3D* obj, const std::string& jsonPath)
    :
        obj(obj),
        jsonPath(jsonPath)
    {
        function = [this](){ fnProcess(); };
        callback = [this](){ fnCallback(); };
    }

    void fnProcess()
    {
        if (!Tools::FileExists(jsonPath.c_str())) {
            LOG_ERROR("[ThreadJobReadObjectScript] File not found: %s", jsonPath.c_str());
            return;
        }

        auto meta = ScriptLuaGUI::ExtractScriptMetainfo(jsonPath);

        if (!Tools::FileExists(meta.codeFile.c_str()) || !Tools::FileExists(meta.typesFile.c_str())) {
            LOG_ERROR("[ThreadJobReadObjectScript] Missing code/types files for '%s'", jsonPath.c_str());
            return;
        }

        script = new ScriptLUA(meta.name, meta.codeFile, meta.typesFile);

        LOG_MESSAGE("[ThreadJobReadObjectScript] Process END: %s", jsonPath.c_str());
    }

    void fnCallback()
    {
        if (!script || !obj) {
            LOG_ERROR("[ThreadJobReadObjectScript] Callback: null script or object, skipping.");
            return;
        }

        // fnCallback corre en hilo principal ANTES de OnUpdateComponents (antes de Lua).
        // AttachAndInitScript es seguro aquí: sin reentrancia Lua.
        obj->AttachAndInitScript(script);
        script = nullptr;

        LOG_MESSAGE("[ThreadJobReadObjectScript] Callback END: script attached to '%s'", obj->getName().c_str());
    }
};

#endif //BRAKEZA3D_THREADJOBREADOBJECTSCRIPT_H
