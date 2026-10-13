#!/usr/bin/env python
"""Comprueba los shaders de nodos de assets/shaders/Examples/Node* SIN arrancar el motor.

  1. Compila un arnes con los NODOS REALES (src/GUI/NodeTypes/*.h) y las funciones REALES del gestor
     (AddGlslHelper, AddUniform, GenerateShaderCode, TraverseNode), extraidas como texto de
     src/GUI/ShaderNodeEditorManager.cpp, contra una clase minima que solo aporta el almacenamiento del grafo.
  2. Genera el GLSL de cada ejemplo, comprueba la ida y vuelta de los parametros (SerializeParams/DeserializeParams)
     y avisa de nodos con pines de mas o de menos respecto al fichero.
  3. Compila y enlaza cada fragment shader con el driver OpenGL real (SDL2 + GLEW, ventana oculta).

Uso:  python tools/check_node_shaders.py [--keep] [ficheros.json ...]
Requisitos: una carpeta cmake-build-*/ ya configurada (se usan sus flags/includes), MinGW con SDL2 y GLEW, una GPU.
No ejecuta cmake. Las salidas se dejan en %TEMP%/brakeza_node_shader_check (el GLSL generado, util para depurar).
"""
import glob, os, re, shutil, subprocess, sys, tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
WORK = os.path.join(tempfile.gettempdir(), 'brakeza_node_shader_check')
os.makedirs(WORK, exist_ok=True)


def fail(msg):
    print('ERROR:', msg)
    sys.exit(2)


# ---------------------------------------------------------------- toolchain (de la carpeta cmake-build-* existente)
rsp = next(iter(glob.glob(os.path.join(ROOT, 'cmake-build-*', 'CMakeFiles', 'Brakeza3D.dir', 'includes_CXX.rsp'))), None)
if not rsp:
    fail('no hay ninguna carpeta cmake-build-* configurada (se necesita includes_CXX.rsp)')
flags = open(os.path.join(os.path.dirname(rsp), 'flags.make'), encoding='utf-8').read()
m = re.search(r'compile CXX with (\S+)', flags)
CXX = m.group(1) if m else 'g++'
BIN = os.path.dirname(CXX)
os.environ['PATH'] = BIN + os.pathsep + os.environ.get('PATH', '')
LIB = os.path.join(os.path.dirname(BIN), 'lib')
INC_SDL = os.path.join(os.path.dirname(BIN), 'include', 'SDL2')


def run(cmd, **kw):
    return subprocess.run(cmd, capture_output=True, text=True, **kw)


def obj(name, source, lang):
    out = os.path.join(WORK, name + '.o')
    if not os.path.exists(out):
        cmd = ([CXX, '-std=gnu++17'] if lang == 'c++' else [os.path.join(BIN, 'gcc')]) + ['-c', '-O0', '-w', '@' + rsp, source, '-o', out] \
            if lang == 'c++' else [os.path.join(BIN, 'gcc'), '-c', '-O0', '-w', source, '-o', out]
        r = run(cmd)
        if r.returncode:
            fail('no compila %s\n%s' % (source, r.stderr[-1500:]))
    return out


# ---------------------------------------------------------------- 1. arnes con los nodos reales
real_cpp = open(os.path.join(ROOT, 'src/GUI/ShaderNodeEditorManager.cpp'), encoding='utf-8').read().replace('\r\n', '\n')


def block(start, end):
    a = real_cpp.index(start)
    return real_cpp[a:real_cpp.index(end, a)]


add_helper = block('void ShaderNodeEditorManager::AddGlslHelper', 'std::string ShaderNodeEditorManager::AddUniform')
add_uniform = block('std::string ShaderNodeEditorManager::AddUniform', 'void ShaderNodeEditorManager::RenderEffect')
generate = block('std::string ShaderNodeEditorManager::GenerateShaderCode()', 'void ShaderNodeEditorManager::CompileShader()')

H = os.path.join(WORK, 'harness')
shutil.rmtree(H, ignore_errors=True)
os.makedirs(H + '/nodes')
RJ = ROOT.replace('\\', '/')

