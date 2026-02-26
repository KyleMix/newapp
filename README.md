# HangoutWorld – Phase 2: Customizable Rooms

## A) Plan + Architecture choice

### Chosen approach: **Option A (separate Room map travel)**
I implemented room housing with standard map travel/session flow instead of per-owner streamed sublevels.

**Why this is simplest and most stable right now:**
1. Reuses your already-working host/join session pipeline.
2. Keeps one authoritative room state on one server world.
3. Avoids complex per-player streaming ownership/routing logic for this phase.
4. Dedicated-server compatible (room map can be loaded by listen or dedicated host).

**Flow**
- Owner presses **Go To My Room** → `HostMyRoom()` hosts a session and travels to room map with `?OwnerId=<UniqueId>` URL option.
- Visitor presses **Visit Friend Room** from session list → `VisitFriendRoom()` (join by session index).
- `AHW_RoomManager` on server reads owner layout JSON, spawns replicated furniture actors, and maintains authoritative layout mutations.

---

## B) New/modified classes and assets

## New C++ classes
- `Source/HangoutWorld/Public/HW_FurnitureCatalogDataAsset.h`
- `Source/HangoutWorld/Private/HW_FurnitureCatalogDataAsset.cpp`
- `Source/HangoutWorld/Public/HW_PlaceableFurnitureActor.h`
- `Source/HangoutWorld/Private/HW_PlaceableFurnitureActor.cpp`
- `Source/HangoutWorld/Public/HW_RoomJsonUtils.h`
- `Source/HangoutWorld/Private/HW_RoomJsonUtils.cpp`
- `Source/HangoutWorld/Public/HW_RoomManager.h`
- `Source/HangoutWorld/Private/HW_RoomManager.cpp`

## Modified C++ classes
- `Source/HangoutWorld/Public/HW_Types.h`
- `Source/HangoutWorld/Public/HW_GameInstance.h`
- `Source/HangoutWorld/Private/HW_GameInstance.cpp`
- `Source/HangoutWorld/Public/HW_PlayerController.h`
- `Source/HangoutWorld/Private/HW_PlayerController.cpp`
- `Source/HangoutWorld/HangoutWorld.Build.cs`

## Unreal assets to create in Editor
1. **Catalog DataAsset**
   - Path: `/Game/HangoutWorld/Rooms/Data/DA_HW_FurnitureCatalog`
   - Class: `UHW_FurnitureCatalogDataAsset`
   - Add 5–10 entries (example IDs):
     - `chair_basic`
     - `couch_small`
     - `table_round`
     - `lamp_floor`
     - `plant_potted`
     - `poster_modern`
     - `rug_square`
2. **Room manager actor BP**
   - Path: `/Game/HangoutWorld/Rooms/BP_HW_RoomManager`
   - Parent class: `AHW_RoomManager`
   - Set `FurnitureCatalog = DA_HW_FurnitureCatalog`
   - Place one instance in room map.
3. **Furniture actor BPs** (optional per item)
   - Parent class: `AHW_PlaceableFurnitureActor`
   - Set static mesh/materials per furniture item.

---

## C) C++ implementation details

### 1) Catalog data structures
- `FHWCatalogItemDefinition` and `FHWPlacedFurnitureRecord` are in `HW_Types.h`.
- Catalog is stored in `UHW_FurnitureCatalogDataAsset` with item lookup by `CatalogId`.

### 2) Placeable replicated actor
- `AHW_PlaceableFurnitureActor` replicates:
  - `PlacedItemId`
  - `CatalogItemId`
  - `RoomOwnerUniqueId`
- Actor replication + server spawn ensures join-in-progress clients receive existing placed furniture.

### 3) Room manager (server authority + replication)
- `AHW_RoomManager` owns room layout authoritative state:
  - Replicated array: `TArray<FHWPlacedFurnitureRecord> RoomLayout` (`OnRep_Layout`)
  - Owner identity: `RoomOwnerUniqueId`
  - Runtime map to avoid duplicates: `SpawnedActors`
- Core behavior:
  - On server `BeginPlay()` reads `OwnerId` from URL and loads JSON.
  - `AddFurnitureFromRequest(...)` validates owner permissions, validates catalog ID, assigns `FGuid`, updates layout, spawns replicated actor, saves JSON.
  - `RemoveFurnitureFromRequest(...)` validates owner permissions, removes layout/actor, saves JSON.
  - `OnRep_Layout()` + `ReconcileSpawnedActorsWithLayout()` handles late join and avoids duplicates.

