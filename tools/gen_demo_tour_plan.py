"""Generates tools/demo_tour_plan.json: what record_demo_tour.ps1 does in each demo of the DemoBrowser
(keys / mouse, "steps") and the subtitles burned into the video ("subs"). Times are seconds after the demo is ready.

    python tools/gen_demo_tour_plan.py
"""
import json
import os

P = {}


def K(t, key, hold=0.15):
    return {"t": t, "key": key, "hold": hold}


def seq(keys, t0=0.6, gap=1.0, hold=0.8):
    return [K(round(t0 + i * gap, 2), k, hold) for i, k in enumerate(keys)]


def taps(keys, t0=0.6, gap=0.9):
    return seq(keys, t0, gap, 0.12)


def S(t, d, text):
    return {"t": t, "d": d, "text": text}


LAYOUTS_MENU, LAYOUT_DEFAULT, LAYOUT_CODING, LAYOUT_DESIGN = [0.4635, 0.0119], [0.4740, 0.0714], [0.4740, 0.0951], [0.4740, 0.1189]


def click(t, pos, pre=0.8):
    return {"t": t, "click": pos, "button": "left", "pre": pre}     # menus / small icons: rest on the target before pressing


def plan(n, steps, subs, end=None):
    last = max([s["t"] + s["d"] for s in subs] + [0])
    P[str(n)] = {"end": max(end or 0, round(last + 0.8, 1)), "steps": steps, "subs": subs}


plan(1, [], [
    S(0.3, 4.2, "Hi! Let's take a quick tour of Brakeza3D: a C++ engine with OpenGL deferred rendering, scripted in Lua."),
    S(4.7, 4.3, "This first demo is tiny: one Lua script prints a message, the run time and the FPS, every single frame."),
])
plan(2, seq(["W", "A", "S", "D"], 1.0, 0.9, 0.6) + taps(["SPACE", "E"], 5.0, 0.9)
     + [{"t": 7.2, "move": [0.3, 0.4, 0.7, 0.6], "dur": 1.4}, {"t": 8.9, "click": [0.6, 0.5], "button": "left"}], [
    S(0.3, 3.8, "Input is polled from Lua. Keys I hold show up on the panel: 'held' versus 'just pressed' are different calls."),
    S(4.4, 2.6, "Space and E are single presses: they only fire on the first frame."),
    S(7.1, 3.0, "The mouse works the same way: position, buttons, and clicks."),
])
plan(3, seq(["W", "D", "S", "A"], 1.0, 0.9, 0.7) + seq(["SPACE", "C"], 5.0, 1.0, 0.7)
     + seq(["I", "K"], 7.5, 1.1, 0.8) + seq(["Q", "E"], 10.0, 1.1, 0.9), [
    S(0.3, 3.8, "This cube has an object script attached. 'this' is the object itself, so the same script works on anything."),
    S(4.7, 2.6, "Up and down along the world axes..."),
    S(7.3, 2.6, "...forward and back along the cube's own axis..."),
    S(9.9, 2.8, "...and rotating on the spot. Speeds are in units per second, rotations in degrees."),
])
plan(4, seq(["W", "S", "A", "D", "Q", "E"], 1.0, 0.9, 0.7) + taps(["SPACE"], 7.4) + taps(["SPACE"], 10.8) + taps(["R"], 11.8), [
    S(0.3, 3.8, "Rotation is just a matrix: W/S pitch, A/D yaw, Q/E roll. Each key multiplies the object's M3 by a small rotation."),
    S(7.2, 3.2, "Space switches on auto-rotation, applied every frame with the delta time so it's frame-rate independent."),
    S(11.0, 2.2, "And R puts everything back to the starting rotation."),
])
plan(5, taps(["C", "C", "C", "C"], 1.0, 0.8) + taps(["V", "V"], 5.2, 1.0) + taps(["X", "X", "X"], 8.0, 0.8), [
    S(0.3, 3.6, "Objects can be created from a script. Every press of C drops a random coloured point light into the scene."),
    S(4.9, 2.9, "V hides and shows the last one. A disabled object costs nothing."),
    S(7.9, 3.0, "And X removes them for good. Removed objects are cleaned up by the engine."),
])
plan(6, seq(["W", "D", "Q", "E"], 1.2, 1.0, 0.8) + [K(6.2, "O", 0.2), K(10.0, "O", 0.2)], [
    S(0.3, 3.8, "Parenting: four coloured lights are children of the cube. Wherever the parent goes, they follow."),
    S(5.8, 3.6, "Children keep their offset relative to the parent, so we can animate their orbits on top of the movement."),
    S(9.6, 2.4, "Switch it off and they sit still again."),
])
plan(7, taps(["S", "P", "P", "O"], 1.0, 1.0) + taps(["SPACE"], 5.4) + seq(["A", "D"], 7.2, 1.3, 1.0) + taps(["TAB", "TAB"], 10.4, 1.0), [
    S(0.3, 3.8, "The cone looks at the sphere with LookAt. Here it turns smoothly; I'm speeding that up and slowing it down."),
    S(5.2, 2.5, "Follow off: now I turn it by hand with A and D."),
    S(10.2, 2.8, "And Tab changes which object it tracks."),
])
plan(8, seq(["W", "D", "SPACE", "C", "I", "K"], 1.0, 1.4, 1.1), [
    S(0.3, 4.0, "A follow camera is a few lines: each frame it eases towards a point behind the cube, then looks at it."),
    S(5.2, 3.5, "The script turns the keyboard off for the camera, so the keys only move the cube."),
])
plan(9, [], [
    S(0.3, 4.2, "Scripts can talk to each other. The cube owns two variables, declared in its JSON file..."),
    S(4.7, 4.0, "...and the sphere reads them every second through getLocalScriptVar. The values go to the console."),
])
plan(10, seq(["Q", "E", "Z", "X", "I", "L", "K", "J"], 1.5, 1.3, 1.0), [
    S(0.3, 4.0, "Lights from code. Q and E rotate the sun: the shading and the shadows follow in real time."),
    S(4.6, 2.6, "Z and X change the ambient light, the 'floor' of the lighting."),
    S(7.4, 5.0, "And a point light I created in the script, with attenuation, moves around the cubes."),
])
plan(11, seq(["W", "D", "S", "A"], 1.0, 0.9, 0.5) + taps(["SPACE"], 5.2) + seq(["I", "K"], 6.6, 1.1, 0.8) + taps(["X"], 9.2) + taps(["SPACE"], 10.4), [
    S(0.3, 4.0, "Physics is Bullet. This box is a rigid body with mass and friction; W A S D push it with forces."),
    S(5.0, 1.3, "Space gives it an impulse: a jump."),
    S(6.4, 2.6, "I and K set its velocity directly instead. Different tools for different feels."),
    S(9.0, 3.0, "X stops all motion dead, and then... jump again."),
])
plan(12, taps(["R", "G", "R", "G"], 1.5, 1.8), [
    S(0.3, 4.2, "The player box falls onto the enemy and gets knocked back: that's a collision callback running in Lua."),
    S(4.7, 4.5, "R fires a visible ray at the target (green is a hit), and G checks the ground below."),
])
# Inspector of the animated character (editor window 1920x1009, Default layout): browser row Hero, "Mesh3D Animation" header, clip list, Speed field
def px(x, y):
    return [round(x / 1920, 4), round(y / 1009, 4)]


def pclick(t, x, y, pre=0.6):
    return click(t, px(x, y), pre)