for path in glob.glob(os.path.join(ROOT, 'src/GUI/NodeTypes/*.h')):
    s = open(path, encoding='utf-8').read().replace('\r\n', '\n')
    for old in ('#include "../ShaderNodeEditorManager.h"', '#include "../../../include/GUI/ShaderNodeEditorManager.h"',
                '#include "../../../include/GUI/NodeEditorManager.h"'):
        s = s.replace(old, '#include "../fake_mgr.h"')
    s = s.replace('#include "../imgui/imgui.h"', '#include <imgui.h>')
    s = s.replace('#include "../../../include/Misc/cJSON.h"', '#include "%s/include/Misc/cJSON.h"' % RJ)
    open(os.path.join(H, 'nodes', os.path.basename(path)), 'w', encoding='utf-8', newline='\n').write(s)

node_headers = sorted(os.path.basename(p) for p in glob.glob(H + '/nodes/*Node.h'))
classes = [f[:-2] for f in node_headers]

FAKE = r'''
#pragma once
#define SDL_MAIN_HANDLED
#include <imgui.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <sstream>
#include <algorithm>
#include <cstdio>
#define LOG_ERROR(fmt, ...) fprintf(stderr, "[error] " fmt "\n", ##__VA_ARGS__)
#define LOG_MESSAGE(fmt, ...) ((void)0)
#include "@ROOT@/include/Misc/cJSON.h"
#include "@ROOT@/include/OpenGL/Base/SharedOpenGLStructs.h"
#include "@ROOT@/include/GUI/NodeEditorManager.h"

class Image;
struct ImageTexture { Image* image = nullptr; std::string filepath; };
enum class InternalTextureType { Color = 0, Depth = 1 };
enum class MeshTextureType { Diffuse = 0, Specular = 1, Normal = 2, Roughness = 3, AO = 4 };
struct ShaderUniform { std::string name; int components = 1; const float* data = nullptr; GLint location = -1; };
class NodeType;

class ShaderNodeEditorManager {
public:
    ShaderCustomType type;
    std::vector<std::shared_ptr<Node>> m_Nodes;
    std::vector<std::shared_ptr<Link>> m_Links;
    std::vector<std::shared_ptr<Pin>> m_Pins;
    int m_NextId = 1;
    std::map<int, std::string> m_NodeOutputs;
    std::set<int> m_Traversing;
    std::vector<ShaderUniform> m_Uniforms;
    std::vector<std::pair<std::string, std::string>> m_GlslHelpers;
    std::map<int, ImageTexture> m_NodeTextures;
    std::map<std::string, NodeType*> m_NodeTypes;
    static constexpr const char* kSystemTexture = "[System Texture]";

    explicit ShaderNodeEditorManager(ShaderCustomType t) : type(t) {}
    ShaderCustomType getType() const { return type; }
    const std::vector<std::shared_ptr<Node>>& GetNodes() const { return m_Nodes; }
    const std::vector<std::shared_ptr<Link>>& GetLinks() const { return m_Links; }
    std::shared_ptr<Node> FindNode(int id) { for (auto& n : m_Nodes) if (n->id == id) return n; return nullptr; }
    std::shared_ptr<Pin> FindPin(int id) { for (auto& p : m_Pins) if (p->id == id) return p; return nullptr; }
    std::shared_ptr<Node> CreateNode(const std::string& name, ImVec4 color) {
        auto n = std::make_shared<Node>(m_NextId++, name, "base", color); m_Nodes.push_back(n); return n; }
    std::shared_ptr<Pin> AddInputPin(std::shared_ptr<Node> node, const std::string& name, PinType t = PinType::Any) {
        auto p = std::make_shared<Pin>(m_NextId++, node->id, ed::PinKind::Input, t, name); node->inputs.push_back(p); m_Pins.push_back(p); return p; }
    std::shared_ptr<Pin> AddOutputPin(std::shared_ptr<Node> node, const std::string& name, PinType t = PinType::Any) {
        auto p = std::make_shared<Pin>(m_NextId++, node->id, ed::PinKind::Output, t, name); node->outputs.push_back(p); m_Pins.push_back(p); return p; }
    void SetNeedsRecompile() {}
    std::map<int, ImageTexture>& GetNodeTextures() { return m_NodeTextures; }
    void LoadImageForNode(int, const std::string&) {}
    void DeleteImageForNode(int id) { m_NodeTextures.erase(id); }
    void SetExternalTextureForNode(int id, unsigned) {
        m_NodeTextures[id].image = reinterpret_cast<Image*>(1);   // basta con que exista: aqui solo se genera GLSL
        m_NodeTextures[id].filepath = kSystemTexture;
    }
    std::string AddUniform(const std::string& name, int components, const float* data);
    void AddGlslHelper(const std::string& key, const std::string& glslCode);
    std::string GenerateShaderCode();
    std::string TraverseNode(int pinId, std::stringstream& code);
};
'''.replace('@ROOT@', RJ)
open(H + '/fake_mgr.h', 'w', encoding='utf-8', newline='\n').write(FAKE)

