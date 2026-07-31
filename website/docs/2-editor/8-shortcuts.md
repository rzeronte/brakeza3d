# Keyboard & Mouse Shortcuts

This section covers all the keyboard and mouse shortcuts available in the Brakeza3D editor to speed up your workflow.

---

## System / Editor

| Key | Action |
|-----|--------|
| F1 | Play / Stop LUA scripts |
| F2 | Reload LUA scripts |
| F3 | Clear Scene (remove all objects) |
| F4 | Clean Scene |
| F5 | Toggle ImGui visibility |
| Ctrl + P | Toggle Post-Processing Chain |
| F6 | Switch to DEFAULT layout |
| F7 | Switch to CODING layout |
| F8 | Switch to DESIGN layout |
| F11 | Toggle Fullscreen mode |
| Right Ctrl | Hold to use shortcuts while LUA is executing |

---

## Selection

| Action | Result |
|--------|--------|
| Left Click | Select single object |
| Left Click (empty space) | Clear selection |
| Ctrl + Left Click | Toggle object into / out of selection group |
| Ctrl + Left Drag | Rectangle-select all objects inside the drawn area |

---

## Transformations (with object selected)

These shortcuts only work when an object is selected in the scene.

| Key | Action |
|-----|--------|
| T | Translate mode |
| R | Rotate mode |
| S | Scale mode |
| X | Delete selected object |
| ESC | Deselect object |

---

## Snap Camera to Selected Object

Place the camera at a fixed distance from the selected object, aligned to a standard view axis.
The snap distance (radius) can be adjusted in **View → Snap camera to selected object**.

| Key | View | Camera placed at |
|-----|------|-----------------|
| Insert | Front | In front of the object (+Z) |
| Home | Top | Above the object (+Y), looking straight down |
| Page Up | Side | To the right of the object (+X) |

:::note
These shortcuts are disabled when no object is selected.
:::

---

## Camera Movement

| Key | Action |
|-----|--------|
| ↑ | Move Forward |
| ↓ | Move Backward |
| ← | Strafe Left |
| → | Strafe Right |
| Shift + ↑ | Move Up |
| Shift + ↓ | Move Down |
| Mouse Wheel | Zoom In / Out |
| Right Click + Drag | Look around (rotate camera view) |

---

## Tips

:::tip Right Ctrl Modifier
When LUA scripts are running (Play mode), most shortcuts are disabled. Hold `Right Ctrl` to temporarily enable editor shortcuts while scripts are executing.
:::

:::tip Transformation Gizmo
After pressing T, R, or S, click and drag the gizmo axes in the viewport to transform the selected object.
:::

:::caution Quick Delete
Press X to instantly remove the selected object from the scene. This action cannot be undone.
:::