AN_LIST = px(230, 700)       # over the clip list once the inspector is scrolled (the wheel scrolls the list, not the panel)
AN_PANEL = px(230, 787)      # under the list: the wheel scrolls the whole panel
AN_SPEED_R, AN_SPEED_L = px(300, 825), px(240, 825)   # Speed is a drag field limited to 0..1: 60 px to the left = 0.4
INTERFACE_MENU, DRAW_BONES = [0.5055, 0.0119], [0.5417, 0.5392]   # Interface menu / its "Draw Animation Bones" row (y = 544 px)
plan(13, [K(4.5, "2", 0.2), K(9.0, "3", 0.2), K(13.5, "1", 0.2), K(16.5, "4", 0.2), K(21.0, "4", 0.2), K(25.5, "4", 0.2),
          K(30.0, "B", 0.2), K(32.5, "B", 0.2), K(35.0, "5", 0.2),
          # ---- the editor: pick the clip and the speed from the inspector (no jump: it looks bad when played from the list) ----
          K(38.6, "1", 0.2),                                              # idle: the character stands upright, always in the same place
          {"t": 39.8, "editor": True},
          click(41.5, INTERFACE_MENU), click(44.0, DRAW_BONES, 1.0),     # Interface > Draw Animation Bones (off)
          click(48.0, INTERFACE_MENU), click(50.5, DRAW_BONES, 1.0),     # ... and on again
          pclick(56.5, 960, 540, 0.8),                                    # click the character in the view: selects it
          pclick(59.5, 47, 833),                                          # open the "Mesh3D Animation" header of the docked inspector
          {"t": 62.0, "wheel": -6, "at": AN_PANEL},                       # scroll the inspector down to the clip list
          {"t": 64.5, "wheel": -3, "at": AN_LIST},                        # scroll the list to its end: WALKING and RUNNING visible
          pclick(66.5, 100, 743),                                         # Armature|WALKING
          pclick(71.0, 100, 722),                                         # Armature|RUNNING
          {"t": 75.5, "drag": AN_SPEED_R + AN_SPEED_L, "button": "left", "dur": 1.2},   # Speed 1.0 -> 0.4
          {"t": 80.5, "drag": AN_SPEED_L + AN_SPEED_R, "button": "left", "dur": 1.2},   # and back to 1.0
          {"t": 84.0, "wheel": 3, "at": AN_LIST},                         # list back to the top
          pclick(85.5, 100, 626),                                         # Armature|CROUCH_WALKING
          {"t": 89.0, "wheel": 8, "at": AN_PANEL},                        # leave the inspector as found: scrolled to the top...
          pclick(91.0, 47, 833),                                          # ...and "Mesh3D Animation" closed
          {"t": 93.5, "editor": False}], [
    S(0.3, 4.0, "Skeletal animation. The yellow lines are the skeleton: every bone moves the part of the mesh attached to it."),
    S(4.6, 4.2, "Walk: press 2 and the character switches to the walking clip."),
    S(9.1, 4.2, "Run: press 3. Same bones, a different clip."),
    S(13.6, 2.6, "Idle: back to the standing pose."),
    S(16.7, 4.0, "Jump: press 4. The clip plays once, and when it ends the script sends the character back to idle."),
    S(21.2, 4.0, "Another jump. Watch the legs bend and the whole skeleton lift."),
    S(25.7, 4.0, "And one more. isAnimationEnds() tells the script when the clip is over."),
    S(30.2, 2.2, "B hides the skeleton..."),
    S(32.7, 2.2, "...and shows it again."),
    S(35.2, 4.0, "Crouch walk is the fifth clip. Speed (P / O) and looping (L) can change while it plays."),
    S(41.2, 5.4, "The editor has its own debug tools. Interface, Draw Animation Bones: the skeleton goes off..."),
    S(48.2, 5.0, "...and on again. Any animated mesh, in any scene, with no script at all."),
    S(53.4, 4.0, "Animations can be chosen without any script, too. Select the character in the view..."),
    S(59.6, 4.4, "...and open Mesh3D Animation in its inspector: every clip stored in the FBX is in a list."),
    S(66.6, 4.2, "Click Walking and it plays at once..."),
    S(71.1, 4.2, "...click Running and it changes again."),
    S(75.6, 4.6, "The Speed field sets the playback rate. Slow motion..."),
    S(80.7, 3.2, "...and back to normal speed."),
    S(85.6, 4.2, "And Crouch walking, from the same list."),
], end=95.5)
plan(14, seq(["1", "2", "3", "4"], 1.0, 2.7, 0.3) + taps(["SPACE"], 12.2), [
    S(0.3, 4.0, "A particle emitter created entirely from code: fire, smoke, magic sparks and snow are just parameter sets."),
    S(4.6, 5.0, "Emission rate, lifetime, gravity, speed and a colour gradient. Up to 512 particles per emitter."),
    S(12.0, 2.0, "Space toggles the emission."),
])
plan(15, taps(["Q", "W", "E", "R", "T"], 1.0, 0.8) + [K(5.6, "M", 0.2), K(9.4, "M", 0.2)], [
    S(0.3, 4.4, "Audio from Lua: register short sounds once, then play them by name whenever you need them."),
    S(5.3, 3.8, "Music loops in the background, and the volume can be set live."),
])
plan(16, [], [
    S(0.3, 4.2, "A scene shader, written in GLSL, tints the whole screen."),
    S(4.6, 4.2, "The script writes new values into the shader's variables every frame. That's how you animate effects."),
])
# Showroom: the first demo doubles as an editor intro (Design layout, scripts stopped so the gizmos work, then Play again)
SCRIPTS_MENU = [0.1375, 0.0119]
ICON_MOVE, ICON_ROTATE = [0.271, 0.0375], [0.290, 0.0375]
SR_CUBE, SR_PAWN = [0.422, 0.479], [0.371, 0.549]
# Docked inspector (Default layout, window 1920x1009), measured on screenshots of the real GUI: x of the header arrows and the y of each
# header row with every section closed (the same click opens and closes a block). The Remove button sits ~40 px under the last header
# and opens a confirmation: never aim near it.
INSP_X = 47
HDR = {"transf": 647, "render": 678, "attached": 709, "collider": 740, "misc": 771, "mesh": 802}
MESH_SUB = {"render": 832, "info": 857}          # sub-nodes of Mesh3D once it is open ("Mesh render options", "Mesh information")
SCRIPTS_PLAY, SCRIPTS_STOP = [0.150, 0.0446], [0.150, 0.0763]     # menu rows at y = 45 and 77 px


def ip(key, x=INSP_X):
    return px(x, HDR[key] if key in HDR else MESH_SUB[key])