includes = '\n'.join('#include "nodes/%s"' % f for f in node_headers)
registry = '\n'.join('        mgr.m_NodeTypes[std::make_unique<%s>()->GetTypeName()] = new %s();' % (c, c) for c in classes)

HARNESS = r'''
#include "fake_mgr.h"
#include "nodes/NodeType.h"
@INCLUDES@
#include <fstream>
#include <iostream>
#include <filesystem>
#include <cmath>

// ---- funciones REALES del gestor (texto extraido de ShaderNodeEditorManager.cpp) ----
@ADD_HELPER@
@ADD_UNIFORM@
@GENERATE@

namespace IconGUI { GUIType::Sheet NODE_TYPE_OUTPUT; GUIType::Sheet NODE_TYPE_DEFAULT; }

static std::string readFile(const std::string& p) { std::ifstream f(p, std::ios::binary); std::stringstream ss; ss << f.rdbuf(); return ss.str(); }

int main(int argc, char** argv) {
    namespace fs = std::filesystem;
    const std::string outDir = argv[1];
    fs::create_directories(outDir);
    int problems = 0;
    for (int a = 2; a < argc; a++) {
        const std::string path = argv[a];
        cJSON* root = cJSON_Parse(readFile(path).c_str());
        cJSON* types = root ? cJSON_GetObjectItem(root, "types") : nullptr;
        if (!types) { std::cerr << "JSON no valido: " << path << "\n"; problems++; continue; }
        const bool obj = NodeJson::String(types, "type") == "NodeMesh3D";

        ShaderNodeEditorManager mgr(obj ? SHADER_NODE_OBJECT : SHADER_NODE_POSTPROCESSING);
@REGISTRY@

        int maxId = 0;
        cJSON* it = nullptr;
        cJSON_ArrayForEach(it, cJSON_GetObjectItem(types, "nodes")) {
            maxId = std::max(maxId, NodeJson::Int(it, "id"));
            for (const char* key : {"inputs", "outputs"}) {
                cJSON* pj = nullptr;
                cJSON_ArrayForEach(pj, cJSON_GetObjectItem(it, key)) maxId = std::max(maxId, NodeJson::Int(pj, "id"));
            }
        }
        mgr.m_NextId = maxId + 1;

        cJSON* nj = nullptr;
        cJSON_ArrayForEach(nj, cJSON_GetObjectItem(types, "nodes")) {
            const std::string typeName = NodeJson::String(nj, "type");
            auto itType = mgr.m_NodeTypes.find(typeName);
            if (itType == mgr.m_NodeTypes.end()) { std::cerr << path << ": tipo de nodo desconocido '" << typeName << "'\n"; problems++; continue; }
            NodeType* nodeType = itType->second;
            auto node = mgr.CreateNode(nodeType->GetTypeName(), nodeType->GetColor());
            node->type = nodeType->GetTypeName();
            nodeType->OnCreate(node);
            nodeType->SetupPins(node, &mgr);

            const int oldId = node->id;
            const int newId = NodeJson::Int(nj, "id", oldId);
            node->id = newId;
            if (node->name == "Internal Texture" || node->name == "Mesh Texture") {
                auto oldIt = mgr.m_NodeTextures.find(oldId);
                if (oldIt != mgr.m_NodeTextures.end() && oldId != newId) {
                    mgr.m_NodeTextures[newId] = oldIt->second;
                    mgr.m_NodeTextures.erase(oldId);
                }
            }
            auto restore = [&](std::vector<std::shared_ptr<Pin>>& pins, const char* key) {
                size_t index = 0; cJSON* pj = nullptr;
                cJSON_ArrayForEach(pj, cJSON_GetObjectItem(nj, key)) {
                    if (index >= pins.size()) { std::cerr << path << ": pin de mas en el nodo " << newId << " (" << typeName << ")\n"; problems++; break; }
                    pins[index]->id = NodeJson::Int(pj, "id", pins[index]->id);
                    index++;
                }
                if (index != pins.size()) { std::cerr << path << ": faltan pines en el nodo " << newId << " (" << typeName << ")\n"; problems++; }
                for (auto& p : pins) p->nodeId = newId;
            };
            restore(node->inputs, "inputs");
            restore(node->outputs, "outputs");

            cJSON* ud = cJSON_GetObjectItem(nj, "userData");
            if (ud) nodeType->DeserializeParams(node, ud);

            // ida y vuelta de los parametros: lo guardado debe contener, con el mismo valor, todo lo leido
            cJSON* back = cJSON_CreateObject();
            nodeType->SerializeParams(node, back);
            cJSON* kv = nullptr;
            cJSON_ArrayForEach(kv, ud) {
                if (std::string(kv->string) == "imagePath") continue;
                cJSON* got = cJSON_GetObjectItem(back, kv->string);
                if (!got) { std::cerr << path << ": " << typeName << " no guarda el parametro '" << kv->string << "'\n"; problems++; }
                else if (cJSON_IsNumber(kv) && std::fabs(got->valuedouble - kv->valuedouble) > 1e-5) {
                    std::cerr << path << ": " << typeName << " '" << kv->string << "' " << kv->valuedouble << " -> " << got->valuedouble << "\n"; problems++;
                }
            }
            cJSON_Delete(back);
        }

        cJSON* lj = nullptr;
        cJSON_ArrayForEach(lj, cJSON_GetObjectItem(types, "links")) {
            int s = NodeJson::Int(lj, "startPinId", -1), e = NodeJson::Int(lj, "endPinId", -1);
            if (!mgr.FindPin(s) || !mgr.FindPin(e)) { std::cerr << path << ": enlace con un pin inexistente\n"; problems++; continue; }
            mgr.m_Links.push_back(std::make_shared<Link>(NodeJson::Int(lj, "id"), s, e));
        }

        const std::string frag = mgr.GenerateShaderCode();
        const std::string base = (obj ? "obj__" : "post__") + fs::path(path).stem().string();
        std::ofstream(outDir + "/" + base + ".frag", std::ios::binary) << frag;
    }
    return problems ? 1 : 0;
}
'''.replace('@INCLUDES@', includes).replace('@ADD_HELPER@', add_helper).replace('@ADD_UNIFORM@', add_uniform) \
   .replace('@GENERATE@', generate).replace('@REGISTRY@', registry)
