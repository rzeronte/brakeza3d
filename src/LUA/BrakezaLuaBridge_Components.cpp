#include "../../include/LUA/BrakezaLuaBridge.h"

void LUAIntegrationComponents(sol::state &lua)
{
    lua.new_usertype<Component>("Component",
        "setEnabled", &Component::setEnabled,
        "isEnabled", &Component::isEnabled
    );

    lua.new_usertype<ComponentCamera>("ComponentCamera",
        sol::base_classes, sol::bases<Component>(),
        "getCamera", &ComponentCamera::getCamera,
        "isEnabled", &ComponentCamera::isEnabled,
        "setEnabled", &ComponentCamera::setEnabled,
        "getGLMMat4ViewMatrix", &ComponentCamera::getGLMMat4ViewMatrix,
        "getGLMMat4ProjectionMatrix", &ComponentCamera::getGLMMat4ProjectionMatrix,
        "worldToScreen", &ComponentCamera::worldToScreen,
        // Lo mismo con números sueltos: devuelve sx, sy, z (z > 0 = delante de la cámara) sin crear
        // un Vertex3D de entrada ni un vec3 de salida. Para bucles que proyectan cientos de puntos
        // por frame (avatares del HUD): cada userdata creado costaba más que la propia proyección.
        "worldToScreenXYZ", [](const ComponentCamera& c, float x, float y, float z, int w, int h) -> std::tuple<float, float, float> {
            const glm::vec3 s = c.worldToScreen(Vertex3D(x, y, z), w, h);
            return {s.x, s.y, s.z};
        }
    );

    lua.new_usertype<ComponentSound>("ComponentSound",
    sol::base_classes, sol::bases<Component>(),
        "AddSound", &ComponentSound::AddSound,
        "AddMusic", &ComponentSound::AddMusic,
        "setMusicVolume", &ComponentSound::setMusicVolume,
        "setSoundsVolume", &ComponentSound::setSoundsVolume,
        "setAmbienceVolume", [](ComponentSound*, int percent) { Sound3D::ambienceVolumeScale = percent / 100.0f; },
        "setAmbiencePanning", [](ComponentSound*, bool enabled) { Sound3D::panningEnabled = enabled; },
        "isAmbiencePanning", [](ComponentSound*) { return Sound3D::panningEnabled; },
        "PlaySound", &ComponentSound::PlaySound,
        "PlayMusic", &ComponentSound::PlayMusic,
        "StopMusic", &ComponentSound::StopMusic,
        "PauseMusic", &ComponentSound::PauseMusic,
        "ResumeMusic", &ComponentSound::ResumeMusic,
        "isMusicPaused", &ComponentSound::isMusicPaused,
        "StopChannel", &ComponentSound::StopChannel,
        "getSoundDuration", &ComponentSound::getSoundDuration,
        "LoadSoundsFromFile", &ComponentSound::LoadSoundsFromFile,
        "setChannelFrequency", &ComponentSound::setChannelFrequency,
        "setChannelPitch", &ComponentSound::setChannelPitch,
        "setChannelVolume", &ComponentSound::setChannelVolume,
        "setChannelPosition", &ComponentSound::setChannelPosition,
        "isChannelPlaying", &ComponentSound::isChannelPlaying,
        "isSoundPlaying", [](ComponentSound* s, std::string label) { return s->isSoundPlaying(label); }
    );

    lua.new_usertype<ComponentCollisions>("ComponentCollisions",
    sol::base_classes, sol::bases<Component>(),
        "isRayCollisionWith", &ComponentCollisions::isRayCollisionWith,
        // hit, hitPoint, hitObject = Collisions:rayTest(from, to, group, mask)
        "rayTest", [](const ComponentCollisions& c, const Vertex3D& from, const Vertex3D& to, int group, int mask) -> std::tuple<bool, Vertex3D, Object3D*> {
            Vertex3D hitPoint;
            Object3D* hitObject = nullptr;
            bool hit = c.rayTest(from, to, group, mask, hitPoint, hitObject);
            return { hit, hitPoint, hitObject };
        },
        // Solo contra el mundo estático (ciudad, suelo, muros: grupo StaticWorld, que solo acepta
        // proyectiles en su máscara -- por eso el rayo va como Projectile). Las unidades no lo tapan.
        "rayTestStatic", [](const ComponentCollisions& c, const Vertex3D& from, const Vertex3D& to) -> std::tuple<bool, Vertex3D, Object3D*> {
            Vertex3D hitPoint;
            Object3D* hitObject = nullptr;
            bool hit = c.rayTest(from, to, Config::Projectile, Config::StaticWorld, hitPoint, hitObject);
            return { hit, hitPoint, hitObject };
        },
        "setEnableDebugMode", &ComponentCollisions::setEnableDebugMode,
        "setEnabled", &ComponentCollisions::setEnabled
    );

    lua.new_usertype<ComponentWindow>("ComponentWindow",
    sol::base_classes, sol::bases<Component>(),
        "setWindowTitle", &ComponentWindow::setWindowTitle,
        "ToggleFullScreen", &ComponentWindow::ToggleFullScreen,
        "getWidth", &ComponentWindow::getWidth,
        "getHeight", &ComponentWindow::getHeight,
        "getHeightRender", &ComponentWindow::getHeightRender,
        "getWidthRender", &ComponentWindow::getWidthRender,
        "isWindowMaximized", &ComponentWindow::isWindowMaximized,
        "LoadCursorImage", &ComponentWindow::LoadCursorImage,
        "scheduleHoverPick", [](ComponentWindow& w, int mx, int my) {
            int nx = (int)((float)mx / w.getWidth()  * w.getWidthRender());
            int ny = (int)((float)my / w.getHeight() * w.getHeightRender());
            w.scheduleHoverPick(nx, ny);
        },
        "getHoverPickID", [](const ComponentWindow& w) -> unsigned int {
            return w.getHoverPickID();
        },
        "setImGuiMouse", &ComponentWindow::setImGuiMouse,
        "setClearColor", &ComponentWindow::setClearColor
    );

    lua.new_usertype<ComponentRender>("ComponentRender",
    sol::base_classes, sol::bases<Component>(),
        "getSceneLoader", &ComponentRender::getSceneLoader,
        "DrawLine",    &ComponentRender::DrawLine,
        "DrawLine2D",     &ComponentRender::DrawLine2D,
        "DrawFilledRect", &ComponentRender::DrawFilledRect,
        "DrawFilledRectToFB", &ComponentRender::DrawFilledRectToFB,
        "DrawCircle2D",   &ComponentRender::DrawCircle2D,
        "DrawCircle2DToFB", &ComponentRender::DrawCircle2DToFB,
        "DrawImage2D",    &ComponentRender::DrawImage2D,
        "DrawImage2DToFB", &ComponentRender::DrawImage2DToFB,
        "DrawImage2DFromImage", &ComponentRender::DrawImage2DFromImage,
        // Como DrawImage2DFromImage pero en una capa concreta ("ui", "foreground"...). Lambda con
        // alpha opcional: sol2 no aplica los argumentos por defecto de C++ (el minimapa dibuja su
        // niebla en "ui", encima de la barra inferior).
        "DrawImage2DFromImageToFB", [](ComponentRender& r, Image* img, int x, int y, int w, int h,
                                       const std::string& fb, sol::optional<float> alpha) {
            r.DrawImage2DFromImageToFB(img, x, y, w, h, fb, alpha.value_or(1.0f));
        },
        // Fuerza la carga+subida a GPU de una imagen sin dibujarla, para "calentar" el cache
        // (ImageCache::getOrLoad es síncrono -- ver ComponentRender::getOrLoadImage) ANTES de que
        // haga falta mostrarla de golpe (p.ej. fondos de pantalla de carga: precargarlos en el
        // menú principal evita el tirón la primera vez que cada uno se dibuja de verdad).
        "PreloadImage2D", [](ComponentRender* r, const std::string& path) {
            r->getOrLoadImage(path);
        },
        "setGlobalIlluminationDirection", &ComponentRender::setGlobalIlluminationDirection,
        "setGlobalIlluminationAmbient", &ComponentRender::setGlobalIlluminationAmbient,
        "setGlobalIlluminationDiffuse", &ComponentRender::setGlobalIlluminationDiffuse,
        "setGlobalIlluminationSpecular", &ComponentRender::setGlobalIlluminationSpecular,
        "getGlobalIlluminationDirection", &ComponentRender::getGlobalIlluminationDirection,
        "getGlobalIlluminationAmbient", &ComponentRender::getGlobalIlluminationAmbient,
        "getGlobalIlluminationDiffuse", &ComponentRender::getGlobalIlluminationDiffuse,
        "getGlobalIlluminationSpecular", &ComponentRender::getGlobalIlluminationSpecular,
        // Shader de escena por etiqueta. Un shader de código se devuelve como ShaderOpenGLCustom (con sus
        // uniforms); cualquier otro (p. ej. de nodos) como ShaderCustom, que solo permite activarlo/desactivarlo.
        "getSceneShaderByLabel", [](ComponentRender& r, const std::string& label, sol::this_state ts) -> sol::object {
            ShaderBaseCustom* shader = r.getSceneShaderByLabel(label);
            if (shader == nullptr) return sol::make_object(ts, sol::lua_nil);
            if (auto* codeShader = dynamic_cast<ShaderBaseCustomOGLCode*>(shader)) return sol::make_object(ts, codeShader);
            return sol::make_object(ts, shader);
        },
        "clearSceneShaders", &ComponentRender::clearSceneShaders,
        "getFps", &ComponentRender::getFps,
        "MakeScreenShot", &ComponentRender::MakeScreenShot,
        // Single-selection (returns nil when 0 or >1 selected)
        "getSelectedObject", &ComponentRender::getSelectedObject,
        // Multi-selection API
        "getSelectedObjects", [](ComponentRender* cr) -> std::vector<Object3D*> {
            return cr->getSelectedObjects();
        },
        "addToSelection", &ComponentRender::addToSelection,
        "removeFromSelection", [](ComponentRender* cr, Object3D* o) {
            cr->removeFromSelection(o);
        },
        "clearSelection", &ComponentRender::clearSelection,
        "setDragSelectEnabled", [](ComponentRender* cr, bool v) { cr->getSelectionManager().setRectSelectEnabled(v); },
        "setSelectedObject", &ComponentRender::setSelectedObject,
        "hasMultipleSelected", &ComponentRender::hasMultipleSelected,
        "isObjectInSelection", [](ComponentRender* cr, Object3D* o) -> bool {
            return cr->isObjectInSelection(o);
        },
        "getLastRightClickedObject", &ComponentRender::getLastRightClickedObject,
        "clearRightClickedObject", &ComponentRender::clearRightClickedObject,
        "getLastRightClickedSubmeshName", &ComponentRender::getLastRightClickedSubmeshName,
        "getLastLeftClickedObject", &ComponentRender::getLastLeftClickedObject,
        "clearLeftClickedObject", &ComponentRender::clearLeftClickedObject,
        "getLastLeftClickedSubmeshName", &ComponentRender::getLastLeftClickedSubmeshName,
        "drawGroundCircle",  sol::overload(
            static_cast<void(ComponentRender::*)(Object3D*,float,float,float,float,float)const>(&ComponentRender::drawGroundCircle),
            static_cast<void(ComponentRender::*)(Object3D*,float,float,float,float,float,float)const>(&ComponentRender::drawGroundCircle),
            &ComponentRender::drawGroundCircleToFB
        ),
        "drawGroundBlob",    sol::overload(&ComponentRender::drawGroundBlob,    &ComponentRender::drawGroundBlobToFB),
        "drawGroundDecal",   sol::overload(&ComponentRender::drawGroundDecal,   &ComponentRender::drawGroundDecalToFB),
        "drawOutlineSubmesh",      &ComponentRender::drawOutlineSubmesh,
        "drawFillSubmesh",         &ComponentRender::drawFillSubmesh,
        "clearOutlineBatch",       &ComponentRender::clearOutlineBatch,
        "drawOutlineSubmeshBatch", &ComponentRender::drawOutlineSubmeshBatch,
        "flushOutlines",           &ComponentRender::flushOutlines,
        "setUIBatching",           &ComponentRender::setUIBatching,
        "setUIQuadBatching",       &ComponentRender::setUIQuadBatching,
        "setShadowMergeEnabled",   &ComponentRender::setShadowMergeEnabled,
        "isUIBatching",            &ComponentRender::isUIBatching,
        // getSubmeshCenter recibe un Object3D* desde Lua igual que los 6 setters de arriba --
        // mismo riesgo de puntero colgante tras un reinicio de partida, mismo fix:
        // isValidObjectPointer() en vez de (solo) obj==nullptr.
        "getSubmeshCenter", [](ComponentRender& self, Object3D* obj, const std::string& submeshName) -> Vertex3D {
            if (!Brakeza::get()->isValidObjectPointer(obj)) return Vertex3D::zero();
            return self.getSubmeshCenter(obj, submeshName);
        },
        "getSubmeshTop", [](ComponentRender& self, Object3D* obj, const std::string& submeshName) -> Vertex3D {
            if (!Brakeza::get()->isValidObjectPointer(obj)) return Vertex3D::zero();
            return self.getSubmeshTop(obj, submeshName);
        },
        "DrawCircle3D", &ComponentRender::DrawCircle3D,
        "drawAxisQuad",    &ComponentRender::drawAxisQuad,
        "getTextWriter",   &ComponentRender::getTextWriter,
        "drawWidget", [](ComponentRender& r, const std::string& name, sol::table data, sol::object fbArg, sol::object scaleArg) -> std::tuple<float, std::string, std::string, std::string> {
            if (!r.getUIManager()) return {0.0f, "", "", ""};
            std::string fb = (fbArg.valid() && fbArg.get_type() == sol::type::string)
                             ? fbArg.as<std::string>() : "foreground";
            float scaleOverride = (scaleArg.valid() && scaleArg.get_type() == sol::type::number)
                             ? scaleArg.as<float>() : 0.0f;
            return r.getUIManager()->drawWidgetLua(name, data, fb, scaleOverride);
        },
        "drawWidgetAtPos", [](ComponentRender& r, const std::string& name, float x, float y, sol::table data, sol::object fbArg, sol::object scaleArg) -> std::tuple<float, std::string, std::string, std::string> {
            if (!r.getUIManager()) return {y, "", "", ""};
            std::string fb = (fbArg.valid() && fbArg.get_type() == sol::type::string)
                             ? fbArg.as<std::string>() : "foreground";
            float scaleOverride = (scaleArg.valid() && scaleArg.get_type() == sol::type::number)
                             ? scaleArg.as<float>() : 0.0f;
            return r.getUIManager()->drawWidgetAtPosLua(name, x, y, data, fb, scaleOverride);
        },
        // N copias de un widget en una llamada (todas al mismo lote de UI): instances = {{x=, y=,
        // alpha=?, data={...}}, ...}. Con "cache": "variants" cada copia es un quad del atlas + sus
        // elementos "live" (ver el bloque CACHÉ DE WIDGETS de UIManager.h). Devuelve índices 1-based
        // (0 = ninguno): clickedIdx, clickedId, rightClickedIdx, rightClickedId, hoveredIdx, hoveredId.
        "drawWidgetInstances", [](ComponentRender& r, const std::string& name, sol::table instances, sol::object fbArg, sol::object scaleArg)
                -> std::tuple<int, std::string, int, std::string, int, std::string> {
            if (!r.getUIManager()) return {0, "", 0, "", 0, ""};
            std::string fb = (fbArg.valid() && fbArg.get_type() == sol::type::string)
                             ? fbArg.as<std::string>() : "foreground";
            float scaleOverride = (scaleArg.valid() && scaleArg.get_type() == sol::type::number)
                             ? scaleArg.as<float>() : 0.0f;
            return r.getUIManager()->drawWidgetInstancesLua(name, instances, fb, scaleOverride);
        },
        // Tamaño declarado de un widget (fracciones de ventana, 0 si no existe): para colocarlo
        // desde Lua (p.ej. centrarlo) respetando lo que se edite en el UI Manager.
        "getWidgetSize", [](ComponentRender& r, const std::string& name) -> std::tuple<float, float> {
            if (!r.getUIManager()) return {0.0f, 0.0f};
            auto& ws = r.getUIManager()->getWidgets();
            auto it = ws.find(name);
            if (it == ws.end()) return {0.0f, 0.0f};
            return {it->second.width, it->second.height};
        },
        // Forzar el re-render de la caché FBO de un widget (o de todos) en el siguiente draw: para
        // cambios de contexto (p.ej. entrar/salir del modo adquisición) tras los que se vería la
        // imagen cacheada anterior. Solo tiene efecto en widgets cacheables de primer nivel.
        "invalidateWidget", [](ComponentRender& r, const std::string& name) -> bool {
            return r.getUIManager() && r.getUIManager()->invalidateWidget(name);
        },
        "invalidateAllWidgets", [](ComponentRender& r) {
            if (r.getUIManager()) r.getUIManager()->invalidateAllWidgets();
        },
        // UI theme: SLOTS images are read from UI/SLOTS_THEMES/<theme>/ ("" = default SLOTS folder). Live.
        "setUITheme", [](ComponentRender& r, const std::string& theme) {
            if (r.getUIManager()) r.getUIManager()->setUITheme(theme);
        },
        // UI design resolution: widget texts scale with the window from it (0, 0 = off)
        "setUIDesignResolution", [](ComponentRender& r, float w, float h) {
            if (r.getUIManager()) r.getUIManager()->setDesignResolution(w, h);
        },
        "getHoveredWidgetCursor", [](ComponentRender& r) -> std::string {
            if (!r.getUIManager()) return "";
            return r.getUIManager()->getHoveredCursorName();
        },
        "isUIScrollHovered", [](ComponentRender& r) -> bool {
            return r.getUIManager() && r.getUIManager()->isScrollHovered();
        },
        "flushTooltip", [](ComponentRender& r, sol::object dtArg) {
            if (!r.getUIManager()) return;
            float dt = (dtArg.valid() && dtArg.get_type() == sol::type::number)
                       ? dtArg.as<float>() : 0.0f;
            r.getUIManager()->flushTooltip(dt);
        },
        "reloadWidgets", [](ComponentRender& r) {
            if (!r.getUIManager()) return;
            r.getUIManager()->reloadWidgets();
        },
        "loadWidget", [](ComponentRender& r, const std::string& filePath) {
            if (!r.getUIManager()) return;
            r.getUIManager()->loadWidgetFromFile(filePath);
        },
        "loadWidgets", [](ComponentRender& r, const std::string& dir) {
            if (!r.getUIManager()) return;
            r.getUIManager()->loadWidgetsFromDir(dir);
        },
        "unloadWidget", [](ComponentRender& r, const std::string& name) {
            if (!r.getUIManager()) return;
            r.getUIManager()->unloadWidget(name);
        },
        "clearWidgets", [](ComponentRender& r) {
            if (!r.getUIManager()) return;
            r.getUIManager()->clearWidgets();
        },
        "setWidgetAlpha", [](ComponentRender& r, float a) {
            if (r.getUIManager()) r.getUIManager()->setGlobalAlpha(a);
        },
        "getObjectByPickingID", [](ComponentRender&, unsigned int id) -> Object3D* {
            if (id == 0) return nullptr;
            return Brakeza::get()->getObjectById(id);
        },
        "getSubmeshEntry", [](ComponentRender& r, unsigned int id) -> std::tuple<Object3D*, std::string> {
            auto entry = r.getSubmeshEntry(id);
            return { entry.first, entry.second };
        }
    );

    lua.new_usertype<ComponentScripting>("ComponentScripting",
    sol::base_classes, sol::bases<Component>(),
        "PlayLUAScripts", &ComponentScripting::PlayLUAScripts,
        "StopLUAScripts", &ComponentScripting::StopLUAScripts,
        "ReloadLUAScripts", &ComponentScripting::ReloadLUAScripts,
        "ReloadScriptEnvironment", &ComponentScripting::ReloadScriptEnvironment,
        "AddSceneLUAScript", &ComponentScripting::AddSceneLUAScript,
        "AddProjectLUAScript", &ComponentScripting::AddProjectLUAScript,
        "getGlobalScriptVar",       &ComponentScripting::getGlobalScriptVar,
        "setGlobalScriptVar",       &ComponentScripting::setGlobalScriptVar,
        "callGlobalScriptFunction", &ComponentScripting::callGlobalScriptFunction,
        "RunProjectScriptsOnStart", &ComponentScripting::RunProjectScriptsOnStart,
        "loadJSON",  &ComponentScripting::loadJSON,
        "saveJSON",  &ComponentScripting::saveJSON
    );

    lua.new_usertype<ComponentInput>("ComponentInput",
    sol::base_classes, sol::bases<Component>(),
        "setKeyboardEnabled", &ComponentInput::setKeyboardEnabled,
        "setMouseEnabled", &ComponentInput::setMouseEnabled,
        "setRelativeMouseMode", &ComponentInput::setRelativeMouseMode,
        "isRelativeMouseMode", &ComponentInput::isRelativeMouseMode,
        "setPadEnabled", &ComponentInput::setPadEnabled,
        "isKeyboardEnabled", &ComponentInput::isKeyboardEnabled,
        "isMouseEnabled", &ComponentInput::isMouseEnabled,
        "isPadEnabled", &ComponentInput::isPadEnabled,
        "isKeyEventDown", &ComponentInput::isKeyEventDown,
        "isKeyEventUp", &ComponentInput::isKeyEventUp,
        "isCharPressed", &ComponentInput::isCharPressed,
        "isCharFirstEventDown", &ComponentInput::isCharFirstEventDown,
        "isMouseMotion", &ComponentInput::isMouseMotion,
        "isClickLeft", &ComponentInput::isClickLeft,
        "isClickRight", &ComponentInput::isClickRight,
        "isClickRightUp", &ComponentInput::isClickRightUp,
        "isMouseButtonDown", &ComponentInput::isMouseButtonDown,
        "isMouseButtonUp", &ComponentInput::isMouseButtonUp,
        "consumeLeftClick", &ComponentInput::consumeLeftClick,
        "isLeftClickConsumed", &ComponentInput::isLeftClickConsumed,
        "isEnabled", &ComponentInput::isEnabled,
        "isAnyControllerButtonPressed", &ComponentInput::isAnyControllerButtonPressed,
        "getRelativeRendererMouseX", &ComponentInput::getRelativeRendererMouseX,
        "getRelativeRendererMouseY", &ComponentInput::getRelativeRendererMouseY,
        "getMouseMotionXRel", &ComponentInput::getMouseMotionXRel,
        "getMouseMotionYRel", &ComponentInput::getMouseMotionYRel,
        "isLeftMouseButtonPressed", &ComponentInput::isLeftMouseButtonPressed,
        "isRightMouseButtonPressed", &ComponentInput::isRightMouseButtonPressed,
        "isMiddleMouseButtonPressed", &ComponentInput::isMiddleMouseButtonPressed,
        "isMouseInWindow", &ComponentInput::isMouseInWindow,
        "getMouseWheelY", &ComponentInput::getMouseWheelY,
        "getRawMouseX", &ComponentInput::getRawMouseX,
        "getRawMouseY", &ComponentInput::getRawMouseY,
        "isGameControllerAvailable", &ComponentInput::isGameControllerAvailable,
        "getControllerButtonA",  &ComponentInput::getControllerButtonA,
        "getControllerButtonB",  &ComponentInput::getControllerButtonB,
        "getControllerButtonX",  &ComponentInput::getControllerButtonX,
        "getControllerButtonY",  &ComponentInput::getControllerButtonY,
        "getControllerAxisTriggerLeft", &ComponentInput::getControllerAxisTriggerLeft,
        "getControllerAxisTriggerRight", &ComponentInput::getControllerAxisTriggerRight,

        "getControllerAxisLeftX", &ComponentInput::getControllerAxisLeftX,
        "getControllerAxisLeftY", &ComponentInput::getControllerAxisLeftY,
        "getControllerAxisRightX", &ComponentInput::getControllerAxisRightX,
        "getControllerAxisRightY", &ComponentInput::getControllerAxisRightY,
        "getControllerPadUp", &ComponentInput::getControllerPadUp,
        "getControllerPadDown", &ComponentInput::getControllerPadDown,
        "getControllerPadLeft", &ComponentInput::getControllerPadLeft,
        "getControllerPadRight", &ComponentInput::getControllerPadRight,

         "getControllerShoulderLeft", &ComponentInput::getControllerShoulderLeft,
         "getControllerShoulderRight", &ComponentInput::getControllerShoulderRight,

         "getControllerButtonBack", &ComponentInput::getControllerButtonBack,
         "getControllerButtonGuide", &ComponentInput::getControllerButtonGuide,
         "getControllerButtonStart", &ComponentInput::getControllerButtonStart
    );

    lua.new_usertype<Components>("Components",
    "Window", &Components::Window,
        "Render", &Components::Render,
        "Camera", &Components::Camera,
        "Collisions", &Components::Collisions,
        "Input", &Components::Input,
        "Sound", &Components::Sound,
        "Scripting", &Components::Scripting
    );

}
