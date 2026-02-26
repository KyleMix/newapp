# HangoutWorld (UE5 C++ Networking Prototype)

This refactor keeps listen-server play in editor working while aligning architecture with dedicated-server expectations.

## A) Short plan
1. Keep server authority for gameplay data and RPC entry points (chat/emote).
2. Keep UI creation and interaction client-only (`PlayerController` + UMG).
3. Add Unreal session flow on OnlineSubsystem NULL (LAN host/find/join).
4. Add dedicated-server target/config and clear run commands.
5. Harden chat on server (sanitize + max length + per-player rate limit).

## B) Files/classes created or modified

### New
- `Source/HangoutWorldServer.Target.cs`

### Modified
- `Source/HangoutWorld/Public/HW_GameInstance.h`
- `Source/HangoutWorld/Private/HW_GameInstance.cpp`
- `Source/HangoutWorld/Public/HW_PlayerController.h`
- `Source/HangoutWorld/Private/HW_PlayerController.cpp`
- `Source/HangoutWorld/Public/HW_PlayerState.h`
- `Source/HangoutWorld/Private/HW_PlayerState.cpp`
- `Source/HangoutWorld/Public/HW_Character.h`
- `Source/HangoutWorld/Private/HW_Character.cpp`
- `Source/HangoutWorld/HangoutWorld.Build.cs`
- `Config/DefaultEngine.ini`
- `HangoutWorld.uproject`

## C) C++ code changes
See code files in `Source/` listed above. Key behavior updates:
- `UHW_GameInstance` now manages LAN sessions (Host/Find/Join) through Unreal's session interface and exposes BP delegates for UI.
- `AHW_PlayerController` keeps chat server-authoritative and sanitizes messages both client-side and server-side.
- `AHW_PlayerState` tracks recent chat send times for basic server-side rate limiting.
- `AHW_Character` safely binds/unbinds `PlayerState` delegates to avoid duplicate bindings and join-in-progress edge issues.
- `HangoutWorldServer.Target.cs` enables server builds.

## D) UMG Blueprint steps for session browser UI

Create a new widget `WBP_HW_SessionBrowser` (or extend your existing lobby widget):

1. **Layout**
   - VerticalBox root:
     - HorizontalBox (top row):
       - Button `HostSessionButton`
       - Button `RefreshSessionsButton`
     - ScrollBox or ListView `SessionList`
     - TextBlock `SessionStatusText`
   - Keep existing chat controls/buttons in same lobby widget.

2. **Widget variables**
   - `CachedSessionResults` (Array of `FHWSessionSearchResult`)
   - `OwningGI` (`UHW_GameInstance` reference)

3. **On Construct**
   - `Get Game Instance` -> cast to `HW_GameInstance` -> set `OwningGI`.
   - Bind to:
     - `OwningGI.OnSessionSearchCompleted`
     - `OwningGI.OnHostCompleted`
     - `OwningGI.OnJoinCompleted`

4. **Host flow**
   - `HostSessionButton.OnClicked`:
     - Set status text to “Hosting session…”
     - Call `OwningGI.HostLobby(8, "/Game/HangoutWorld/Maps/HangoutLobby")`

5. **Find flow**
   - `RefreshSessionsButton.OnClicked`:
     - Set status text to “Searching LAN sessions…”
     - Call `OwningGI.FindLobbySessions(20)`
   - On `OnSessionSearchCompleted`:
     - Save array to `CachedSessionResults`
     - Clear `SessionList`
     - For each result, spawn row widget `WBP_HW_SessionRow` with:
       - HostName
       - `CurrentPlayers/MaxPlayers`
       - Join button that stores its index

6. **Join flow**
   - In row widget `JoinButton.OnClicked`:
     - Call parent widget event `RequestJoinSession(Index)`
   - In parent:
     - Set status text “Joining session…”
     - `OwningGI.JoinLobbySession(Index)`

7. **Result handling**
   - `OnHostCompleted(bool)`:
     - True: status “Session hosted. Traveling…”
     - False: status “Failed to host session.”
   - `OnJoinCompleted(bool)`:
     - True: status “Join success. Traveling…”
     - False: status “Failed to join session.”

8. **Client-only UI rule**
   - Keep all widget spawning and visual updates in PlayerController/UI only.
   - Do not put widget logic in `GameMode` or dedicated-server paths.

## E) Run / test instructions

## Listen server (Editor)
1. Open project in UE5.
2. Ensure map is `HangoutLobby` and game mode uses `HW_GameMode` (or BP subclass).
3. PIE settings:
   - Number of Players: `2`
   - Net Mode: `Play As Listen Server`
4. In host window:
   - Open session browser and click **Host Session**.
5. In client window:
   - Click **Find Sessions** then **Join** on host entry.
6. Validate:
   - Display names replicate
   - Chat replicates and is server-filtered
   - Emotes replicate

## Dedicated server packaging readiness

### Build target
- Server target file exists: `Source/HangoutWorldServer.Target.cs`

### Required config
- `OnlineSubsystem NULL` is enabled in `DefaultEngine.ini`.
- `OnlineSubsystemNull` plugin is enabled in `.uproject`.

### Typical build commands (example)
From your Unreal Engine source/binary toolchain environment:
- `UnrealBuildTool HangoutWorldServer Win64 Development -Project="<path>/HangoutWorld.uproject"`
- Package client normally via Editor/AutomationTool.

### Local run examples (once packaged)
- Dedicated server:
  - `HangoutWorldServer.exe /Game/HangoutWorld/Maps/HangoutLobby -log -port=7777`
- Client 1:
  - `HangoutWorld.exe -log -ResX=1280 -ResY=720`
- Client 2:
  - `HangoutWorld.exe -log -ResX=1280 -ResY=720 -WinX=1400`

For direct dedicated connect testing:
- In client console: `open 127.0.0.1:7777`

## Hardening notes
- Server chat validation:
  - Control chars stripped (newline/tab converted to spaces).
  - Final message trimmed and clamped to 256 chars.
- Server chat rate limit:
  - Default 5 messages / 5 seconds / player.
- Join-in-progress safety:
  - Character now unbinds/rebinds display-name delegates cleanly on player-state changes.
