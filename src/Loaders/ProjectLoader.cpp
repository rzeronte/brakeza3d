//
// Created by Eduardo on 28/01/2025.
//

#include "../../include/Loaders/ProjectLoader.h"
#include "../../include/Brakeza.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Misc/Logging.h"
#include "../../include/Components/Components.h"
#include "../../include/GUI/Objects/FileSystemGUI.h"
#include "../../include/Misc/ToolsJSON.h"
#include <set>

std::vector<ProjectLoader::SkippedProjectScript> ProjectLoader::skippedProjectScripts;

// Un --set key=value "activa" un script condicionado si llega con un valor distinto de vacío,
// "false" y "0" (bool true, ciclos > 0...). Se mira rawParams directamente: los scripts se leen
// antes que cli_params.
bool ProjectLoader::IsCliParamActive(const std::string &param)
{
    const auto &rawParams = Brakeza::get()->getCliOptions().rawParams;
    auto it = rawParams.find(param);
    if (it == rawParams.end()) return false;
    const std::string &v = it->second;
    return !v.empty() && v != "false" && v != "0";
}

void ProjectLoader::LoadProject(const FilePath::ProjectFile &filename)
{
    auto scripting = Components::get()->Scripting();

    char* contentFile = Tools::ReadFile(filename);
    auto contentJSON = cJSON_Parse(contentFile);
    free(contentFile);

    // thread pools — resize antes de ClearWorld, pools aún sin tareas nuevas
    if (cJSON_GetObjectItemCaseSensitive(contentJSON, "thread_pools") != nullptr) {
        auto poolsJSON = cJSON_GetObjectItemCaseSensitive(contentJSON, "thread_pools");
        auto resizePool = [](ThreadPool &pool, cJSON *j) {
            if (!j) return;
            cJSON *item = cJSON_GetObjectItemCaseSensitive(j, "num_threads");
            if (item && cJSON_IsNumber(item) && (size_t)item->valueint != pool.getNumThreads()) {
                pool.resize((size_t)item->valueint);
            }
        };
        resizePool(Brakeza::get()->PoolCompute(), cJSON_GetObjectItemCaseSensitive(poolsJSON, "compute"));
        resizePool(Brakeza::get()->PoolImages(),  cJSON_GetObjectItemCaseSensitive(poolsJSON, "images"));
    }

    if (scripting->getCurrentScene() != nullptr) {
        SceneLoader::ClearWorld();
    }

    RemoveProjectScripts();
    RemoveProjectScenes();

    Components::get()->Scripting()->setCurrentProject(nullptr);
    Components::get()->Scripting()->setCurrentScene(nullptr);

    LOG_MESSAGE("[ProjectLoader] Loading project file '%s'...", filename.c_str());

    if (cJSON_GetObjectItemCaseSensitive(contentJSON, "name") != nullptr) {
        auto sceneName = cJSON_GetObjectItemCaseSensitive(contentJSON, "name")->valuestring;
        Components::get()->Window()->setWindowTitle(sceneName);
        Config::get()->ENGINE_TITLE = sceneName;
    }

    // sceneScripts. Una entrada con "onlyWithCliParam": "<param>" solo se carga si ese parámetro
    // llega activo por --set (scripts de test/escenarios automáticos): en una partida normal ni se
    // compila ni corre.
    skippedProjectScripts.clear();
    if (cJSON_GetObjectItemCaseSensitive(contentJSON, "scripts") != nullptr) {
        cJSON *currentScript;
        cJSON_ArrayForEach(currentScript, cJSON_GetObjectItemCaseSensitive(contentJSON, "scripts")) {
            std::string name = cJSON_GetObjectItemCaseSensitive(currentScript, "name")->valuestring;
            std::string codeScript = cJSON_GetObjectItemCaseSensitive(currentScript, "codeFile")->valuestring;
            std::string typesFile = cJSON_GetObjectItemCaseSensitive(currentScript, "typesFile")->valuestring;
            cJSON *condItem = cJSON_GetObjectItemCaseSensitive(currentScript, "onlyWithCliParam");
            if (condItem && cJSON_IsString(condItem) && !IsCliParamActive(condItem->valuestring)) {
                skippedProjectScripts.push_back({ name, codeScript, typesFile, condItem->valuestring });
                LOG_MESSAGE("[ProjectLoader] Script '%s' not loaded (only with --set %s)", name.c_str(), condItem->valuestring);
                continue;
            }
            Components::get()->Scripting()->AddProjectLUAScript(new ScriptLUA(name, codeScript, typesFile));
        }
    }

    // scenes
    if (cJSON_GetObjectItemCaseSensitive(contentJSON, "scenes") != nullptr) {
        cJSON *currentScene;
        cJSON_ArrayForEach(currentScene, cJSON_GetObjectItemCaseSensitive(contentJSON, "scenes")) {
            std::string path = cJSON_GetObjectItemCaseSensitive(currentScene, "path")->valuestring;
            Components::get()->Scripting()->AddProjectScene(path);
        }
    }

    // RESOLUTION
    if (cJSON_GetObjectItemCaseSensitive(contentJSON, "resolution") != nullptr) {
        auto resolutionJSON = cJSON_GetObjectItemCaseSensitive(contentJSON, "resolution");
        const int rw = cJSON_GetObjectItemCaseSensitive(resolutionJSON, "width")->valueint;
        const int rh = cJSON_GetObjectItemCaseSensitive(resolutionJSON, "height")->valueint;

        // "locked": true -> resolución de render FIJA para este proyecto: setRendererSize() la marca
        // como personalizada (al redimensionar la ventana solo se adaptan ventana y UI) y recrea ya
        // los buffers a ese tamaño -- se crearon al arrancar la ventana, antes de cargar el proyecto.
        // Sin "locked" (por defecto): tamaño inicial que sigue a la ventana, como siempre.
        const cJSON* lockedJSON = cJSON_GetObjectItemCaseSensitive(resolutionJSON, "locked");
        if (cJSON_IsTrue(lockedJSON)) {
            Components::get()->Window()->setRendererSize(rw, rh);
        } else {
            Components::get()->Window()->setInitialRendererSize(rw, rh);
        }
    }

    // GRAVITY
    if (cJSON_GetObjectItemCaseSensitive(contentJSON, "gravity") != nullptr) {
        auto gravity = ToolsJSON::getVertex3DByJSON(cJSON_GetObjectItemCaseSensitive(contentJSON, "gravity"));
        Config::get()->gravity = gravity;
        Components::get()->Collisions()->setGravity(gravity);
    }

    // volumes
    if (cJSON_GetObjectItemCaseSensitive(contentJSON, "sound") != nullptr) {
        auto soundJSON = cJSON_GetObjectItemCaseSensitive(contentJSON, "sound");
        Config::get()->SOUND_VOLUME_MUSIC = cJSON_GetObjectItemCaseSensitive(soundJSON, "volume_music")->valuedouble;
        Config::get()->SOUND_VOLUME_FX = cJSON_GetObjectItemCaseSensitive(soundJSON, "volume_fx")->valuedouble;
        Components::get()->Sound()->setSoundsVolume((int) Config::get()->SOUND_VOLUME_FX);
        Components::get()->Sound()->setMusicVolume((int) Config::get()->SOUND_VOLUME_MUSIC);
    }

    // thread pools
    if (cJSON_GetObjectItemCaseSensitive(contentJSON, "thread_pools") != nullptr) {
        auto poolsJSON = cJSON_GetObjectItemCaseSensitive(contentJSON, "thread_pools");

        auto applyPool = [](ThreadPool &pool, cJSON *j, const char *poolName) {
            if (!j) return;
            auto read = [&](const char *key, size_t current) -> size_t {
                cJSON *item = cJSON_GetObjectItemCaseSensitive(j, key);
                return (item && cJSON_IsNumber(item)) ? (size_t)item->valueint : current;
            };
            pool.setMaxConcurrentTasks(read("max_concurrent_tasks",      pool.getMaxConcurrentTasks()));
            pool.setMaxCallbacksPerFrame(read("max_callbacks_per_frame", pool.getMaxCallbacksPerFrame()));
            pool.setMaxEnqueuedTasks(read("max_enqueued_tasks",          pool.getMaxEnqueuedTasks()));
            pool.setMaxEnqueuedCallbacks(read("max_enqueued_callbacks",  pool.getMaxEnqueuedCallbacks()));
            LOG_SUCCESS("[ProjectLoader] ThreadPool '%s' => threads=%zu | concurrent=%zu | callbacks/frame=%zu | enqueued tasks=%zu | enqueued callbacks=%zu",
                poolName,
                pool.getNumThreads(),
                pool.getMaxConcurrentTasks(),
                pool.getMaxCallbacksPerFrame(),
                pool.getMaxEnqueuedTasks(),
                pool.getMaxEnqueuedCallbacks()
            );
        };

        applyPool(Brakeza::get()->PoolCompute(), cJSON_GetObjectItemCaseSensitive(poolsJSON, "compute"), "compute");
        applyPool(Brakeza::get()->PoolImages(),  cJSON_GetObjectItemCaseSensitive(poolsJSON, "images"),  "images");
    }

    // cli_params — parámetros que ESTE proyecto declara aceptar por línea de comandos
    // (--set key=value, ver Brakeza::ReadArgs). El motor solo lee nombre/tipo/default aquí;
    // qué significa cada uno (p.ej. "scenario") vive por completo en el Lua del proyecto
    // (ver AutoRun.lua en el RTS), nunca en este fichero.
    Config::get()->cliParams.clear();
    if (cJSON_GetObjectItemCaseSensitive(contentJSON, "cli_params") != nullptr) {
        const auto &rawParams = Brakeza::get()->getCliOptions().rawParams;
        std::set<std::string> declaredNames;

        cJSON *paramJSON;
        cJSON_ArrayForEach(paramJSON, cJSON_GetObjectItemCaseSensitive(contentJSON, "cli_params")) {
            cJSON *nameItem = cJSON_GetObjectItemCaseSensitive(paramJSON, "name");
            cJSON *typeItem = cJSON_GetObjectItemCaseSensitive(paramJSON, "type");
            if (!nameItem || !cJSON_IsString(nameItem)) continue;

            std::string name = nameItem->valuestring;
            std::string type = (typeItem && cJSON_IsString(typeItem)) ? typeItem->valuestring : "string";
            declaredNames.insert(name);

            cJSON *defItem = cJSON_GetObjectItemCaseSensitive(paramJSON, "default");
            auto rawIt = rawParams.find(name);
            bool provided = rawIt != rawParams.end();

            Config::CliParamValue value;
            if (type == "bool") {
                value.type = Config::CliParamValue::Type::Bool;
                value.b = provided ? (rawIt->second == "true" || rawIt->second == "1")
                                    : (defItem && cJSON_IsBool(defItem) && cJSON_IsTrue(defItem));
            } else if (type == "number") {
                value.type = Config::CliParamValue::Type::Number;
                value.n = provided ? atof(rawIt->second.c_str())
                                    : (defItem && cJSON_IsNumber(defItem) ? defItem->valuedouble : 0.0);
            } else {
                value.type = Config::CliParamValue::Type::String;
                value.s = provided ? rawIt->second
                                    : (defItem && cJSON_IsString(defItem) ? defItem->valuestring : "");
            }

            Config::get()->cliParams[name] = value;
            if (provided) {
                LOG_MESSAGE("[ProjectLoader] cli_param '%s' = '%s' (via --set)", name.c_str(), rawIt->second.c_str());
            }
        }

        // Aviso (no bloqueante) si llegó un --set que este proyecto no declara -- suele ser un typo.
        for (auto &kv : rawParams) {
            if (!declaredNames.count(kv.first)) {
                LOG_WARNING("[ProjectLoader] --set '%s' no está declarado en cli_params de este proyecto, se ignora", kv.first.c_str());
            }
        }
    }

    Components::get()->Scripting()->setCurrentProject(new Project(filename));
    FileSystemGUI::autoExpandProject = true;

    cJSON_Delete(contentJSON);
}