plan(17, [{"t": 10.8, "editor": True},
          # project panel (docked right in the Default layout): scenes, global scripts, settings; closed again at the end so the other demos find it as it was
          click(13.5, [0.8094, 0.2134], 0.6), click(17.5, [0.8094, 0.1806], 0.6), click(21.5, [0.8094, 0.1494], 0.6),
          click(27.0, [0.8094, 0.1494], 0.6), click(29.0, [0.8094, 0.1806], 0.6), click(31.0, [0.8094, 0.2134], 0.6),
          click(34.0, [0.4635, 0.0119]), click(36.8, [0.4740, 0.1189], 1.0),            # Layouts > Design
          click(40.5, SCRIPTS_MENU), click(43.3, SCRIPTS_STOP, 1.0),                    # Script Controls > Stop scripts
          click(47.0, SR_PAWN),                                                          # select: green outline
          click(50.5, ICON_MOVE),
          {"t": 53.5, "drag": [0.367, 0.514, 0.367, 0.425], "button": "left", "dur": 1.8, "pre": 1.0},   # up the green (Y) arrow of the move gizmo
          click(59.0, SR_CUBE),
          click(62.0, ICON_ROTATE),
          {"t": 65.0, "drag": [0.466, 0.449, 0.484, 0.532], "button": "left", "dur": 1.8, "pre": 1.0},   # along the rotation ring
          click(68.5, LAYOUTS_MENU), click(71.3, LAYOUT_DEFAULT, 1.0),                   # Layouts > Default: Object Properties is docked there (the cube stays selected)
          # every block of the inspector, one at a time (rows above the header never move, so the same click opens and closes it)
          click(77.5, ip("transf"), 0.6), {"t": 81.5, "drag": px(275, 706) + px(325, 706), "button": "left", "dur": 1.2, "pre": 0.8}, click(87.0, ip("transf"), 0.6),
          click(90.0, ip("render"), 0.6), {"t": 93.0, "wheel": -3, "at": px(230, 800)}, {"t": 96.0, "wheel": 6, "at": px(230, 800)}, click(97.5, ip("render"), 0.6),
          click(100.5, ip("attached"), 0.6), click(105.0, ip("attached"), 0.6),
          click(108.0, ip("collider"), 0.6), click(113.0, ip("collider"), 0.6),
          click(116.0, ip("misc"), 0.6), click(121.0, ip("misc"), 0.6),
          click(124.0, ip("mesh"), 0.6), click(127.0, px(100, MESH_SUB["render"]), 0.6),
          {"t": 130.0, "wheel": -3, "at": px(230, 850)}, {"t": 133.0, "wheel": 3, "at": px(230, 850)},
          click(135.0, px(100, MESH_SUB["render"]), 0.6), click(137.5, px(100, MESH_SUB["info"]), 0.6), click(142.0, px(100, MESH_SUB["info"]), 0.6),
          click(144.0, ip("mesh"), 0.6),
          click(147.0, SCRIPTS_MENU), click(149.8, SCRIPTS_PLAY, 1.0),                    # Play again
          {"t": 151.4, "editor": False}], [
    S(0.3, 3.3, "Hi! A quick tour of Brakeza3D: a C++ engine with OpenGL deferred rendering, scripted in Lua."),
    S(3.8, 3.2, "Geometry goes into a G-Buffer first, and the lighting comes after."),
    S(7.2, 3.0, "Per-pixel lights, shadows and a post-process on top."),
    S(11.0, 2.4, "Now the editor. First, the project panel."),
    S(13.6, 3.8, "Project scenes: every scene the project can load. This whole tour is one project of 35 scenes."),
    S(17.6, 3.8, "Project scripts: global scripts that run in every scene, like this demo browser."),
    S(21.6, 4.4, "And the project settings: name, file, gravity and resolution."),
    S(32.0, 4.2, "Layouts rearrange the editor: this one, Design, gives the scene all the room."),
    S(40.7, 5.2, "Script Controls, Stop: with the scripts stopped, the scene becomes editable, with its grid and gizmos."),
    S(47.2, 3.0, "Click an object and it gets a green outline."),
    S(50.7, 5.8, "Pick the Move tool and drag an arrow of the gizmo: the pawn slides up along that axis."),
    S(59.2, 2.6, "Select another one, switch to Rotate..."),
    S(65.2, 4.0, "...and turn it with the rotation ring."),
    S(71.7, 5.2, "Back in the Default layout, Object Properties is docked on the left: the inspector of the selected cube."),
    S(77.7, 3.7, "Transformations: position, rotation and scale."),
    S(81.7, 4.8, "Drag a field and the value changes live, right in the scene."),
    S(90.2, 3.4, "Render settings: alpha, selectable, lighting and raster state..."),
    S(93.6, 3.6, "...down to face culling and the rest of the flags."),
    S(100.7, 4.0, "Attached objects: the children hanging from this one."),
    S(108.2, 4.4, "Collider: the physics body of the object and its mode."),
    S(116.2, 4.4, "Misc: the object's id, its picking colour and its distance to the camera."),
    S(124.2, 2.6, "Mesh3D: everything about the model."),
    S(127.2, 3.0, "Mesh render options: pipeline, culling, mipmaps and emission..."),
    S(130.2, 4.4, "...scroll for the rest."),
    S(137.7, 4.0, "Mesh information: the model file, its submeshes and materials."),
    S(147.2, 4.0, "Script Controls, Play: the scripts run again."),
    S(152.0, 3.0, "And on with the tour."),
], end=155.6)
# Object browser (left panel of the editor, 1920x1009 window): tree arrows and the enable checkbox of each light, as fractions
OB_X, OB_SPOT, OB_POINT = 0.0135, 0.2825, 0.2577
CB_X = 0.0224
CB_LIGHT, CB_SPOT1, CB_SPOT2 = 0.2795, 0.3280, 0.3518


def cb(t, y):
    return {"t": t, "click": [CB_X, y], "button": "left"}


# object browser rows (fractions of the 1009 px window): Mesh 3D 0.2329, its child 0.2577, Light Point 0.2825, Light Spot 0.3076 (once Mesh 3D is expanded)
plan(18, [{"t": 4.4, "editor": True},
          click(5.6, [OB_X, 0.2329], 0.5),                             # expand "Mesh 3D"
          click(7.6, [OB_X, 0.3076], 0.5),                             # expand "Light Spot"
          click(10.0, [0.075, 0.3328], 0.6),                           # select the spotlight: its debug cone appears in the scene
          click(14.2, [0.075, 0.2577], 0.5),                           # select the room mesh instead: the cone goes away
          click(16.0, [OB_X, 0.3076], 0.5), click(17.0, [OB_X, 0.2329], 0.5),   # leave the trees as found: "Light Spot" and "Mesh 3D" collapsed
          {"t": 18.4, "editor": False},
          K(20.6, "1"), K(22.0, "2"), K(23.4, "1"), K(24.8, "2"), K(26.2, "3"), K(27.6, "3")], [
    S(0.3, 4.0, "A global directional light turns in a 30-second loop to fake day and night."),
    S(5.0, 4.4, "Back in the editor: the object browser groups the scene by type, lights included."),
    S(10.2, 3.8, "Select a spotlight and the editor draws its debug cone: the volume of space it lights."),
    S(14.4, 2.2, "Select something else and it goes away."),
    S(18.6, 1.8, "Back to the demo."),
    S(20.4, 4.4, "Keys 1, 2 and 3 switch the lights off and on from Lua: setEnabled on each one."),
    S(25.0, 3.8, "The editor's light checkboxes call exactly the same thing."),
], end=29.4)

# Editor GUI (1920x1009 window): "Scene Shaders" node of the project panel, rows of the list with a green dot (enabled switch)
EX, DOT_X = 0.796, 0.9005
ROWS = [0.4004, 0.4242, 0.448, 0.4718, 0.4955]


def dot(t, i):
    return {"t": t, "click": [DOT_X, ROWS[i]], "button": "left"}


PROF_X = 0.625
PROF_ITEM = [PROF_X, 0.4044]
TAB_OVERVIEW, TAB_RENDER, TAB_SCRIPTS, TAB_RES, TAB_PHYS = [0.3156, 0.4113], [0.35, 0.4113], [0.4130, 0.4113], [0.4521, 0.4113], [0.4911, 0.4113]
CB_GPU, CB_SCRIPT_TIMING, PROF_CLOSE = [0.3036, 0.4886], [0.3036, 0.4500], [0.7037, 0.2141]
WIN_MENU19 = [0.5953, 0.0119]

