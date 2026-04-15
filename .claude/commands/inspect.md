# Inspect engine runtime state

Read the three observability files written by EngineObserver while the engine is running.

## Steps

1. Read `brakeza_state.json` (project root) — current engine snapshot:
   - `frame` — current frame number
   - `scene` — active scene path
   - `pipeline_step` — which render step was active at last dump (idle/PreUpdate/OnUpdate+ObjectShaders/LightPass/PostUpdate/FlipBuffersToGlobal/PostProcessingChain/FlipToWindow)
   - `fps` — frames per second
   - `objects` — total scene objects
   - `lights` — total lights
   - `scene_loading` — is a scene being loaded async?
   - `recent_errors` — last 10 errors logged

2. Read the last 50 lines of `brakeza_events.jsonl` (project root) — JSON lines log:
   - Each line: `{"frame": N, "level": "info|warning|error", "msg": "..."}`
   - Look for errors, warnings, scene transitions, object additions

3. Read `brakeza_debug.png` (project root) if it exists — last manual screenshot taken via `Components.Render().MakeScreenShot("../brakeza_debug.png")` from Lua or from the editor toolbar.

## Report format
- Engine status: running/idle/loading
- Active scene and frame
- FPS
- Any errors in recent_errors or in the last 50 events
- What is visually on screen (from screenshot if available)
- Assessment: is the engine in a healthy state?

## Notes
- Files are written to the project root (`ROOT_FOLDER = "../"` relative to the build dir)
- `brakeza_state.json` is updated every 60 frames (~1 second at 60 FPS)
- `brakeza_events.jsonl` is appended every log call and truncated at session start
- If `brakeza_state.json` doesn't exist yet, the engine hasn't started or EngineObserver::init() hasn't been called