void ProjectLoader::SaveProject(const FilePath::ProjectFile &filename)
{
    cJSON *root = cJSON_CreateObject();

    auto project = Components::get()->Scripting()->getCurrentProject();
    std::string projectName = project != nullptr ? project->getName() : Config::get()->ENGINE_TITLE;
    std::string projectDesc = project != nullptr ? project->getDescription() : "";

    cJSON_AddStringToObject(root, "name", projectName.c_str());
    cJSON_AddStringToObject(root, "description", projectDesc.c_str());
    cJSON_AddItemToObject(root, "gravity", ToolsJSON::Vertex3DToJSON(Config::get()->gravity));

    // scripts
    cJSON *sceneScriptsArray = cJSON_CreateArray();
    for (auto &script : Components::get()->Scripting()->getProjectScripts()) {
        cJSON *scriptSceneSON = cJSON_CreateObject();
        cJSON_AddStringToObject(scriptSceneSON, "name", script->getScriptFilename().c_str());
        cJSON_AddItemToArray(sceneScriptsArray, scriptSceneSON);
    }
    // Los condicionados que no se cargaron (onlyWithCliParam) se conservan tal cual
    for (auto &s : skippedProjectScripts) {
        cJSON *scriptJSON = cJSON_CreateObject();
        cJSON_AddStringToObject(scriptJSON, "name", s.name.c_str());
        cJSON_AddStringToObject(scriptJSON, "codeFile", s.codeFile.c_str());
        cJSON_AddStringToObject(scriptJSON, "typesFile", s.typesFile.c_str());
        cJSON_AddStringToObject(scriptJSON, "onlyWithCliParam", s.cliParam.c_str());
        cJSON_AddItemToArray(sceneScriptsArray, scriptJSON);
    }
    cJSON_AddItemToObject(root, "scripts", sceneScriptsArray);

    // scenes
    cJSON *scenesArray = cJSON_CreateArray();
    for (auto &scenePath : Components::get()->Scripting()->getProjectScenes()) {
        cJSON *sceneJSON = cJSON_CreateObject();
        cJSON_AddStringToObject(sceneJSON, "path", scenePath.c_str());
        cJSON_AddItemToArray(scenesArray, sceneJSON);
    }
    cJSON_AddItemToObject(root, "scenes", scenesArray);

    // sound
    cJSON *soundJSON = cJSON_CreateObject();
    cJSON_AddNumberToObject(soundJSON, "volume_fx", Config::get()->SOUND_VOLUME_FX);
    cJSON_AddNumberToObject(soundJSON, "volume_music", Config::get()->SOUND_VOLUME_MUSIC);
    cJSON_AddItemToObject(root, "sound", soundJSON);

    // render resolution
    auto window = Components::get()->Window();
    cJSON *resolutionJSON = cJSON_CreateObject();
    cJSON_AddNumberToObject(resolutionJSON, "width", window->getWidthRender());
    cJSON_AddNumberToObject(resolutionJSON, "height", window->getHeightRender());
    if (window->isRenderResolutionLocked()) cJSON_AddBoolToObject(resolutionJSON, "locked", true);
    cJSON_AddItemToObject(root, "resolution", resolutionJSON);

    // thread pools
    auto serializePool = [](const ThreadPool &p) -> cJSON* {
        cJSON *j = cJSON_CreateObject();
        cJSON_AddNumberToObject(j, "num_threads",             (int)p.getNumThreads());
        cJSON_AddNumberToObject(j, "max_concurrent_tasks",    (int)p.getMaxConcurrentTasks());
        cJSON_AddNumberToObject(j, "max_callbacks_per_frame", (int)p.getMaxCallbacksPerFrame());
        cJSON_AddNumberToObject(j, "max_enqueued_tasks",      (int)p.getMaxEnqueuedTasks());
        cJSON_AddNumberToObject(j, "max_enqueued_callbacks",  (int)p.getMaxEnqueuedCallbacks());
        return j;
    };
    cJSON *threadPoolsJSON = cJSON_CreateObject();
    cJSON_AddItemToObject(threadPoolsJSON, "compute", serializePool(Brakeza::get()->PoolCompute()));
    cJSON_AddItemToObject(threadPoolsJSON, "images",  serializePool(Brakeza::get()->PoolImages()));
    cJSON_AddItemToObject(root, "thread_pools", threadPoolsJSON);

    Tools::WriteToFile(filename, cJSON_Print(root));
}