plan(19, [{"t": 0.6, "move": [0.5, 0.5, 0.25, 0.25], "dur": 1.0}, {"t": 1.8, "move": [0.25, 0.25, 0.75, 0.25], "dur": 0.9},
          {"t": 2.9, "move": [0.75, 0.25, 0.75, 0.75], "dur": 0.7}, {"t": 3.8, "move": [0.75, 0.75, 0.25, 0.75], "dur": 0.5},
          {"t": 4.4, "editor": True},
          {"t": 5.6, "click": [EX, 0.337], "button": "left"},
          dot(9.4, 0), {"t": 10.3, "move": [DOT_X, ROWS[0], 0.30, 0.20], "dur": 0.8},
          dot(11.4, 1), {"t": 12.3, "move": [DOT_X, ROWS[1], 0.65, 0.20], "dur": 0.8},
          dot(13.4, 2), {"t": 14.3, "move": [DOT_X, ROWS[2], 0.45, 0.45], "dur": 0.8},
          dot(15.6, 0), dot(16.8, 1), dot(18.0, 2),
          # ---- the profiler, with the eight chained shaders as the load ----
          click(20.4, WIN_MENU19), click(23.0, PROF_ITEM, 1.0),             # Windows > Profiler
          {"t": 27.5, "move": [0.34, 0.47, 0.52, 0.62], "dur": 3.5},        # over the per-component table
          click(32.0, TAB_RENDER),
          click(36.5, CB_GPU),                                              # GPU timing on
          K(42.5, "0", 0.2), K(46.5, "9", 0.2),                             # every shader off, then on: watch Post-Processing
          click(52.0, TAB_SCRIPTS), click(55.5, CB_SCRIPT_TIMING),          # per-script timing on
          click(63.0, TAB_RES),
          click(70.0, TAB_PHYS),
          click(77.0, TAB_OVERVIEW),
          click(83.0, PROF_CLOSE),
          click(85.0, [EX, 0.337]),                                         # collapse "Scene Shaders" again: the tree keeps its open state between demos
          # ---- last: stop the scripts to free the editor camera and zoom on the pawn, so the effects are seen to apply to the whole scene. Play (which makes the
          # demo browser load the demo again, duplicating the scene in the project panel) comes last, with the editor closing right after ----
          click(88.0, SCRIPTS_MENU), click(90.6, SCRIPTS_STOP, 1.0),
          {"t": 94.0, "wheel": 4, "at": [0.49, 0.40]}, {"t": 95.4, "wheel": 4, "at": [0.49, 0.40]},
          {"t": 97.8, "wheel": -4, "at": [0.49, 0.40]}, {"t": 99.2, "wheel": -4, "at": [0.49, 0.40]},
          click(101.8, SCRIPTS_MENU), click(104.4, SCRIPTS_PLAY, 1.0),
          {"t": 105.7, "editor": False}], [
    S(0.3, 4.0, "Eight full-screen shaders in a chain: the image is handed from one to the next through two ping-pong buffers."),
    S(5.0, 4.0, "Here's the engine's own editor. Under 'Scene Shaders' you can see all eight of them."),
    S(9.1, 4.0, "Each green dot is the shader's on/off switch. Turn one off and its quarter of the screen goes back to normal."),
    S(13.2, 2.1, "Another one... they're independent, so you can mix them freely."),
    S(15.4, 4.0, "And back on. This is how you'd debug a long effects chain: switch things off one by one."),
    S(20.6, 6.4, "This demo is the heaviest on post-processing, so it's the perfect excuse to open the profiler: Windows, Profiler."),
    S(27.6, 4.2, "The overview: frame time, memory, and the cost of every engine component each frame."),
    S(32.2, 4.0, "The Render tab breaks the frame into its passes: scripts, G-Buffer, shadows, lights, transparencies, post-processing."),
    S(36.7, 5.6, "Switch on GPU timing and every pass also shows the time the graphics card spent on it."),
    S(42.4, 3.8, "Watch the Post-Processing row while I turn all eight shaders off..."),
    S(46.4, 5.2, "...and on again. Each shader in the chain adds a fullscreen pass."),
    S(52.2, 3.2, "Scripts: per-script timing is off by default, so measuring costs nothing until you ask for it."),
    S(55.7, 6.8, "Switched on, it lists how long each Lua script takes every frame. That's how the slow ones get found."),
    S(63.2, 6.4, "Resources: the thread pools that load assets in the background, with their queues and workloads."),
    S(70.2, 6.4, "Physics: how many Bullet bodies there are by type, their collision groups, and a switch for the debug wireframe."),
    S(77.2, 5.2, "And back to the overview. Everything here can be turned off with --no-profiler when you don't need it."),
    S(88.2, 4.8, "One more thing: stop the scripts, and the editor camera is free."),
    S(94.1, 3.2, "Zoom in on the pawn: every quarter has its own effect."),
    S(97.9, 3.2, "And out: the whole scene is filtered, not just the pawn."),
    S(101.9, 2.8, "Play again, and on with the tour."),
], end=108.0)
# Demo 19, profiler, Render tab: besides the GPU timing, go through its other sections (headers at x = 0.349 once the pipeline one is collapsed, y = 0.451 / 0.4816 /
# 0.5124 / 0.543): Frame Buffers (layers, G-Buffer, picking), the Post-Processing Chain (texture in / out of every shader) and the OpenGL status.
_v = P["19"]
_T0, _SH = 52.0, 49.0
for _e in _v["steps"] + _v["subs"]:
    if _e["t"] >= _T0:
        _e["t"] = round(_e["t"] + _SH, 2)
HX = 0.349
VIEW = [0.40, 0.55]
_v["steps"] += [click(52.0, [HX, 0.451], 0.5),                                   # collapse "Render Pipeline"
                click(54.5, [HX, 0.4816], 0.5),                                  # open "Frame Buffers"
                {"t": 58.6, "wheel": -5, "at": VIEW}, {"t": 62.0, "wheel": -5, "at": VIEW}, {"t": 66.0, "wheel": -8, "at": VIEW}, {"t": 70.0, "wheel": 20, "at": VIEW},
                click(71.5, [HX, 0.4816], 0.5),                                  # close it
                click(73.5, [HX, 0.5124], 0.5),                                  # open "Post-Processing Chain"
                {"t": 77.8, "wheel": -6, "at": VIEW}, {"t": 83.0, "wheel": 20, "at": VIEW},
                click(84.5, [HX, 0.5124], 0.5),
                click(86.5, [HX, 0.543], 0.5),                                   # open "OpenGL Status"
                {"t": 91.0, "wheel": -3, "at": VIEW}, {"t": 96.0, "wheel": 10, "at": VIEW},
                click(97.0, [HX, 0.543], 0.5)]
_v["subs"] += [S(52.1, 2.3, "The Render tab has more: Frame Buffers..."),
               S(54.6, 3.8, "Every offscreen target the engine renders to, as live thumbnails."),
               S(58.5, 3.3, "The layers of the final image: global, scene, background, foreground and the UI."),
               S(62.1, 3.8, "The G-Buffer of the deferred renderer: positions, normals, albedo and depth."),
               S(66.1, 3.8, "And the picking buffer: a colour ID per object, how a click knows what it hit."),
               S(73.6, 4.0, "Post-Processing Chain: for every shader, the texture it reads and the one it writes."),
               S(77.9, 4.8, "That's the ping-pong: each effect feeds the next, and the last one is copied to the global buffer."),
               S(86.7, 3.8, "OpenGL Status: the limits of this GPU and driver..."),
               S(91.1, 4.0, "...and the vendor, the renderer and the exact GL version in use.")]
_v["subs"].sort(key=lambda e: e["t"]); _v["steps"].sort(key=lambda e: e["t"])
_v["end"] = round(_v["end"] + _SH, 1)


# Editor GUI of the Node shaders demo (1920x1009 window, default layout), as fractions
SH_NODE, SH_EDIT_DUOTONE = [0.8099, 0.3370], [0.9151, 0.4242]      # "Scene Shaders" node of the project panel / edit icon of Duotone
NODE_EXPAND, NODE_CANVAS, NODE_CLOSE = [0.2917, 0.2963], [0.4688, 0.4955], [0.8771, 0.1566]
SC_NODE, SC_EDIT = [0.7964, 0.3053], [0.9292, 0.3687]              # "Scene Scripts" node / edit icon of the script
CODE_CLOSE, CODE_AREA = [0.7943, 0.1962], [0.5730, 0.3964]
OB_MESH_ARROW, OB_LAVA, INSPECTOR_TRANSFORM, POS_Y_FIELD = [0.0135, 0.2329], [0.0677, 0.4222], [0.0495, 0.6185], [0.1484, 0.6780]
EDIT_MENU, EDIT_UNDO, EDIT_REDO = [0.0906, 0.0119], [0.0833, 0.0446], [0.0833, 0.0654]

FILE_BROWSER_ITEM = [0.625, 0.1903]   # Windows menu (x 1200 px), File Browser row (y 192 px)

