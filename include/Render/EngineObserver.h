#ifndef BRAKEZA3D_ENGINEOBSERVER_H
#define BRAKEZA3D_ENGINEOBSERVER_H

#include <string>
#include <deque>
#include <fstream>
#include <mutex>
#include "../Misc/cJSON.h"

// EngineObserver — machine-readable observability for AI agents
//
// Writes two files to the project root (ROOT_FOLDER):
//   brakeza_state.json   — current engine snapshot, updated every 60 frames
//   brakeza_events.jsonl — append-only JSON lines log (one event per line)
//
// Usage:
//   EngineObserver::init(Config::get()->ROOT_FOLDER);   // call once at startup
//   EngineObserver::appendEvent("info", "message");     // from Logging
//   EngineObserver::dumpState();                        // from main loop
//
class EngineObserver {
public:
    static inline int         frameCount   = 0;
    static inline std::string currentScene = "none";
    static inline std::string pipelineStep = "idle";

    // last 10 errors kept in memory for the state dump
    static inline std::deque<std::string> recentErrors;

    static inline std::string   statePath;
    static inline std::string   eventsPath;
    static inline std::ofstream eventsFile;
    static inline std::mutex    eventsMutex;

    // Call once after Config is ready
    static void init(const std::string &rootFolder) {
        statePath  = rootFolder + "brakeza_state.json";
        eventsPath = rootFolder + "brakeza_events.jsonl";
        // Truncate events file at each session start (fresh log)
        eventsFile.open(eventsPath, std::ios::trunc);
    }

    static void setScene(const std::string &name) {
        currentScene = name;
        appendEvent("scene", "Loaded: " + name);
    }

    static void setPipelineStep(const std::string &step) {
        pipelineStep = step;
    }

    // Append one JSON line to brakeza_events.jsonl
    // Thread-safe: called from Logging which can fire from worker threads
    static void appendEvent(const std::string &level, const std::string &msg) {
        std::lock_guard<std::mutex> lock(eventsMutex);
        if (!eventsFile.is_open()) return;

        cJSON *e = cJSON_CreateObject();
        cJSON_AddNumberToObject(e, "frame", frameCount);
        cJSON_AddStringToObject(e, "level", level.c_str());
        cJSON_AddStringToObject(e, "msg",   msg.c_str());
        char *s = cJSON_PrintUnformatted(e);
        eventsFile << s << "\n";
        eventsFile.flush();
        free(s);
        cJSON_Delete(e);

        if (level == "error") {
            recentErrors.push_back(msg);
            if (recentErrors.size() > 10)
                recentErrors.pop_front();
        }
    }
};

#endif //BRAKEZA3D_ENGINEOBSERVER_H
