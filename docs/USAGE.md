# Waffle — Engine Usage Guide

A small voxel engine with a Chladni-pattern-driven material system: surfaces
show real vibration-plate nodal figures instead of static textures, with
lighting that reacts to them like actual relief (bump mapping), not flat
paint.

## Building & running

```
cmake -B cmake-build-debug
cmake --build cmake-build-debug --target OpenGLSkeleton
./cmake-build-debug/OpenGLSkeleton
```

Run it from a directory where `../shaders/` resolves (shaders are loaded
from disk at runtime, not compiled in) — the `cmake-build-*` directories
already satisfy this.

## Controls

| Input | Action |
|---|---|
| Mouse | Look around |
| W / A / S / D | Move (horizontal, follows where you're facing) |
| **Double-tap Space** | Toggle flying on/off |
| Space (while flying) | Ascend |
| Shift (while flying) | Descend |
| Ctrl | Accelerate |
| Left click | Break the block under the crosshair |
| Right click | Place a block against the face under the crosshair |

There's no gravity/collision yet — flight is the only way to move. A
green wireframe box previews where a placed block would land.

## Architecture map

| Module | Role |
|---|---|
| `Application` | Owns the window/camera/UI, runs the main loop |
| `Window`, `Renderer` | GLFW window + base GL state (depth test, culling, blending, vsync, MSAA) |
| `Shader` | Loads/compiles GLSL, supports `#include "file.glsl"` between shader files |
| `Mesh` | VAO/VBO/EBO wrapper; `UpdateData()` re-uploads after an edit |
| `Camera` | Free-fly controls; `ProcessKeyboardInput`/`ProcessMouseInput` |
| `Chunk` | Voxel grid (`voxels[]`, where each byte is a **material ID**, 0 = air); `Raycast()` does a DDA voxel-grid raycast |
| `ChunkMesher` | Greedy meshing: turns solid voxel faces into merged quads, carrying color/normal/material-ID per vertex |
| `BuildTool` | Minecraft-style break/place: raycasts every frame, edits the chunk + remeshes on click, draws the placement preview |
| `Light` / `LightManager` | Directional + point lights, applied as a uniform array (see `shaders/triangle.frag`) |
| `ChladniController` | Per-material pattern state machine: holds a clean integer `(n, m)` mode pair, crossfades to a new one over time |
| `Material` / `MaterialManager` | Per-material look (colors, pattern scale/line width/bump strength) + its own `ChladniController` |

## How the material/pattern system works

Each **voxel's byte value is a material ID** (`Chunk::GeneratePillarField`
writes `1` for floor, `2` for walls). The mesher carries that ID through to
each vertex; `triangle.frag` looks it up in a `ChladniMaterial uMaterials[]`
array and renders that material's own Chladni figure, colors, and pattern
scale — so different surfaces can look and behave completely differently.

The pattern itself (`shaders/chladni.glsl`) is the classic two-mode
superposition `sin(nπu)sin(mπv) − sin(mπu)sin(nπv)`. Its **analytic
gradient** is used to tilt the lighting normal (real bump mapping, not a
texture), and screen-space derivatives fade detail out when it would
otherwise alias (far away / grazing angles) — see the `detailFade` logic
in `triangle.frag`.

### Add a light

```cpp
lights.AddDirectionalLight(direction, color, intensity);
lights.AddPointLight(position, color, intensity, radius);
```

### Add/tune a material

```cpp
Material& mat = materials.Get(id);   // id = the voxel byte value that should use it
mat.baseColor = glm::vec3(...);
mat.nodalColor = glm::vec3(...);     // color along the pattern's nodal lines
mat.scale = 1.0f;                    // spatial frequency of the pattern
mat.chladni.holdDuration = 5.0f;     // seconds a figure holds before crossfading
mat.chladni.transitionDuration = 2.5f; // seconds the crossfade itself takes
```

### Change the world layout

`Chunk` currently has `GenerateFlatTerrain`, `GenerateHollowRoom`, and
`GeneratePillarField`. Add a new generator method the same way — it just
needs to fill `voxels[Index(x,y,z)]` with a material ID per cell.

## Known gaps (by design, not bugs)

- No gravity/collision — flight-only movement for now.
- `BuildTool` always places material `1` — there's no material picker
  (hotbar) yet.
- Materials are hardcoded in `Application.cpp` (`SetupDemoMaterials`) —
  no in-game editor or save/load presets yet.
