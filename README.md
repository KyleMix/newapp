# HangoutWorld (UE5 C++ Starter Prototype)

## Scope
This repository provides a starter Unreal Engine 5 C++ project skeleton for an MMO-style social hangout app foundation.

## Phase 1 Plan (Implemented)
1. Create UE5 C++ project shell (`HangoutWorld`) and folder conventions.
2. Add core multiplayer gameplay classes (GameMode, GameState, PlayerState, PlayerController, Character, GameInstance).
3. Implement Host/Join flow for listen server setup.
4. Implement replicated display names via `PlayerState`.
5. Implement replicated global text chat using server-authoritative state in `GameState`.
6. Implement two replicated placeholder emotes (`Wave`, `Point`).
7. Provide Blueprint/UI wiring steps for lobby widget and map setup.
8. Document local multiplayer test flow.

## Project Structure

### Content folders
Create these in the Unreal Editor Content Browser if not already present:

- `Content/HangoutWorld/Maps/`
- `Content/HangoutWorld/Blueprints/`
- `Content/HangoutWorld/UI/`
- `Content/HangoutWorld/Characters/`
- `Content/HangoutWorld/Systems/`
- `Content/HangoutWorld/Data/`
- `Content/HangoutWorld/Audio/`

### C++ classes
- `AHW_GameMode`: base game mode for lobby gameplay.
- `AHW_GameState`: replicated global chat buffer.
- `AHW_PlayerState`: replicated display name.
- `AHW_PlayerController`: chat send/focus/emote trigger + local UI spawn.
- `AHW_Character`: third-person pawn with replicated emote playback and overhead name text.
- `UHW_GameInstance`: local player name storage + host/join travel helpers.

## Unreal Editor Setup Steps (Blueprint + Map)

1. **Create project**
   - UE5 -> Games -> Third Person -> C++ -> Project name `HangoutWorld`.
   - Copy this repo's `Source/` and `Config/` over that project (or create classes from editor and paste code).

2. **Create lobby map**
   - Create map: `Content/HangoutWorld/Maps/HangoutLobby.umap`.
   - Add floor, light, PlayerStart(s), and nav if needed.

3. **Create character BP** (`BP_HW_Character`)
   - Parent: `AHW_Character`.
   - Set mesh/anim blueprint from Third Person template mannequin.
   - (Optional) Assign placeholder montages to `WaveMontage` and `PointMontage`.

4. **Create game mode BP** (`BP_HW_GameMode`)
   - Parent: `AHW_GameMode`.
   - Set `Default Pawn Class = BP_HW_Character`.

5. **Create lobby widget** (`WBP_HangoutLobby`)
   - Layout:
     - Vertical Box with:
       - ScrollBox (`ChatScroll`)
       - EditableTextBox (`ChatInput`)
       - Horizontal row of buttons: Host, Join, Wave, Point.
       - EditableTextBox for Join IP (`JoinAddressInput`).
   - Add a lightweight row widget (`WBP_ChatLine`) with one TextBlock (`ChatLineText`).

6. **Widget logic**
   - On Construct:
     - Get Owning Player -> cast `AHW_PlayerController`.
     - Get GameState -> cast `AHW_GameState`.
     - Bind to `OnChatMessagesUpdated` delegate.
   - On `ChatInput.OnTextCommitted`:
     - If commit type is Enter: call `SendChatMessage` on player controller.
     - Clear input.
   - On Escape key:
     - Call `SetChatFocus(false)`.
   - Chat refresh function:
     - Clear `ChatScroll` children.
     - For each `FHWChatMessage`, spawn `WBP_ChatLine`, set text as `[Timestamp] Sender: Message`, add to scroll.

7. **Buttons**
   - Host button -> Get GameInstance (`UHW_GameInstance`) -> `HostLobby`.
   - Join button -> `JoinLobby(JoinAddressInput.Text)`.
   - Wave button -> `TriggerEmote(EHWEmoteType::Wave)`.
   - Point button -> `TriggerEmote(EHWEmoteType::Point)`.

8. **Controller widget class**
   - In `AHW_PlayerController` defaults (or BP subclass), set `LobbyWidgetClass = WBP_HangoutLobby`.

9. **Map world settings**
   - Set GameMode override to `BP_HW_GameMode`.

## Local Multiplayer Test Steps

1. In editor, set **Play Number of Players = 2**.
2. In Advanced Play settings, set Net Mode to **Play as Listen Server**.
3. Click Play:
   - Window 1 acts as host/server.
   - Window 2 acts as client.
4. Verify:
   - Movement replicates naturally from `ACharacter` networking.
   - Overhead names visible/updated for both players.
   - Chat message from either client appears for both.
   - Emote buttons trigger replicated placeholder actions on both clients.

For separate processes/package test:
- Run host build and execute `open /Game/HangoutWorld/Maps/HangoutLobby?listen`.
- Run client build and execute `open 127.0.0.1`.

## Phase 2 TODO (Customizable Rooms + Persistence)
- Add room metadata data model (`RoomId`, title, theme, owner, privacy).
- Introduce backend service for account auth and room persistence.
- Replace direct IP join with room directory + matchmaking/session browser.
- Save player profile customization (name, avatar cosmetics, emote loadout).
- Persist chat history per room (bounded retention + moderation flags).

## Phase 3 TODO (Activities)
- Activity framework with replicated state machines (e.g., mini-games, whiteboard).
- Activity join/leave + spectator flows.
- Score/progress replication with late join synchronization.
- Activity plugin interface so features can be toggled per room.

## Phase 4 TODO (Screen Share / Presenting)
- Integrate voice chat provider (OSS voice, Vivox, or platform-native).
- Add presenter role + permission system.
- Add WebRTC/screen-share pipeline service (SFU architecture).
- Add QoS/adaptive bitrate and abuse safety controls.

## Notes
- This starter keeps networking server-authoritative where relevant and avoids dedicated-server-only assumptions.
- Code is intentionally minimal and readable for iteration.
