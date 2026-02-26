# HangoutWorld (UE5) – Multiplayer + Phase 2 Customizable Room (Test-Ready Setup)

## Prerequisites
- **Unreal Engine**: 5.7.3 (validated target).
- **Unreal Engine C++ toolchain support** installed via Epic Launcher for your platform.
- **Starter Content** (recommended) for catalog meshes. If unavailable, use Engine basic shapes.

## Repo Structure
- `Content/HangoutWorld/`
  - `Maps/` → `MainMenu`, `HangoutLobby`, `PlayerRoom`
  - `UI/` → UMG widgets for menu/lobby/room editor
  - `Rooms/` → data assets / room manager blueprints / placeable BP classes
- `Source/HangoutWorld/`
  - Session + travel: `HW_GameInstance`
  - Multiplayer gameplay: `HW_GameMode`, `HW_PlayerController`, `HW_PlayerState`, `HW_GameState`, `HW_Character`
  - Room system: `HW_RoomManager`, `HW_PlaceableFurnitureActor`, `HW_FurnitureCatalogDataAsset`, `HW_RoomJsonUtils`

## Required Maps (must exist in Content Browser)
- `Content/HangoutWorld/Maps/MainMenu.umap`
- `Content/HangoutWorld/Maps/HangoutLobby.umap`
- `Content/HangoutWorld/Maps/PlayerRoom.umap`

> Project defaults are configured to open **MainMenu** first.

## Build and Run (exact steps)
1. Open `HangoutWorld.uproject` directly in **Unreal Engine**.
2. If prompted, allow Unreal to generate project files and compile missing C++ modules.
3. In Unreal Editor, use **Platforms > Windows > Build Target: Development Editor** (or your platform equivalent).
4. Use **Compile** in the Unreal Editor toolbar to ensure the project builds successfully.
5. Confirm startup map is `MainMenu`.

## Multiplayer PIE Test (2 players)
1. In Unreal Editor, set **Play > Number of Players = 2**.
2. Ensure **Net Mode = Play As Listen Server**.
3. Press Play on `MainMenu`.
4. Validate session flow:
   - Player 1 clicks **Host Lobby** (travels to `HangoutLobby` listen server).
   - Player 2 clicks **Find Sessions (LAN)** and joins.
5. Validate lobby features:
   - Replicated movement
   - Replicated display names
   - Replicated global text chat
   - Replicated emote placeholders (Wave/Point)

## Room Feature Test (Phase 2)
1. In Lobby, host uses **Go To My Room**.
2. Room opens in `PlayerRoom` listen server mode with `OwnerId` travel option.
3. Owner toggles **Edit Mode** and places/removes furniture.
4. Visitor joins and confirms join-in-progress sees all spawned furniture actors.
5. Click **Save** in room UI (or rely on autosave-on-place/delete).
6. Verify persistence file:
   - `Saved/HangoutWorld/Rooms/<OwnerUniqueId>.json`
7. Re-enter room and confirm layout loads.

## Session and Travel Configuration
- Online subsystem: `NULL` (LAN sessions)
- Host path:
  - Main menu → `HostLobby` → opens `HangoutLobby?listen`
- Join path:
  - Main menu → `FindLobbySessions` / `JoinLobbySession`
- Room path:
  - Lobby → `HostMyRoom` → opens `PlayerRoom?OwnerId=<id>?listen`

## Guardrails and Stability
- Defensive null checks in session and room flows.
- Log instrumentation for host/join/travel/load/save failures.
- JSON loader handles:
  - Missing file → empty layout
  - Invalid JSON → warning + empty/fallback behavior
  - Malformed item entries → skip invalid records
- Server-authoritative placement/deletion only.

## Known Issues
- UMG widgets and map assets must be created/wired in editor (not all binary assets are tracked in this repo snapshot).
- If player unique IDs are unavailable in local PIE, owner checks may fall back to player name and should be kept consistent.
- Starter Content mesh references depend on project content availability.

## TODO
- Add automated functional tests for session flow and room persistence.
- Add debounce timer for autosave (currently save-per-mutation/manual save).
- Improve room visit UX with explicit “Visit Host Room” list filtering.