NODE_WIN_X = [0.7141, 0.1516]      # title-bar X of the floating "Code/Nodes editor" window (closing its tab leaves the empty window open)
LAVA_VIEW = [0.5, 0.4658]          # the Lava sphere in the 3D view once the scripts are stopped
INSP_ROW_TRANSF = px(47, 647)
POS_Y = [0.1432, 0.7007]           # Position Y of the docked inspector (Transformations open)
plan(20, taps(["RIGHT", "RIGHT", "RIGHT", "RIGHT", "RIGHT", "SPACE"], 3.2, 1.6)
     + [{"t": 12.6, "editor": True},
        click(15.0, LAYOUTS_MENU), click(17.6, LAYOUT_DEFAULT, 1.0),  # Layouts > Default re-docks everything: the project panel and the object panels are where the clicks below expect them
        click(21.0, SH_NODE),                                         # expand "Scene Shaders" in the project panel
        click(23.5, SH_EDIT_DUOTONE),                                 # edit icon of the Duotone row: opens the node editor on the screen effect
        click(26.5, NODE_EXPAND),                                     # full view of the graph
        click(29.0, NODE_CANVAS), K(30.4, "F", 0.2),                 # F = navigate to content: the whole graph, centred
        click(37.0, NODE_CLOSE),                                      # closes the graph tab...
        click(39.0, NODE_WIN_X),                                      # ...and the empty editor window it leaves behind
        # ---- object shaders: stop the scripts, select an object that wears one, open its node graph ----
        click(42.0, SCRIPTS_MENU), click(44.6, SCRIPTS_STOP, 1.0),   # Script Controls > Stop scripts
        click(47.5, OB_MESH_ARROW),                                   # expand "Mesh 3D"
        click(50.0, OB_LAVA),                                         # the Lava sphere wears a node shader
        click(53.0, [0.4, 0.7574], 0.6),                              # tab "Object Shaders" of the bottom panel
        click(55.5, [0.2188, 0.8177], 0.6),                           # expand the Lava row of the Object Shaders panel
        click(58.5, [0.7031, 0.8177], 0.8),                           # "Edit nodes" icon of the Lava shader row (the red X next to it removes the shader: never aim right of it)
        click(62.0, [0.6875, 0.396]), K(63.4, "F", 0.2),             # F = navigate to content: the whole graph, centred
        click(72.0, [0.7031, 0.2954]),                                # close the graph tab...
        click(74.0, NODE_WIN_X),                                      # ...and the empty editor window
        click(77.0, SCRIPTS_MENU), click(79.6, SCRIPTS_PLAY, 1.0),    # Play again (BEFORE opening the code editor: after it, the demo's script outlived its scene)
        click(84.0, SC_NODE), click(86.5, SC_EDIT),                  # the scene script opens in the code editor (coding layout)
        {"t": 90.5, "wheel": -4, "at": CODE_AREA}, {"t": 93.0, "wheel": -4, "at": CODE_AREA},
        click(97.0, CODE_CLOSE),
        # the code editor switched the GUI to the coding layout: back to Default from the Layouts menu
        click(99.5, LAYOUTS_MENU), click(102.0, LAYOUT_DEFAULT, 1.0),
        # ---- scene objects: select, inspect, edit with the inspector, undo / redo (history is only recorded with the scripts stopped) ----
        click(105.5, SCRIPTS_MENU), click(108.1, SCRIPTS_STOP, 1.0),
        click(111.5, LAVA_VIEW, 0.8),                                 # select the Lava sphere in the view: the green outline and the gizmo
        click(114.5, INSP_ROW_TRANSF, 0.6),                           # open its Transformations in the docked inspector
        {"t": 118.0, "drag": POS_Y + [POS_Y[0] + 0.0234, POS_Y[1]], "button": "left", "dur": 1.4, "pre": 0.8},
        click(122.5, EDIT_MENU), click(125.0, EDIT_UNDO, 1.0),       # undo: the object goes back
        click(129.0, EDIT_MENU), click(131.5, EDIT_REDO, 1.0),       # redo: and forward again
        click(135.0, INSP_ROW_TRANSF, 0.6),                           # leave the inspector as found: Transformations closed
        click(138.0, SCRIPTS_MENU), click(140.6, SCRIPTS_PLAY, 1.0),  # Play again, so the next demo finds the scripts running
        {"t": 145.5, "editor": False}], [
    S(0.3, 3.0, "Node shaders: fourteen objects, each wearing a different one, and twelve full-screen effects."),
    S(3.3, 7.4, "Left and right switch the screen effect: sepia, duotone, night vision, thermal, chromatic aberration, underwater..."),
    S(15.2, 5.4, "Back in the editor. Every effect is a node graph: this is the Duotone one, opened from the Scene Shaders list."),
    S(26.7, 2.6, "Let me give it more room..."),
    S(29.4, 7.4, "...and press F to centre the whole graph: the scene texture goes through Grayscale, is mixed with two colours, and reaches the Output node."),
    S(42.2, 2.6, "Now the objects. First, stop the scripts..."),
    S(50.2, 2.6, "...and select an object that wears an object shader: Lava."),
    S(53.2, 4.4, "Its row in the Object Shaders panel expands to list the nodes and how they connect."),
    S(58.7, 3.4, "Edit nodes opens the object's own graph, live, in the same node editor."),
    S(62.2, 8.0, "F centres it: time and UV coordinates drive an offset, and the result is the lava texture, wired to a mesh instead of the screen."),
    S(77.2, 3.4, "Play scripts again, and the demo carries on."),
    S(84.2, 4.8, "Scripts open in the code editor, with syntax highlighting. This is the Lua script that drives this very demo."),
    S(90.6, 5.0, "It's a plain Lua file: the screen effects, the keys, and how each shader is switched on by its label."),
    S(99.7, 5.0, "Back to the default layout. Now the scene itself: stop the scripts, and the objects can be edited."),
    S(111.7, 2.6, "Click an object in the view, here the Lava sphere..."),
    S(114.7, 3.0, "...and the docked inspector shows its transform, render settings, collider and more."),
    S(118.2, 4.0, "Drag a field and the object moves right there in the scene. Every change goes into the undo history."),
    S(122.7, 6.0, "Edit, Undo (Ctrl+Z) puts it back where it was. The history size is configurable."),
    S(129.2, 5.0, "And Redo (Ctrl+Y) brings the move back."),
    S(138.2, 3.4, "Play again, and on with the tour."),
], end=148.0)


# Demo 20, object shaders: after Lava, walk through more objects, each with its own node graph (select it, Edit nodes, centre the graph with F, close the tab).
# The Object Shaders panel row of every object sits at the same place (collapsed: dot 1293, reload 1327, edit 1361 px at y 825).
EDIT_NODES_ICON, TAB_CLOSE = [0.7089, 0.8177], [0.7031, 0.2954]
OBJ_SHADERS = [(258, "BurnDissolve"), (306, "DiagonalStripes"), (330, "FireGlow"), (402, "Hologram"), (450, "NeonPulse"), (474, "NormalColors"), (378, "HitFlash")]
T_OBJ0, T_OBJ_STEP = 74.0, 8.6
_n = len(OBJ_SHADERS)
_dt = _n * T_OBJ_STEP + 0.4            # the rest of the demo moves this much later
_v = P["20"]
for _e in _v["steps"] + _v["subs"]:
    if _e["t"] >= 74.0 and not (_e.get("click") and abs(_e["t"] - 74.0) < 0.01 and _e["click"] == NODE_WIN_X):
        _e["t"] = round(_e["t"] + _dt, 2)
_v["steps"] = [_e for _e in _v["steps"] if not (_e.get("click") == NODE_WIN_X and _e["t"] < 74.5)]      # the window stays open between the graphs
for _i, (_y, _name) in enumerate(OBJ_SHADERS):
    _t = T_OBJ0 + _i * T_OBJ_STEP
    _v["steps"] += [click(_t, [OB_LAVA[0], round(_y / 1009, 4)], 0.5), click(_t + 1.8, EDIT_NODES_ICON, 0.7), click(_t + 3.8, [0.6875, 0.396]), K(round(_t + 4.6, 2), "F", 0.2),
                    click(_t + 7.3, TAB_CLOSE, 0.5)]
    _v["subs"].append(S(round(_t + 0.2, 2), 3.2 if _i else 4.4, ("Next object, next graph: %s." % _name) if _i else "More objects wear other node shaders: %s..." % _name))
_v["steps"].append(click(T_OBJ0 + _n * T_OBJ_STEP, NODE_WIN_X, 0.5))
_v["steps"].sort(key=lambda e: e["t"])
_v["subs"].sort(key=lambda e: e["t"])
_v["end"] = round(_v["end"] + _dt, 1)

# Demo 20: Play makes the demo browser load the demo again (a second copy of the scene in the project panel, shown to the end), so scripts are stopped only once:
# screen shader graph -> code editor (scripts running) -> Stop -> object shaders -> inspector edit / undo -> Play -> editor off.
_v = P["20"]
def _in(e, lo, hi): return lo <= e["t"] < hi
_A  = lambda e: e["t"] < 41.0
_B  = lambda e: _in(e, 41.0, 136.0)                    # object shaders (starts with Stop at 42.0)
_C  = lambda e: _in(e, 143.0, 164.0)                   # code editor + back to Default layout
_D  = lambda e: _in(e, 169.0, 197.0)                   # object select / inspector / undo (the Stop at 166-169 is dropped: already stopped)
_END= lambda e: e["t"] >= 197.0
def _shift(lst, f, d): return [dict(e, t=round(e["t"] + d, 2)) for e in lst if f(e)]
_dC = 42.0 - 144.6
_dB = 63.4 - 42.0
_dD = 158.0 - 172.1 + 0.0
_play0 = 198.6 + _dD
for _key in ("steps", "subs"):
    _src = _v[_key]
    _new = _shift(_src, _A, 0.0) + _shift(_src, _C, _dC) + _shift(_src, _B, _dB + 0.0) + _shift(_src, _D, _dD)
    if _key == "steps":
        _new += [dict(e, t=round(e["t"] + _dD, 2)) for e in _src if 197.0 <= e["t"] < 205.0]       # Play
        _new.append({"t": round(_play0 + 3.3, 2), "editor": False})
    else:
        _new += [dict(e, t=round(e["t"] + _dD, 2)) for e in _src if 197.0 <= e["t"] < 205.0]
    _v[_key] = sorted(_new, key=lambda e: e["t"])
