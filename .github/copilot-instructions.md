# Sago Multi Scrambler Puzzle II - AI Coding Agent Instructions

## Project Overview
C++ puzzle game using SDL2, featuring image scrambling puzzles with splitting, shuffling, and flipping mechanics. Uses a state-based architecture with PhysFS for virtual filesystem abstraction and Dear ImGui for UI.

## Architecture

### State Machine Pattern
All game screens inherit from `sago::GameStateInterface` (located in `src/sago/GameStateInterface.hpp`):
- `IsActive()` - Returns false to pop state from stack
- `Draw(SDL_Renderer*)` - Render to screen
- `ProcessInput(const SDL_Event&, bool& processed)` - Handle events
- `Update()` - Per-frame logic

**Key States:**
- `MainGameState` - Main menu (minimal implementation)
- `CollectionListState` - Lists all available collections
- `CollectionPlayState` - Plays puzzles sequentially from a collection, tracks progress
- `ImageSelectState` - Browse and select images from a folder
- `PuzzleSingleImageState` - Core puzzle gameplay (shuffle, flip, solve detection)
- `PuzzlePieceEditorState` - Custom piece layout editor (WIP)
- `SagoTextureSelector` - Texture selector for editor mode (WIP)

### Global State & Resource Management
`GlobalData` struct (in `src/globals.hpp`) is the singleton holding:
- `SDL_Renderer* screen` - Rendering target
- `sago::SagoSpriteHolder* spriteHolder` - Sprite cache
- `sago::SagoDataHolder* dataHolder` - Texture/resource cache
- Window dimensions (`xsize`/`ysize`), mouse state, flags

Access via global `globalData` instance. Resources auto-reload when renderer recreates.

### Sprite System
Sprites defined in JSON files at `data/sprites/*.sprite`:
```json
{
  "sprite_name": {
    "texture": "texture_name",
    "topx": 0, "topy": 0,
    "height": 64, "width": 64,
    "number_of_frames": 1,
    "frame_time": 100
  }
}
```
Load via `globalData.spriteHolder->GetSprite("sprite_name")`, draw with `.Draw(renderer, SDL_GetTicks(), x, y)`.

**UI Convention:** Nine-patch rectangles use `ui_rect_white_*` and `ui_rect_yellow_*` sprites (n, s, e, w, ne, nw, se, sw, fill variants). See `DrawRectYellow()`/`DrawRectWhite()` helpers in `MainGameState.cpp`.

## Build System & Development

### Building
```bash
cmake .        # Configure (requires SDL2, SDL2_image, SDL2_ttf, SDL2_mixer, SDL2_gfx, Boost, physfs, rhash)
make           # Compile
```
**No test suite** — zero test infrastructure (no CTest, GTest, etc.).

GitHub Actions CI: Builds in Docker ([extra/docker/Dockerfile](../extra/docker/Dockerfile)) on every push.

### Running
```bash
./sago_multi_scrambler_puzzle2                               # Main menu
./sago_multi_scrambler_puzzle2 image.jpg                     # Direct puzzle
./sago_multi_scrambler_puzzle2 --collection fairy_tales      # Collection
./sago_multi_scrambler_puzzle2 --folder /path/to/images      # Custom folder
./sago_multi_scrambler_puzzle2 --editor                      # Editor (WIP)
./sago_multi_scrambler_puzzle2 --install-desktop-entry       # Linux: install .desktop + icon
```

### File Structure
- `src/` - Main source (game states, utilities)
- `src/sago/` - Reusable library code (sprites, data holders, platform folders)
- `src/Libs/` - Embedded third-party (Dear ImGui, rapidjson)
- `src/editor/` - Editor-specific components
- `data/` - Game assets (sprites, textures, fonts, collections)
- `embedded_libs/PlatformFolders-4.2.0/` - Cross-platform paths library

## Key Patterns & Conventions

### Game Loop
Standard flow: `InitGame()` → `RunGameState(state)` → `UninitGame()`

`RunGameState()` (in `src/sago_common.cpp`) handles:
1. Clear/draw background
2. ImGui frame setup
3. Call state's `Draw()` and `Update()`
4. Process SDL events via state's `ProcessInput()`
5. Render ImGui and present

