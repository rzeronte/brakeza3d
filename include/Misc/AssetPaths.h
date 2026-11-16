/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
#ifndef BRAKEZA3D_ASSETPATHS_H
#define BRAKEZA3D_ASSETPATHS_H

#include <string>
#include <vector>

// Resolución de rutas de assets con DOS niveles: la carpeta de assets del PROYECTO activo (prioridad) y la del CORE del motor (respaldo).
//
// Los ficheros de contenido (escenas, scripts, widgets, JSON de datos...) escriben rutas literales "../assets/<tipo>/<fichero>" relativas al
// ejecutable. Con un proyecto activo que declare "assetsFolder" (p. ej. "../projects/MyGame/assets/"), Resolve() mira primero
// "<assetsFolder><tipo>/<fichero>" y, si existe, devuelve esa ruta; si no, devuelve la original (el core). Así un proyecto tiene su carpeta de
// assets propia con la misma estructura por tipo, puede reutilizar lo que ofrece el core sin copiarlo, y puede sobrescribir cualquier recurso
// del core con uno propio del mismo nombre.
//
// Sin proyecto activo (o sin "assetsFolder") Resolve() devuelve la ruta tal cual: el comportamiento es IDÉNTICO al anterior.
// Solo usa la librería estándar (sin dependencias del motor); es seguro llamarlo desde los hilos de carga.
namespace AssetPaths {

    // Carpeta de assets del proyecto activo, relativa al ejecutable (con o sin "/" final). Vacía = ningún proyecto con assets propios.
    void SetProjectRoot(const std::string &folder);
    std::string GetProjectRoot();

    // Prefijo del core que se reescribe (por defecto "../assets/"; el motor lo sincroniza con Config::ASSETS_FOLDER).
    void SetCorePrefix(const std::string &prefix);
    std::string GetCorePrefix();

    // true si hay una raíz de proyecto activa.
    bool IsActive();

    // Devuelve la ruta real a usar para `path`: la del proyecto si existe allí, o `path` sin cambios.
    // Tolera "//" tras el prefijo ("../assets//sounds/x.wav", presente en datos antiguos) y colapsa "." / ".." (las texturas de un modelo se
    // referencian como ".../models/pack/BASE/../../../textures/x.png"); si los ".." se salen de la raíz de assets devuelve la ruta tal cual.
    std::string Resolve(const std::string &path);

    // Carpetas a recorrer para LISTAR `dirPath` (p. ej. "../assets/ui/"): primero la del core y después la del proyecto (prioridad creciente), solo
    // las que existen. Sin proyecto activo o fuera del core devuelve {dirPath}. Es la contraparte de Resolve() para listados.
    std::vector<std::string> ResolveDirs(const std::string &dirPath);

    // Olvida las comprobaciones de existencia cacheadas (llamar si se añaden o quitan ficheros del proyecto en ejecución).
    void ClearCache();

    // Ruta relativa al core ("models/x.fbx") de una ruta "../assets/models/x.fbx"; cadena vacía si no cuelga del prefijo del core.
    std::string RelativeToCore(const std::string &path);

} // namespace AssetPaths

#endif //BRAKEZA3D_ASSETPATHS_H