for _e in _v["subs"]:
    if _e["text"].startswith("Back to the default layout."):
        _e["text"] = "Back to the default layout."
_v["end"] = round(_play0 + 3.3 + 2.8, 1)


plan(21, [K(round(2.0 + 1.5 * i, 2), "LEFT", 0.15) for i in range(14)], [
    S(0.3, 3.2, "Thirty particle emitters on a carousel, starting with confetti. I'll flick through them backwards."),
    S(3.6, 4.3, "Black hole, fireworks, bubbles... every one is just a set of emitter parameters, no code."),
    S(8.1, 2.9, "Lightning, then a magic portal: a torus stood upright and swirling."),
    S(11.2, 4.3, "Particles can be born on a mesh surface, on a cone, a sphere or a ring."),
    S(15.7, 4.9, "And the newer spiral and vortex controls: a sprinkler, a galaxy with three arms, a tornado..."),
    S(20.9, 3.4, "...and a helix. Thirty effects, all made of the same emitter."),
])
plan(22, taps(["1", "2", "3"], 1.0, 2.2) + taps(["SPACE", "UP"], 8.0, 1.5), [
    S(0.3, 3.4, "Thirty meshes flying as a swarm. They follow three simple rules: separation, alignment and cohesion."),
    S(3.9, 3.4, "The red cube is a predator: the boids steer away from it."),
    S(7.8, 3.0, "I can pause it, or make it faster."),
])
plan(23, [], [
    S(0.3, 4.6, "Three emitters each spawn a physics cube every half second. The wireframes are Bullet's own debug view."),
    S(5.1, 4.4, "You can see the real collision shapes, and the contact points where the cubes touch."),
    S(9.7, 4.0, "The engine keeps only the newest cubes, so the physics world stays light."),
])
plan(24, [K(1.6, "F"), K(4.2, "A", 0.3), K(5.4, "F"), K(7.8, "E", 0.9), K(9.4, "F"),
          K(11.6, "P"), K(12.8, "F"), K(14.2, "D", 0.5), K(15.6, "F"),
          {"t": 17.6, "click": [0.45, 0.45], "button": "left"},
          K(19.4, "F"), K(21.4, "P"), K(22.6, "F"), K(23.8, "X", 0.15), K(24.8, "F")], [
    S(0.3, 3.8, "The launcher shoots bouncy physics projectiles at the base. A and D aim sideways, W and S up and down, with a smooth turn."),
    S(4.2, 3.2, "I nudge the aim and fire again: the shot arcs under gravity, hits the floor and bounces."),
    S(7.6, 3.6, "E adds power, so it flies further before the first bounce."),
    S(11.3, 5.4, "Now the physics debug view, with P: the wireframes are the real collision shapes Bullet is using."),
    S(17.2, 3.4, "A left click fires too."),
    S(21.1, 2.3, "P again hides it."),
    S(23.6, 3.4, "X makes the projectile bigger: more mass to push things around."),
], end=27.5)
plan(25, [{"t": 1.0, "key": "W", "hold": 3.6},                                   # walk forward...
          {"t": 2.2, "drag": [0.5, 0.5, 0.58, 0.5], "button": "left", "dur": 1.2},     # ...turning the head a little to the right
          {"t": 5.2, "key": "S", "hold": 3.6},                                         # ...and back
          {"t": 6.2, "drag": [0.58, 0.5, 0.44, 0.5], "button": "left", "dur": 1.4},    # head to the left
          {"t": 9.6, "key": "D", "hold": 2.4},                                         # strafe right
          {"t": 10.2, "drag": [0.44, 0.5, 0.5, 0.47], "button": "left", "dur": 1.0},   # look a bit up
          {"t": 12.4, "key": "A", "hold": 2.6},                                        # strafe left
          {"t": 13.2, "drag": [0.5, 0.47, 0.5, 0.5], "button": "left", "dur": 1.0},
          {"t": 15.6, "key": "SPACE", "hold": 0.2},
          {"t": 16.4, "key": "W", "hold": 2.6}, {"t": 17.2, "drag": [0.5, 0.5, 0.56, 0.5], "button": "left", "dur": 1.2}], [
    S(0.3, 3.4, "A first-person starter kit: a kinematic capsule with the camera in its eyes."),
    S(4.0, 4.6, "W and S walk forward and back; holding the left mouse button and dragging turns the head."),
    S(9.0, 5.8, "While it moves, the view sways gently from side to side, step after step, and A and D strafe."),
    S(15.3, 3.4, "Space jumps. It's a plain object script, so you can copy it onto your own character."),
], end=20.6)
plan(26, [{"t": 1.0, "key": "W", "hold": 2.0}, {"t": 3.2, "key": "SPACE", "hold": 0.2},
          {"t": 4.0, "drag": [0.5, 0.5, 0.62, 0.5], "button": "left", "dur": 1.0}, {"t": 5.6, "key": "W", "hold": 2.0},
          {"t": 8.0, "drag": [0.5, 0.5, 0.38, 0.5], "button": "left", "dur": 1.0}, {"t": 9.4, "key": "W", "hold": 1.5}], [
    S(0.3, 3.6, "Third person: the camera follows from behind and always looks at the character."),
    S(3.9, 4.6, "Dragging the mouse turns the character, and the camera swings round with it."),
    S(9.2, 3.0, "Same idea: one object script, ready to reuse."),
])
# 2D platformer: run right (D), clear the block on the way with a full jump plus wall-jump taps (against its face the player counts as "on a wall"; measured by
# hand), climb the tall wall at the right end the same way, then run all the way back to the left (A) and finish with an attack.
def _hops(t0, t1, period=2.8, taps=4):
    """A full jump followed by wall-jump taps, repeated: whenever the player is up against the block or the wall, one of the rounds lifts it over / up
    (the moment it gets there varies from run to run by a few seconds)."""
    out, t = [], t0
    while t < t1:
        out.append({"t": round(t, 2), "key": "SPACE", "hold": 1.0})
        out += [{"t": round(t + 1.3 + 0.4 * i, 2), "key": "SPACE", "hold": 0.1} for i in range(taps)]
        t += period
    return out


plan(27, [{"t": 4.0, "key": "D", "hold": 20.0}]                                           # run right (the player starts moving ~3 s after "ready")
     + [{"t": 5.0, "key": "SPACE", "hold": 0.5}]
     + [{"t": round(6.0 + 0.45 * i, 2), "key": "SPACE", "hold": 0.1} for i in range(36)]  # a tap every 0.45 s: hooked on the block / the wall, one wall jump after another
     + [{"t": 23.0, "key": "J", "hold": 0.15}], [
    S(0.3, 3.6, "A 2D platformer inside a 3D level. No rigid bodies here: the player is moved by script."),
    S(4.0, 4.4, "Raycasts find the floor and the walls, which gives variable jump height, coyote time and jump buffering."),
    S(9.0, 5.0, "Up against a wall the player hooks on and slides down it. Space is a wall jump: it kicks off, D brings it back."),
    S(15.0, 5.0, "Jump after jump, it climbs the block and the wall."),
    S(22.9, 1.8, "And J attacks."),
], end=25.6)

# Editor GUI (1920x1009 window) coordinates as fractions: menu bar y, the Windows menu list (x 1200 px) and the Add Object menu
WIN_MENU, ADD_MENU, SCRIPT_MENU = [0.5953, 0.0119], [0.1927, 0.0119], [0.1375, 0.0119]
ITEM_X = 0.625