open(H + '/harness.cpp', 'w', encoding='utf-8', newline='\n').write(HARNESS)

GLCHECK = r'''
#include <SDL.h>
#include <GL/glew.h>
#include <cstdio>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <windows.h>
static std::string readFile(const std::string& p) { std::ifstream f(p, std::ios::binary); std::stringstream ss; ss << f.rdbuf(); return ss.str(); }
// mismos vertex shaders que ShaderNodeEditorManager::CompileShader
static const char* kObjVert = R"(#version 330 core
layout (location = 0) in vec4 aPos; layout (location = 1) in vec3 aNormal; layout (location = 2) in vec2 aTexCoords;
out vec3 v_FragPos; out vec3 v_Normal; out vec2 v_TexCoord;
uniform mat4 u_Projection; uniform mat4 u_View; uniform mat4 u_Model;
void main() { vec4 worldPos = u_Model * aPos; v_FragPos = worldPos.xyz; v_Normal = mat3(transpose(inverse(u_Model))) * aNormal; v_TexCoord = aTexCoords; gl_Position = u_Projection * u_View * worldPos; })";
static const char* kPostVert = R"(#version 330 core
layout (location = 0) in vec2 a_Position; layout (location = 1) in vec2 a_TexCoord;
out vec2 v_TexCoord;
void main() { v_TexCoord = a_TexCoord; gl_Position = vec4(a_Position, 0.0, 1.0); })";
static bool compile(GLenum kind, const std::string& src, GLuint& id, std::string& log) {
    id = glCreateShader(kind); const char* p = src.c_str(); glShaderSource(id, 1, &p, nullptr); glCompileShader(id);
    GLint ok = 0; glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
    if (!ok) { GLint n = 0; glGetShaderiv(id, GL_INFO_LOG_LENGTH, &n); std::vector<char> b(n > 1 ? n : 1); glGetShaderInfoLog(id, n, nullptr, b.data()); log = b.data(); }
    return ok;
}
int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3); SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_Window* w = SDL_CreateWindow("t", 0, 0, 64, 64, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
    if (!w || !SDL_GL_CreateContext(w)) { printf("no hay contexto OpenGL: %s\n", SDL_GetError()); return 3; }
    glewExperimental = GL_TRUE; glewInit();
    printf("GL %s | %s\n", glGetString(GL_VERSION), glGetString(GL_RENDERER));
    int fails = 0, total = 0;
    WIN32_FIND_DATAA fd; HANDLE h = FindFirstFileA((std::string(argv[1]) + "/*.frag").c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) { printf("no hay shaders\n"); return 4; }
    do {
        std::string name = fd.cFileName; total++;
        bool obj = name.rfind("obj__", 0) == 0;
        GLuint vs, fs; std::string log;
        bool ok = compile(GL_VERTEX_SHADER, obj ? kObjVert : kPostVert, vs, log) && compile(GL_FRAGMENT_SHADER, readFile(std::string(argv[1]) + "/" + name), fs, log);
        if (ok) {
            GLuint prog = glCreateProgram(); glAttachShader(prog, vs); glAttachShader(prog, fs); glLinkProgram(prog);
            GLint linked = 0; glGetProgramiv(prog, GL_LINK_STATUS, &linked);
            if (!linked) { GLint n = 0; glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &n); std::vector<char> b(n > 1 ? n : 1); glGetProgramInfoLog(prog, n, nullptr, b.data()); log = std::string("LINK: ") + b.data(); ok = false; }
        }
        if (!ok) { fails++; printf("FAIL %s\n%s\n", name.c_str(), log.c_str()); }
    } while (FindNextFileA(h, &fd));
    printf("%d shaders, %d con error\n", total, fails);
    return fails ? 1 : 0;
}
'''
open(H + '/glcheck.cpp', 'w', encoding='utf-8', newline='\n').write(GLCHECK)