### PhysFS Virtual Filesystem
All assets loaded through PhysFS (not direct file I/O):
- **Call order matters:** `InitSagoFS()` must run **before** `InitGame()` — the window icon loads from PhysFS in `InitGame()`
- Mount point: `data/` directory at startup (`InitSagoFS()` in `src/sago_common.cpp`)
- Use `sago::GetFileContent(path)` for reading files (note: in `sago::` namespace)
- Paths relative to mount: `"sprites/background.sprite"`, not `"data/sprites/..."`
- Write directory: save folder (lowest priority in mount search order)

### Save Files
Platform-specific paths via `sago::platform_folders`:
- Linux: `~/.local/share/sago_multi_scrambler_puzzle2/`
- Windows: `Documents/My Games/sago_multi_scrambler_puzzle2/`
- Function: `getPathToSaveFiles()` in `os.cpp`
- ImGui settings: `imgui.ini` in save folder

### Dear ImGui Integration
- Initialized in `InitGame()` with docking enabled
- Settings manually loaded/saved (not auto-saved)
- Process events via `ImGui_ImplSDL2_ProcessEvent(&event)` in each state's `ProcessInput()`
- New frame setup in `RunGameState()` loop

## Common Tasks

### Adding a New Game State
1. Create header/cpp in `src/`, inherit from `sago::GameStateInterface`
2. Implement all virtual methods (IsActive, Draw, ProcessInput, Update)
3. Set `isActive = false` to exit state (pops from stack)
4. Call `RunGameState(yourState)` after `InitGame()`

### Adding UI Elements
Use nine-patch helpers: `DrawRectYellow(renderer, x, y, height, width)` or `DrawRectWhite()`. These are duplicated across states - consider refactoring to shared utility if modifying.

### Working with Images
`PuzzleSingleImageState` loads images via `SDL_image`, splits them into logical pieces (stored in `pieces_logical`), then creates physical pieces (`pieces_physical`) scaled to screen. Key methods:
- `LoadPictureFromFile()` - Load and create initial piece
- `SplitPiece()` / `SplitPieceVertical()` / `SplitPieceHorisontal()` - Recursively divide pieces
- `Shuffle()` - Randomize piece positions and rotations
- `ResizeImagePhysical()` - Recalculate on window resize

### Adding New Collections
Place images in `data/collections/<collection_name>/`. Collections need a `collection.json` with a `puzzles` array (fields: `image`, `title`, `description`, `flip_mode`, `rectangular_mode`). Add a `README.md` for documentation. Collections are auto-discovered at runtime.

### Application Icon
- **Window icon**: `data/textures/app_icon.png` — 128×128 PNG loaded via PhysFS in `InitGame()`
- **Desktop icon**: `extra/sago-multi-scrambler-puzzle2.svg` — SVG installed to `~/.local/share/icons/hicolor/scalable/apps/` by `--install-desktop-entry`
- Rasterize SVG: `inkscape --export-type=png --export-width=128 --export-height=128 --export-filename=data/textures/app_icon.png extra/sago-multi-scrambler-puzzle2.svg`

## Gotchas & Technical Notes

- **PhysFS init order:** `InitSagoFS()` must be called before `InitGame()` — icon loading at window creation depends on it
- **`sago::` namespace:** `GetFileContent()`, `FileExists()`, `WriteFileContent()` are all in the `sago::` namespace (from `sago/SagoMisc.hpp`)
- **Duplicated Code:** `DrawRect()`/`DrawRectYellow()`/`DrawRectWhite()` exist in both `MainGameState.cpp` and `ImageSelectState.cpp` as static functions
- **Header Links:** Some headers reference `https://github.com/sago007/saland` (old project) instead of correct `sago_multi_scrambler_puzzle2`
- **Version Macros:** `GAMENAME` defined in two places: `globals.hpp` (with display name) and `version.h` (lowercase for save paths)
- **Mouse Cursor:** Native cursor disabled, custom sprite drawn via `globalData.mouse`
- **Resource Invalidation:** When window resizes, must call `dataHolder.invalidateAll()` to force texture reloads
- **ImGui Backends:** Uses `imgui_impl_sdl2.cpp` and `imgui_impl_sdlrenderer2.cpp` from ImGui's backends

## Dependencies
External libraries required by CMake:
- **SDL2** (core, image, ttf, mixer, gfx) - Graphics/audio/input
- **Boost** (program_options) - Command-line parsing
- **PhysFS** - Virtual filesystem
- **rhash** - Hashing library (used for puzzle state tracking)
- **platform_folders** - Cross-platform paths (embedded in `embedded_libs/`)

All other libraries (ImGui, rapidjson) vendored in `src/Libs/`.
