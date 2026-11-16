// Prueba aislada de AssetPaths (solo libreria estandar; no necesita el resto del motor ni CMake):
//   g++ -std=c++17 -I. tools/asset_paths_test.cpp src/Misc/AssetPaths.cpp -o asset_paths_test && ./asset_paths_test
#include "../include/Misc/AssetPaths.h"

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <thread>
#include <vector>

namespace fs = std::filesystem;
static int fails = 0;
static void check(const char *label, bool ok) { std::printf("%s %s\n", ok ? "OK   " : "FALLA", label); if (!ok) fails++; }
static void touch(const fs::path &p) { fs::create_directories(p.parent_path()); std::ofstream(p.string()) << "x"; }

int main()
{
    const fs::path tmp = fs::temp_directory_path() / "asset_paths_test";
    fs::remove_all(tmp);
    const std::string core = (tmp / "assets").generic_string() + "/";
    const std::string proj = (tmp / "projects" / "P" / "assets").generic_string() + "/";
    touch(tmp / "assets" / "models" / "core_only.fbx");
    touch(tmp / "assets" / "models" / "both.fbx");
    touch(tmp / "projects" / "P" / "assets" / "models" / "both.fbx");
    touch(tmp / "projects" / "P" / "assets" / "models" / "proj_only.fbx");
    touch(tmp / "projects" / "P" / "assets" / "sounds" / "pack" / "x.wav");

    AssetPaths::SetCorePrefix(core);
    const std::string c = core;

    check("sin proyecto: IsActive false", !AssetPaths::IsActive());
    check("sin proyecto: la ruta no cambia", AssetPaths::Resolve(c + "models/both.fbx") == c + "models/both.fbx");

    AssetPaths::SetProjectRoot(proj);
    check("con proyecto: IsActive true", AssetPaths::IsActive());
    check("el proyecto tiene prioridad (existe en los dos)", AssetPaths::Resolve(c + "models/both.fbx") == proj + "models/both.fbx");
    check("solo en el proyecto -> la del proyecto", AssetPaths::Resolve(c + "models/proj_only.fbx") == proj + "models/proj_only.fbx");
    check("solo en el core -> respaldo en el core", AssetPaths::Resolve(c + "models/core_only.fbx") == c + "models/core_only.fbx");
    check("no existe en ninguno -> ruta original", AssetPaths::Resolve(c + "models/nada.fbx") == c + "models/nada.fbx");
    check("tolera '//' tras el prefijo (datos antiguos)", AssetPaths::Resolve(core.substr(0, core.size() - 1) + "//sounds/pack/x.wav") == proj + "sounds/pack/x.wav");
    check("fuera del prefijo del core no se toca", AssetPaths::Resolve("../GLSL/Render.vs") == "../GLSL/Render.vs");
    check("prefijo parecido no se confunde (assets_otros)", AssetPaths::Resolve(core.substr(0, core.size() - 1) + "_otros/models/both.fbx") == core.substr(0, core.size() - 1) + "_otros/models/both.fbx");
    check("ruta vacia", AssetPaths::Resolve("") == "");
    check("RelativeToCore", AssetPaths::RelativeToCore(c + "models/x.fbx") == "models/x.fbx" && AssetPaths::RelativeToCore("../GLSL/x") == "");

    // cache: un fichero que aparece despues no se ve hasta ClearCache
    const std::string late = c + "models/late.fbx";
    check("late: aun no existe -> original", AssetPaths::Resolve(late) == late);
    touch(tmp / "projects" / "P" / "assets" / "models" / "late.fbx");
    check("late: cacheado como inexistente", AssetPaths::Resolve(late) == late);
    AssetPaths::ClearCache();
    check("late: tras ClearCache se ve", AssetPaths::Resolve(late) == proj + "models/late.fbx");

    // barras invertidas y sin '/' final en la raiz
    std::string winRoot = proj.substr(0, proj.size() - 1);
    for (auto &ch : winRoot) if (ch == '/') ch = '\\';
    AssetPaths::SetProjectRoot(winRoot);
    check("raiz con '\\' y sin '/' final se normaliza", AssetPaths::Resolve(c + "models/proj_only.fbx") == proj + "models/proj_only.fbx");

    // ".." de las texturas de un modelo: ".../models/pack/BASE/../../../textures/x.png" se colapsa antes de mapear al proyecto
    touch(tmp / "projects" / "P" / "assets" / "textures" / "pack" / "BASE" / "x.png");
    AssetPaths::SetProjectRoot(proj);
    check("'..' del modelo: se colapsa y se resuelve en el proyecto",
          AssetPaths::Resolve(c + "models/pack/BASE/../../../textures/pack/BASE/x.png") == proj + "textures/pack/BASE/x.png");
    check("'..' que se sale de la raiz: ruta tal cual", AssetPaths::Resolve(c + "../../fuera/x.png") == c + "../../fuera/x.png");
    check("'.' intermedios", AssetPaths::Resolve(c + "./models/./proj_only.fbx") == proj + "models/proj_only.fbx");

    // listados: ResolveDirs une core y proyecto
    fs::create_directories(tmp / "assets" / "ui" / "core_only");
    fs::create_directories(tmp / "projects" / "P" / "assets" / "ui" / "kit");
    fs::create_directories(tmp / "assets" / "ui" / "both");
    fs::create_directories(tmp / "projects" / "P" / "assets" / "ui" / "both");
    auto d1 = AssetPaths::ResolveDirs(c + "ui/");
    check("ResolveDirs de una carpeta en los dos: core primero, proyecto despues", d1.size() == 2 && d1[0] == c + "ui/" && d1[1] == proj + "ui/");
    auto d2 = AssetPaths::ResolveDirs(c + "ui/kit/");
    check("ResolveDirs de una carpeta solo del proyecto", d2.size() == 1 && d2[0] == proj + "ui/kit/");
    auto d3 = AssetPaths::ResolveDirs(c + "ui/core_only/");
    check("ResolveDirs de una carpeta solo del core", d3.size() == 1 && d3[0] == c + "ui/core_only/");
    auto d4 = AssetPaths::ResolveDirs(c + "no_existe/");
    check("ResolveDirs de una que no existe: la original", d4.size() == 1 && d4[0] == c + "no_existe/");
    auto d5 = AssetPaths::ResolveDirs("../GLSL/");
    check("ResolveDirs fuera del core: la original", d5.size() == 1 && d5[0] == "../GLSL/");
    auto d6 = AssetPaths::ResolveDirs(c);
    check("ResolveDirs de la propia raiz de assets: core y proyecto", d6.size() == 2 && d6[1] == proj);
    AssetPaths::SetProjectRoot("");
    check("sin proyecto ResolveDirs devuelve la original", AssetPaths::ResolveDirs(c + "ui/").size() == 1);
    AssetPaths::SetProjectRoot(proj);

    // hilos (la carga de assets va por ThreadPool)
    std::vector<std::thread> th; std::vector<int> bad(8, 0);
    AssetPaths::SetProjectRoot(proj);
    for (int t = 0; t < 8; t++) th.emplace_back([&, t] {
        for (int i = 0; i < 4000; i++) {
            if (AssetPaths::Resolve(c + "models/both.fbx") != proj + "models/both.fbx") bad[t]++;
            if (AssetPaths::Resolve(c + "models/core_only.fbx") != c + "models/core_only.fbx") bad[t]++;
            if (i % 997 == 0) AssetPaths::ClearCache();
        }
    });
    for (auto &x : th) x.join();
    int total = 0; for (int b : bad) total += b;
    check("8 hilos x 4000 resoluciones sin resultados erroneos", total == 0);

    AssetPaths::SetProjectRoot("");
    check("al vaciar la raiz vuelve a ser identidad", !AssetPaths::IsActive() && AssetPaths::Resolve(c + "models/both.fbx") == c + "models/both.fbx");

    fs::remove_all(tmp);
    std::printf(fails ? "FALLOS: %d\n" : "TODO OK\n", fails);
    return fails ? 1 : 0;
}