void ProjectLoader::RemoveProjectScenes()
{
    auto scripting = Components::get()->Scripting();
    for (auto &o: scripting->getProjectScenes()) {
        scripting->RemoveProjectScene(o);
    }
}

void ProjectLoader::RemoveProjectScripts()
{
    auto scripting = Components::get()->Scripting();
    for (auto &o: scripting->getProjectScripts()) {
        scripting->RemoveProjectScript(o);
    }
}

void ProjectLoader::CloseCurrentProject()
{
    LOG_MESSAGE("[ProjectLoader] Closing current project...");

    auto scripting = Components::get()->Scripting();

    if (scripting->getCurrentScene() != nullptr) {
        SceneLoader::ClearWorld();
    }

    scripting->StopLUAScripts();
    auto projectScripts = scripting->getProjectScripts();
    for (auto *s : projectScripts) {
        scripting->RemoveProjectScript(s);
    }

    scripting->setCurrentProject(nullptr);
    FileSystemGUI::autoExpandProject = false;
}

void ProjectLoader::CreateProject(const FilePath::ProjectFile &filename)
{
    LOG_MESSAGE("Creating new project file: %s", filename.c_str());

    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "name", filename.c_str());

    auto projectJsonFile = std::string(filename + ".json");

    Tools::WriteToFile(projectJsonFile, cJSON_Print(root));
}

void ProjectLoader::RemoveProject(const FilePath::ProjectFile &filename)
{
    LOG_MESSAGE("Deleting project: %s", filename.c_str());

    Tools::RemoveFile(filename);
}