plan(28, [{"t": 1.0, "move": [0.5, 0.5, 0.8, 0.4], "dur": 0.8}, {"t": 1.8, "key": "W", "hold": 1.5},
          {"t": 3.4, "click": [0.8, 0.4], "button": "left"}, {"t": 3.9, "click": [0.8, 0.4], "button": "left"},
          {"t": 4.3, "key": "D", "hold": 1.5}, {"t": 4.4, "move": [0.8, 0.4, 0.3, 0.6], "dur": 1.5},
          {"t": 6.2, "click": [0.3, 0.6], "button": "left"}, {"t": 6.7, "click": [0.3, 0.6], "button": "left"},
          {"t": 8.2, "key": "S", "hold": 1.5},
          {"t": 9.9, "key": "R", "hold": 0.2}, {"t": 10.6, "click": [0.5, 0.3], "button": "left"},
          # ---- final section: the editor's windows and widgets ----
          # Everything through the menus (the F-key shortcuts need RCtrl while scripts run, and did not take effect in the recordings).
          # The pointer rests ~1 s on an item before pressing and ~2.5 s separate the menu button from the item.
          {"t": 12.4, "editor": True},
          click(15.0, LAYOUTS_MENU), click(17.5, LAYOUT_CODING, 1.0),
          click(21.5, LAYOUTS_MENU), click(24.0, LAYOUT_DESIGN, 1.0),
          click(28.0, LAYOUTS_MENU), click(30.5, LAYOUT_DEFAULT, 1.0),
          # the Windows menu: just look at its list (nothing is toggled: the layouts already show what they need)
          click(34.5, WIN_MENU),
          {"t": 37.0, "move": [ITEM_X, 0.1189, ITEM_X, 0.50], "dur": 6.0},
          click(45.0, WIN_MENU),
          # the menu bar: open the first menu and slide along it (ImGui switches menus on hover)
          click(48.5, SCRIPT_MENU),
          {"t": 50.1, "move": [SCRIPT_MENU[0], SCRIPT_MENU[1], WIN_MENU[0], WIN_MENU[1]], "dur": 9.0},
          click(60.5, ADD_MENU),
          {"t": 62.1, "move": [0.224, 0.045, 0.224, 0.40], "dur": 3.0},
          click(66.3, ADD_MENU),
          {"t": 68.0, "editor": False}], [
    S(0.3, 4.0, "A twin-stick shooter: one stick moves, the other aims. It works with a gamepad, or keyboard and mouse like now."),
    S(4.5, 3.8, "The mouse position becomes a world direction through a screen-to-world conversion."),
    S(8.5, 3.6, "R reloads, and the targets come back after a few seconds."),
    S(12.6, 2.2, "Last stop: the editor. Everything in it is a dockable window."),
    S(15.2, 6.0, "The Layouts menu rearranges the whole GUI. This one, Coding, puts the code editor in the middle."),
    S(21.7, 6.0, "Design gives the scene all the space it can get..."),
    S(28.2, 5.0, "...and Default brings back the usual arrangement. F6, F7 and F8 do the same, holding Right Ctrl while the scripts run."),
    S(34.7, 5.0, "The Windows menu lists every window of the editor: scene objects, properties, project, files, logs, profiler..."),
    S(40.2, 4.6, "Each one can be shown, hidden or docked wherever you like."),
    S(48.7, 11.5, "The menu bar holds the rest: script controls, add object, video, colliders, illumination, camera, sound, logs, layouts, interface..."),
    S(60.7, 5.2, "Add Object creates any entity type of the engine: meshes, animated models, lights, particle emitters, sound sources..."),
    S(68.6, 4.6, "And that's the tour. Every demo is a small, readable Lua script. Thanks for watching!"),
], end=74.5)

SOLO_BTN = [0.7742, 0.1286]       # UI Manager title bar, top-right icon next to the docking one: "Close all others" / "Restore windows" (same spot in both states)
# The demo's widgets sit in the middle of the screen, so the UI Manager window (isolated, x 414..1506) is slid 696 px to the right while editing:
# everything inside it shifts by WX. It goes back before the "restore" icon, which would otherwise be off screen.
WX = 696 / 1920
WIN_TITLE_HOME, WIN_TITLE_AWAY = [0.3646, 0.1031], [0.3646 + WX, 0.1031]


def wx(x, y):
    return [round(x + WX, 4), y]