# ---------------------------------------------------------------- compilar y ejecutar
imgui_dir = os.path.join(ROOT, 'imgui')
objs = [obj('cjson', os.path.join(ROOT, 'src/Misc/cJSON.c'), 'c')]
objs += [obj(n, os.path.join(imgui_dir, n + '.cpp'), 'c++') for n in ('imgui', 'imgui_draw', 'imgui_tables', 'imgui_widgets')]

exe = os.path.join(H, 'harness.exe')
r = run([CXX, '-std=gnu++17', '-O0', '-w', '@' + rsp, '-I' + H, os.path.join(H, 'harness.cpp')] + objs + ['-o', exe])
if r.returncode:
    fail('el arnes no compila (¿cambio de firma en un nodo o en el gestor?)\n' + r.stderr[-3000:])

args = [a for a in sys.argv[1:] if not a.startswith('--')]
files = args or sorted(glob.glob(os.path.join(ROOT, 'assets/shaders/Examples/Node*/*.json')))
out_dir = os.path.join(WORK, 'glsl')
shutil.rmtree(out_dir, ignore_errors=True)
r = run([exe, out_dir] + files)
sys.stderr.write(r.stderr)
generated = len(glob.glob(out_dir + '/*.frag'))
print('GLSL generado con el codigo real: %d shaders (%s)' % (generated, 'con avisos' if r.returncode else 'sin avisos'))

gl_exe = os.path.join(H, 'glcheck.exe')
r2 = run([CXX, '-std=gnu++17', '-O0', '-w', '-I' + INC_SDL, os.path.join(H, 'glcheck.cpp'), '-o', gl_exe,
          '-L' + LIB, '-lmingw32', '-lSDL2main', '-lSDL2', '-lglew32', '-lopengl32'])
if r2.returncode:
    fail('glcheck no compila\n' + r2.stderr[-1500:])
r3 = run([gl_exe, out_dir])
print(r3.stdout.strip())
print('GLSL en', out_dir)
sys.exit(1 if (r.returncode or r3.returncode) else 0)
