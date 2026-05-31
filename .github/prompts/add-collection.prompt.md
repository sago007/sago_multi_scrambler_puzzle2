---
description: "Scaffold a new puzzle collection: creates collection.json and README.md under data/collections/<name>/"
argument-hint: "Collection name (e.g. animals, landscapes)"
agent: "agent"
---

Scaffold a new puzzle collection for this game. The collection name is: **$ARGUMENTS**

## What to do

1. Derive the folder name: lowercase, underscores for spaces (e.g. "Fairy Tales" → `fairy_tales`).

2. Create `data/collections/<folder_name>/collection.json` using this exact schema — do not add extra fields:
```json
{
    "name": "<Human-readable collection name>",
    "description": "<One-sentence description of the collection>",
    "puzzles": [
        {
            "image": "<filename.jpg>",
            "title": "<Puzzle title>",
            "description": "<One-sentence description of this image>",
            "flip_mode": false,
            "rectangular_mode": false
        }
    ]
}
```
- `image` must be a filename only (no path) — images live in the same folder as `collection.json`
- `flip_mode: true` enables piece-flipping gameplay; use `false` unless the user asks for it
- `rectangular_mode: true` forces rectangular (non-rotated) pieces; use `false` unless asked

3. Create `data/collections/<folder_name>/README.md` listing each image filename, its source URL (if known), and the image author/copyright. Follow the format used in [data/collections/fairy_tales/README.md](../../data/collections/fairy_tales/README.md).

4. **Do not** copy any image files — only create the metadata files. Remind the user to place image files in `data/collections/<folder_name>/` before running the game.

5. After creating the files, print a short summary: folder path, number of puzzle entries scaffolded, and a reminder about adding the actual image files.

## Notes
- Collections are auto-discovered at runtime — no code changes needed.
- See [.github/copilot-instructions.md](../copilot-instructions.md) → "Adding New Collections" for full context.
- If the user hasn't provided image filenames, create placeholder entries with `"image": "TODO.jpg"` and note they need to be filled in.