### 4) JSON save/load utilities
- `FHW_RoomJsonUtils` writes/reads:
  - `Saved/HangoutWorld/Rooms/<OwnerUniqueId>.json`
- Includes:
  - `itemId`
  - `catalogId`
  - full transform (`location`, `rotation`, `scale` components)
- Error behavior:
  - Missing file: valid empty layout.
  - Invalid JSON: warning log + empty layout fallback.
  - Unknown catalog ID: skipped with warning.

### 5) Player-controller placement API (owner edit mode)
`AHW_PlayerController` now exposes Blueprint-callable room editing hooks:
- `SetRoomEditModeEnabled(bool)` (owner check via room manager)
- `SetRoomGridSnapEnabled(bool)`
- `SetSelectedFurnitureCatalogId(FName)`
- `ConfirmFurniturePlacement(FTransform)` → server RPC `ServerConfirmFurniturePlacement(...)`
- `DeleteFurnitureItem(AHW_PlaceableFurnitureActor*)` → server RPC `ServerDeleteFurnitureItem(...)`

All placement/deletion is validated and executed server-side only.

### 6) Room travel UI hooks
`UHW_GameInstance` now includes:
- `HostMyRoom(int32 MaxPublicConnections)`
- `VisitFriendRoom(int32 SessionIndex)`
- `RoomMapPath` config value (`/Game/HangoutWorld/Maps/HangoutRoom` by default)

---

## D) UMG Blueprint steps

Create/extend your lobby widget with:
- Button: **Go To My Room**
- Button: **Visit Friend Room** (from selected session row)

### Lobby widget bindings
1. On construct, cache `HW_GameInstance`.
2. **Go To My Room** button:
   - Call `HW_GameInstance.HostMyRoom(8)`
3. **Visit Friend Room** button/row action:
   - Call `HW_GameInstance.VisitFriendRoom(SessionIndex)`

### Room HUD widget (`WBP_HW_RoomEditHUD`)
Add:
- Toggle button: **Edit Mode**
- Toggle: **Grid Snap**
- ListView/VerticalBox for catalog entries
- Buttons: **Place**, **Cancel**, **Delete Selected**

Binding logic:
1. `Edit Mode Toggle` → `PlayerController.SetRoomEditModeEnabled`.
2. `Grid Snap Toggle` → `PlayerController.SetRoomGridSnapEnabled`.
3. Catalog row click → `PlayerController.SetSelectedFurnitureCatalogId(CatalogId)`.
4. In edit mode, spawn/update a local ghost preview BP (non-replicated visual only).
5. Place confirm:
   - Build transform from preview (apply snap if enabled).
   - Call `PlayerController.ConfirmFurniturePlacement(Transform)`.
6. Delete selected:
   - Raycast/select a furniture actor.
   - Call `PlayerController.DeleteFurnitureItem(TargetFurnitureActor)`.

Permission UX:
- If not owner, hide/disable edit controls, but keep interaction controls (e.g., sit/interact) enabled.

---

## E) Exact local test instructions

## Test 1: Owner save/load persistence
1. Launch 1 listen server client.
2. In lobby, click **Go To My Room**.
3. Enable edit mode.
4. Place 3 items from catalog.
5. Exit to lobby/menu, then re-open **Go To My Room**.
6. Verify all 3 items load in same transforms.
7. Confirm file exists at:
   - `Saved/HangoutWorld/Rooms/<OwnerUniqueId>.json`

## Test 2: Visitor replication and permissions
1. Launch 2 clients (listen host + client).
2. Host goes to room and places items.
3. Client uses **Visit Friend Room** (session join).
4. Verify visitor sees exact placed items.
5. On visitor, attempt edit mode/place/delete:
   - Must be blocked (no mutation).

## Test 3: Join-in-progress
1. Host room with placed items already loaded.
2. Start a new client after host is already in room.
3. Join room session.
4. Verify placed furniture appears automatically without manual refresh.

## Test 4: Robustness checks
1. Delete owner JSON file and open room:
   - Room should start empty.
2. Corrupt JSON content manually and open room:
   - Warning log expected, room should start empty.
3. Put unknown `catalogId` in JSON item:
   - Unknown item skipped, others still load.

---

## TODO (future phases only)
- Backend persistence service (replace local JSON)
- Furniture inventory/economy
- Monetization and entitlement checks
- Advanced moderation for object interactions
