---
description: "Scaffold a new game state: creates the .hpp and .cpp boilerplate for a sago::GameStateInterface subclass in src/"
argument-hint: "State class name (e.g. HighScoreState, OptionsState)"
agent: "agent"
---

Scaffold a new game state for this project. The class name is: **$ARGUMENTS**

## What to do

1. Derive the filename: class name as-is (e.g. `HighScoreState` → `HighScoreState.hpp` / `HighScoreState.cpp`). Files go in `src/`.

2. Create `src/<ClassName>.hpp` using this exact pattern (based on existing states like [src/CollectionListState.hpp](../src/CollectionListState.hpp)):

```cpp
/*
===========================================================================
 * Sago Multi Scrambler Puzzle
Copyright (C) 2022-2026 Poul Sander
...license header...
===========================================================================
*/

#pragma once

#include "sago/GameStateInterface.hpp"

class <ClassName> : public sago::GameStateInterface {
public:
	<ClassName>();
	<ClassName>(const <ClassName>& orig) = delete;
	virtual ~<ClassName>();

	bool IsActive() override;
	void ProcessInput(const SDL_Event& event, bool& processed) override;
	void Draw(SDL_Renderer* target) override;
	void Update() override;

private:
	bool isActive = true;
};
```

3. Create `src/<ClassName>.cpp` using this exact pattern (based on [src/CollectionListState.cpp](../src/CollectionListState.cpp)):

```cpp
/*
===========================================================================
 * Sago Multi Scrambler Puzzle
Copyright (C) 2022-2026 Poul Sander
...license header...
===========================================================================
*/

#include "<ClassName>.hpp"
#include "sago_common.hpp"
#include "globals.hpp"
#include "SagoImGui.hpp"

<ClassName>::<ClassName>() {
}

<ClassName>::~<ClassName>() {
}

bool <ClassName>::IsActive() {
	return isActive;
}

void <ClassName>::ProcessInput(const SDL_Event& event, bool& processed) {
	ImGui_ImplSDL2_ProcessEvent(&event);
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_ESCAPE) {
			isActive = false;
			processed = true;
		}
	}
}

void <ClassName>::Draw(SDL_Renderer* target) {
	// TODO: implement drawing
}

void <ClassName>::Update() {
	// TODO: implement per-frame logic
}
```

4. **Do not** wire the state into `src/sago_multi_scrambler_puzzle2.cpp` unless the user explicitly asks — just create the two files.

5. After creating the files, print:
   - The two file paths created
   - A snippet showing how to launch the state from `main()`:
     ```cpp
     #include "<ClassName>.hpp"
     // ...
     InitGame();
     <ClassName> state;
     RunGameState(state);
     UninitGame();
     ```

## Key rules
- Always `#include "SagoImGui.hpp"` in the .cpp so `ImGui_ImplSDL2_ProcessEvent` is available — forgetting this is the most common compile error.
- Use tabs for indentation (matches the existing codebase style).
- `isActive = false` is the only way to exit a state (pops from the game loop in `RunGameState()`).
- Access renderer output size via `globalData.xsize` / `globalData.ysize` (not SDL_GetWindowSize).
- See [.github/copilot-instructions.md](../copilot-instructions.md) → "Adding a New Game State" for full context.
