//
// Undo/redo del editor. Versión mínima: deshace los cambios de TRANSFORMACIÓN y estado básico de un objeto
// (posición, rotación, escala, alpha, enabled) hechos con el gizmo o desde los campos del inspector.
// No guarda punteros a objetos (solo su nombre): si el objeto ya no existe, el paso se descarta.
//
// Cómo se engancha (todo con una línea):
//   - Gizmo:      Drawable::drawObject3DGizmo   -> observe(o, SRC_GIZMO, ImGuizmo::IsUsing())
//   - Inspector:  GUIAddonObject3DProperties    -> observe(o, SRC_FIELD, fieldEditing())
//   - Campos:     GUIFields::EndRow             -> onFieldItem()   (detecta cuándo se edita/suelta un campo)
//   - Teclas:     ComponentInput::HandleGUIShortCuts (Ctrl+Z / Ctrl+Y / Ctrl+Shift+Z) y el menú Edit
//

#ifndef BRAKEZA3D_UNDOMANAGER_H
#define BRAKEZA3D_UNDOMANAGER_H

#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

class Object3D;
struct ParticlesContext;

class UndoManager
{
public:
    enum Source : unsigned { SRC_GIZMO = 1u, SRC_FIELD = 2u };

    static UndoManager &get();

    // Llamar cada frame para el objeto que se está editando. editing = hay un gesto en curso en esa fuente.
    // Mientras nadie edita se guarda el estado "de partida"; al terminar el gesto, si cambió, se apila un paso.
    void observe(Object3D *o, Source source, bool editing);

    // Desde GUIFields::EndRow: mira el último widget dibujado
    void onFieldItem();
    [[nodiscard]] bool fieldEditing() const { return fieldActive; }

    // Borrado desde el editor: guarda una foto JSON de cada objeto y los marca como eliminados (deshacer = recrearlos)
    void removeObjects(const std::vector<Object3D *> &list);
    // Alta desde el editor (menú Add Object): el próximo objeto que entre en la escena queda registrado como creación
    // (deshacer = eliminarlo). onObjectAdded lo llama Brakeza::AddObject3D.
    void expectCreation();
    void onObjectAdded(Object3D *o);

    bool undo();
    bool redo();
    void clear();
    // Recorta la pila al límite de Config::UNDO_MAX_STEPS (descarta los pasos más antiguos)
    void trim();

    // Solo un comando activo a la vez: mientras un objeto se está recreando (carga asíncrona) no se acepta otro
    // undo/redo, así un doble Ctrl+Z no puede adelantarse a la recreación en curso
    [[nodiscard]] bool isBusy() const;
    [[nodiscard]] bool canUndo() const { return !undoStack.empty() && !isBusy(); }
    [[nodiscard]] bool canRedo() const { return !redoStack.empty() && !isBusy(); }
    [[nodiscard]] std::string undoLabel() const { return undoStack.empty() ? "" : undoStack.back().label; }
    [[nodiscard]] std::string redoLabel() const { return redoStack.empty() ? "" : redoStack.back().label; }

private:
    struct State
    {
        float pos[3]   = {0, 0, 0};
        float rot[9]   = {1, 0, 0, 0, 1, 0, 0, 0, 1};
        float scale[3] = {1, 1, 1};
        float alpha    = 1.0f;
        bool  enabled  = true;
        // Propiedades propias del tipo (luces, partículas, sonido, mesh...), en el orden de captureExtra/applyExtra
        std::vector<float> extra;
        std::shared_ptr<ParticlesContext> pctx;   // emisor de partículas: copia completa del contexto
        bool operator==(const State &o) const;
    };
    // Objeto eliminado/creado: foto JSON (formato de escena) y la escena a la que pertenecía
    struct Snap
    {
        std::string name;
        std::string json;
        std::string sceneName;
        bool belongToScene = false;
    };
    struct Entry
    {
        std::string name;
        std::string label;
        State before, after;
        // Entradas de alta/baja de objetos (si objs no está vacío se ignoran before/after)
        std::vector<Snap> objs;
        bool presence = false;      // true = entrada de alta/baja
        bool createdEntry = false;  // true = nació de un alta (deshacer elimina); false = nació de un borrado (deshacer recrea)
    };

    std::vector<Entry> undoStack, redoStack;
    State baseline;
    std::string baselineName;
    unsigned activeMask = 0;     // fuentes con un gesto en curso
    bool wasEditing = false;
    bool fieldActive = false;
    bool awaitingCreation = false;
    std::unordered_set<std::string> pendingRecreate;   // objetos cuya recreación aún no ha terminado
    unsigned long long pendingSince = 0;
    unsigned long long awaitingSince = 0;

    static Snap snapshot(Object3D *o);
    static void recreate(const Snap &s);
    static void removeByName(const std::string &name);
    void push(const Entry &e);
    static State capture(Object3D *o);
    static void captureExtra(Object3D *o, State &s);
    static void applyExtra(Object3D *o, const State &s);
    static bool apply(const std::string &name, const State &s);
    static bool blocked();       // sin undo mientras corren los scripts
};

#endif //BRAKEZA3D_UNDOMANAGER_H