EL_X = 0.445 + WX
plan(29, [{"t": 1.0, "move": [0.3, 0.3, 0.7, 0.6], "dur": 2.0}, {"t": 3.6, "click": [0.5, 0.5], "button": "left"},
          {"t": 9.0, "editor": True},
          click(11.0, LAYOUTS_MENU), click(13.6, LAYOUT_DEFAULT, 1.0),                       # Layouts > Default: the window positions below depend on this layout
          click(17.0, [0.5953, 0.0119]), click(19.6, [0.6172, 0.4995], 1.0),                    # Windows > UI Manager (last entry)
          click(21.5, SOLO_BTN, 0.8),                                                        # isolated window mode: every other window closes
          {"t": 26.0, "drag": WIN_TITLE_HOME + WIN_TITLE_AWAY, "button": "left", "dur": 1.4, "pre": 0.6},   # slide the window aside: the widgets are in the middle
          click(29.5, wx(0.3125, 0.19), 0.5),                                                # the filter box of the widget list
          K(31.0, "E", 0.1), K(31.6, "X", 0.1), K(32.2, "A", 0.1), K(32.8, "M", 0.1), K(33.4, "P", 0.1),
          click(35.0, wx(0.265, 0.2535), 0.5),                                               # exampleCard (filter "examp": Array, Card, Nested)
          click(38.5, [EL_X, 0.2008], 0.5),                                                  # element bg
          click(41.0, wx(0.4375, 0.4647), 0.5),                                              # "Layout" header (opened first: it sits below)
          click(43.0, wx(0.4375, 0.4335), 0.5),                                              # "Identity & Type" header
          click(45.5, wx(0.5, 0.5748), 0.7),                                                 # the type selector: opens the list
          click(48.5, wx(0.5, 0.465), 0.5),                                                  # ... and closes it with a click on an empty spot of the window (never on an entry: that would change the type)
          click(51.0, [EL_X, 0.2216], 0.5),                                                  # element avatar
          click(54.0, [EL_X, 0.2424], 0.5),                                                  # element name
          click(57.0, [EL_X, 0.2632], 0.5),                                                  # element subtitle
          click(60.0, [EL_X, 0.2840], 0.5),                                                  # element hpBar
          click(63.0, wx(0.265, 0.2743), 0.5),                                               # exampleNestedWidget: drawn with drawWidget, so scale AND position come from its JSON
          {"t": 66.0, "drag": wx(0.3333, 0.7918) + wx(0.3703, 0.7918), "button": "left", "dur": 1.6, "pre": 0.8},   # Scale up
          {"t": 69.5, "drag": wx(0.3703, 0.7918) + wx(0.3333, 0.7918), "button": "left", "dur": 1.6, "pre": 0.8},   # ... and down
          {"t": 73.5, "drag": wx(0.3203, 0.8186) + wx(0.3516, 0.8186), "button": "left", "dur": 1.4, "pre": 0.8},   # X %
          {"t": 77.5, "drag": wx(0.3766, 0.8186) + wx(0.4078, 0.8186), "button": "left", "dur": 1.4, "pre": 0.8},   # Y %
          {"t": 82.0, "wheel": -3, "at": wx(0.31, 0.82)},                                    # scroll the setup: Background, Frame (9-slice), Border...
          {"t": 85.0, "wheel": -3, "at": wx(0.31, 0.82)},
          click(88.0, wx(0.2390, 0.7572), 0.5), click(91.0, wx(0.2390, 0.7572), 0.5),        # open the Frame section, and close it again
          {"t": 93.5, "drag": WIN_TITLE_AWAY + WIN_TITLE_HOME, "button": "left", "dur": 1.4, "pre": 0.6},   # the window back home, where its "restore" icon is
          click(96.5, SOLO_BTN, 0.8),                                                        # restore the windows (the UI Manager closes with it)
          {"t": 99.5, "editor": False}], [
    S(0.3, 4.0, "UI widgets are described in JSON files: a nested panel, a list, and single cards."),
    S(4.5, 4.0, "Lua only feeds them data every frame. The layout stays out of the code."),
    S(9.2, 1.8, "And here is the tool to edit them."),
    S(17.2, 4.0, "Windows, UI Manager: every widget loaded in the engine, in one list."),
    S(21.6, 4.2, "The icon at the window's top right isolates it: every other window closes and the scene stays in view."),
    S(26.2, 3.0, "I slide it to the right, so the widgets in the middle stay visible."),
    S(29.6, 5.0, "Type in the filter to narrow it down, and pick a widget."),
    S(35.1, 3.2, "A widget is a list of elements. Click one to see it."),
    S(38.6, 2.4, "The first one is a rect: the card's background."),
    S(41.2, 1.8, "Layout: where the element sits."),
    S(43.1, 2.3, "Identity and type: its id and what it is."),
    S(45.6, 2.8, "The type selector lists every kind of element: rect, image, text, bars..."),
    S(51.1, 2.8, "Then an image: the avatar."),
    S(54.1, 2.8, "Text elements: the name..."),
    S(57.1, 2.8, "...and the subtitle, filled in from Lua."),
    S(60.1, 3.0, "And a progress bar for the health."),
    S(63.1, 2.8, "Another widget: the nested panel, drawn from its JSON file."),
    S(66.1, 3.2, "Scale resizes the whole widget at once, with every element inside it."),
    S(69.6, 3.6, "Drag it back down and the widget shrinks again."),
    S(73.6, 7.6, "X % and Y % place it as a fraction of the window, so it fits any resolution."),
    S(82.2, 5.0, "Further down: background, frame, border and text settings."),
    S(88.2, 4.4, "The Frame section takes a 9-slice image: corners stay sharp and the edges stretch."),
    S(93.6, 2.8, "Window back in place..."),
    S(96.6, 2.8, "...and the same icon brings the other windows back."),
    S(100.0, 1.7, "Back to the demo."),
], end=103.0)
plan(30, taps(["0", "0", "1", "2", "3", "4", "5", "6", "R"], 1.2, 0.85), [
    S(0.3, 4.4, "Every way of drawing text: TTF auto-sized, TTF with a fixed size, centred, aligned, cached..."),
    S(4.8, 4.0, "The glyph atlas is the cheapest one, perfect for HUDs that change every frame."),
])
plan(31, [{"t": 1.0, "move": [0.2, 0.5, 0.8, 0.5], "dur": 3.0}, {"t": 4.3, "move": [0.8, 0.6, 0.3, 0.3], "dur": 2.0}]
     + taps(["1", "2", "3", "4", "0"], 7.0, 1.0) + taps(["H"], 12.2), [
    S(0.3, 4.4, "The cursor changes with whatever is under the mouse. The engine tells the script which object is picked."),
    S(5.0, 1.8, "It's the same pattern the RTS uses."),
    S(6.9, 4.6, "I can also force a specific cursor with 1 to 4, and 0 goes back to automatic."),
])
# Multi-selection scene, measured on a 1280x720 frame: cubes / spheres / rings sit in the upper-left half of the view, the big floor covers the rest
# (clicking there selects "Ground", so every click and the rectangle aim at objects).
MS_CUBE, MS_SPHERE_A, MS_SPHERE_B = [0.285, 0.299], [0.43, 0.289], [0.57, 0.299]
plan(32, [{"t": 1.0, "click": MS_CUBE, "button": "left"}, {"t": 2.4, "key": "LCTRL+click", "pos": MS_SPHERE_A},
          {"t": 3.5, "key": "LCTRL+click", "pos": MS_SPHERE_B}, {"t": 5.2, "key": "W", "hold": 1.2},
          {"t": 6.8, "key": "D", "hold": 1.0}, {"t": 8.2, "key": "O", "hold": 0.8}, {"t": 9.6, "key": "R", "hold": 0.15},
          {"t": 10.6, "keydrag": "LCTRL", "drag": [0.15, 0.33, 0.46, 0.55], "button": "left", "dur": 1.4},   # a rectangle around the four objects at the lower left
          {"t": 12.9, "key": "A", "hold": 1.2}, {"t": 14.6, "key": "Z", "hold": 0.8}], [
    S(0.3, 4.2, "Selecting things RTS-style: a click selects one object, and Ctrl plus click adds more to the group."),
    S(4.8, 4.0, "The group has its own centre, and I can move all of it at once with W A S D."),
    S(9.2, 1.1, "R clears the selection."),
    S(10.4, 4.6, "A rectangle selection with Ctrl and drag picks several at once."),
])
plan(33, [{"t": 1.0, "key": "T", "hold": 0.15},                                  # automatic camera: orbits the sources, always looking at the centre
          {"t": 18.0, "key": "1", "hold": 0.15}, {"t": 19.6, "key": "1", "hold": 0.15}, {"t": 21.0, "key": "2", "hold": 0.15},
          {"t": 22.6, "key": "2", "hold": 0.15}, {"t": 24.2, "key": "P", "hold": 0.15},
          {"t": 27.5, "key": "TAB", "hold": 0.15},
          {"t": 29.5, "key": "X", "hold": 2.5}, {"t": 33.0, "key": "Z", "hold": 3.0},
          {"t": 37.0, "key": "V", "hold": 3.0}, {"t": 41.0, "key": "C", "hold": 3.5}], [
    S(0.3, 5.4, "Positional sound: four looping sources in space. The camera is the listener, and volume falls off with distance."),
    S(6.0, 5.6, "The camera circles the sources, always looking at the centre: the volume of each one rises and falls as it gets closer."),
    S(12.6, 5.0, "The panel lists distance, volume and left/right pan of every source, and changes as the camera moves."),
    S(18.2, 3.6, "Keys 1 to 4 mute each source on its own."),
    S(24.4, 3.0, "P switches the left/right panning off and on."),
    S(27.7, 1.7, "TAB picks the source to tune."),
    S(29.7, 3.2, "X grows its inner radius: full volume now reaches farther."),
    S(33.2, 3.4, "Z shrinks it again."),
    S(37.2, 3.4, "V widens the outer radius: the sound now fades out over a longer distance."),
    S(41.2, 3.8, "And C narrows it. All of it live, with setInnerRadius and setOuterRadius."),
], end=46.0)
plan(34, [{"t": 1.0, "key": "V", "hold": 0.15}, {"t": 7.0, "key": "B", "hold": 0.15}, {"t": 11.5, "key": "F", "hold": 0.15}], [
    S(0.3, 5.2, "Playing an MP4 from a script, full screen first."),
    S(6.8, 4.4, "Now the same video in a framed plane, with the scene visible around it."),
    S(11.3, 4.2, "And I can switch between both while it plays."),
])
plan(35, taps(["1", "SPACE"], 1.0, 1.0) + [K(7.2, "SPACE", 0.15)] + taps(["2", "SPACE"], 8.2, 1.0)
     + taps(["SPACE"], 12.0) + taps(["DELETE", "DELETE", "R"], 13.0, 1.0), [
    S(0.3, 5.2, "A stress test: spawn static meshes, physics bodies, lights, particles, animated meshes, in batches."),
    S(6.0, 3.0, "The FPS readout tells how far the engine can go on this machine."),
    S(9.0, 4.6, "There's a Profiler window in the editor to see where each frame goes."),
    S(14.0, 3.0, "And that's the tour. Thanks for watching!"),
], end=17)

# Short "peeks" at the editor at the end of some demos, so the video shows it is Brakeza (the full editor demos are 17, 18, 19, 20, 28, 29)
PEEKS = {
    21: "The editor: every emitter on the carousel is a regular object you can select and tweak.",
    22: "The editor again: the swarm is a regular object, with its own inspector.",
    24: "The editor, with the physics running underneath.",
    27: "The editor around the 2D platformer.",
}
for n, text in PEEKS.items():
    v = P[str(n)]
    t = v["end"]
    v["steps"] = v["steps"] + [{"t": round(t, 1), "editor": True}, {"t": round(t + 3.4, 1), "editor": False}]
    v["subs"] = v["subs"] + [S(round(t + 0.3, 1), 3.0, text)]
    v["end"] = round(t + 4.6, 1)

bad = []
for n, v in P.items():
    ss = sorted(v["subs"], key=lambda x: x["t"])
    for a, b in zip(ss, ss[1:]):
        if a["t"] + a["d"] > b["t"] + 0.05:
            bad.append("demo %s: '%s' ends %.1f, next starts %.1f" % (n, a["text"][:28], a["t"] + a["d"], b["t"]))
if bad:
    raise SystemExit("overlapping subtitles:" + chr(10) + "  " + (chr(10) + "  ").join(bad))

out = os.path.join(os.path.dirname(os.path.abspath(__file__)), "demo_tour_plan.json")
json.dump(P, open(out, "w", encoding="utf8"), indent=1, ensure_ascii=False)
print(len(P), "demos,", round(sum(v["end"] + 4 for v in P.values()) / 60, 1), "min approx ->", out)
