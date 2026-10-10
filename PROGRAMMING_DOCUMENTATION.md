# RPG - Programming Documentation

---

## Table of Contents

1. [Architecture Overview](#architecture-overview)
2. [State Machine Pattern](#state-machine-pattern)
3. [Ownership Model](#ownership)
4. [Class Reference](#class-reference)
5. [Systems Documentation](#systems-documentation)

---

## Architecture Overview

The project is structured as a set of controllers, each owning the data and state for one phase of the game. All navigation between phases flows through `GameController`.

```
main()
  -> GameController
    -> CharacterCreation      (new game flow)
    -> Town                   (rest, save, choose next action)
    -> DungeonController      (exploration)
      -> Dungeon              (map generation and storage)
      -> CombatController     (turn-based fighting)
        -> AbilityMenu
      -> Inventory

Supporting systems (no UI of their own):
  PlayerCharacter  <-- owned by Player, owned by GameController
  ItemManager      <-- static creator and manager of all items
  SaveManager      <-- static save/load system using serialization
  LootGenerator    <-- static procedural item generation
  Utils            <-- flushInput, showTop, showCharacter, wrongInput, readInt functions used across.
```

### Key design principles

- Each controller owns its own `Page` enum and `done` flag. The run loop iterates until `done` is set, then returns control to the caller.
- `GameController` owns the player. All other classes receive either a `unique_ptr<Player>&` or a raw `Player*` depending on whether they need to write to the pointer itself or just read/modify the player's data.
- `ItemManager` is the sole creator for items. `Item` and its subclasses have private constructors - only `ItemManager` can create or destroy them.

---

## State Machine Pattern

Every controller class uses the same pattern:

```cpp
enum class Page { A, B, C };
Page currentPage = Page::A;
bool done = false;

void run() {
    while (!done) {
        system("CLS");
        showTop(player);          // redrawn every frame
        switch (currentPage) {
        case Page::A: showA(); break;
        case Page::B: showB(); break;
        case Page::C: showC(); break;
        }
    }
}
```

Each `show*` method reads input and updates `currentPage` or sets `done = true`. Returning from a `show*` method without changing `currentPage` causes the same page to redraw - this is how the wrong-input recovery works.

---

## Ownership

```
GameController
  unique_ptr<Player> mainCharacter_          <- sole owner

Player
  PlayerCharacter us                         <- value member (no pointer)

PlayerCharacter
  unique_ptr<PlayerCharacterDelegate> pcClass
  unique_ptr<Race> race_
  Item* equippedArmors[]                     <- owning raw pointers (managed by ItemManager/destructor)
  Item* equippedWeapons[]                    <- owning raw pointers
  vector<Item*> backpack                     <- owning raw pointers

DungeonController
  unique_ptr<Player>& player_                <- borrowed reference
  Dungeon dungeon                            <- value member

CombatController
  Player* player_                            <- borrowed raw pointer
  unique_ptr<Monster> monster_               <- owned for duration of combat

AbilityMenu
  Player* player_                            <- borrowed
  Monster* monster_                          <- borrowed (nullptr outside combat)
  unordered_map<string,int>& cooldowns_      <- borrowed reference to CombatController's map
```

### Rules

- If a class **owns** something, it uses `unique_ptr` or manages deletion explicitly.
- If a class only **reads or modifies** something it doesn't own, it uses a raw pointer or reference.
- `ItemManager` is the exception - it uses raw `Item*` throughout because its lifetime is managed manually through `markedForDeletion` and `cleanUpBackpack` methods.

---

## Class Reference

### GameController

**Purpose:** Top-level game loop. Owns the player and utilizes a switch statement to change between major game states depending on currentPage enum value (logic replicated for all subcontrollers).

**State enum:** `MAIN_MENU`, `CHARACTER_CREATION`, `TOWN`, `DUNGEON`, `EXIT`

**Key members:**
- `unique_ptr<Player> mainCharacter:`  the player, created during character creation and persisted across all states

**Notes:** `CharacterCreation::run()` returns a `unique_ptr<Player>` which `GameController` assigns to `mainCharacter`. All subsequent classes receive a reference or pointer to this.

---

### CharacterCreation

**Purpose:** Guides the player through class -> race -> name -> confirmation. Returns a fully constructed `Player`.

**State enum:** `CLASS_SELECT`, `CLASS_CONFIRM`, `RACE_SELECT`, `RACE_CONFIRM`, `NAME_ENTRY`, `FINAL_CONFIRM`

**Key members:**
- `unique_ptr<PlayerCharacterDelegate> playerClass` - built up across states
- `unique_ptr<Race> race_`
- `string name`
- `char lastClassChoice`, `char lastRaceChoice` - stored so confirm pages can re-display info

**Return:** `unique_ptr<Player>` from `run()`. `GameController` assigns this to `mainCharacter`.

---

### Player (struct)

**Purpose:** Stores `PlayerCharacter` with data that the game world needs but the character itself shouldn't know about.

**Members:**
- `PlayerCharacter us` - the character's stats, class, race, inventory
- `size_t daysAlive`
- `int gold`
- `string characterName`

---

### PlayerCharacter

**Purpose:** The character's full stat sheet, inventory, equipped items and abilities. Delegates class-specific behaviour to `PlayerCharacterDelegate`.

**Key design:** Brigdes the two - `PlayerCharacter` is the stable interface, `PlayerCharacterDelegate` holds class-specific data (HP, AP, level, EXP, abilities). Race bonuses are applied once at construction via `applyRaceBonuses`.

**Important:** Copy constructor and move constructor are deleted. `PlayerCharacter` cannot be copied - it must be moved or held by pointer.

**Friend:** `ItemManager` - required to access private `backpack`, `equippedArmors`, `equippedWeapons` for item management.

---

### PlayerCharacterDelegate (abstract)

**Purpose:** Base class for all playable classes (Cleric, Warrior, Rogue, Wizard). Holds HP, AP, level, EXP, abilities, and buffs.

**Pure virtual methods:**
- `levelUp()` - called automatically by `gainEXP` when EXP threshold is reached
- `getClassName()` - returns display name
- `getClassID()` - returns `ClassID` enum for serialisation
- `restoreStats(...)` - called by `SaveManager` to restore state without triggering constructor logic

**PCCONSTRUCT / LEVELUP macros:** Shared constructor and level-up logic used by all subclasses to avoid code duplication. Sets base HP, AP, and stats.

---

### StatBlock

**Purpose:** Stores and calculates base and buffed stats. Base class for `PlayerCharacterDelegate` and `Monster`.

**Key design:** Stats are split into `base` (permanent) and `fromBuffs` (recalculated whenever a buff is added). `increaseStats` modifies base permanently; buffs are added on top.

**Protected methods:**
- `increaseStats(...)` - permanently raises base stats (used at construction and level-up)
- `setBaseStats(...)` - overwrites base stats directly (used by `restoreStats` on load)
- `addNewBuff(Buff)` - adds or refreshes a buff and recalculates `fromBuffs`

---

### ItemManager (static)

**Purpose:** Sole factory and lifecycle manager for all `Item` objects.

**Why static:** Items need to be created and manipulated from many places (dungeon loot, save loading, combat, inventory). A static class avoids passing an `ItemManager` instance everywhere.

**Key methods:**
- `createArmor / createWeapon / createPotion` - allocate and return raw `Item*`
- `equip(Item*, PlayerCharacter*)` - moves item from backpack to equipment slot, swaps old item back to backpack, removes new item from backpack
- `use(Item*, PlayerCharacter*)` - applies potion effect, decrements quantity, marks for deletion and cleans up if quantity reaches zero
- `moveToBackpack / removeFromBackpack` - direct backpack manipulation
- `deleteItem` - explicit deallocation for items that never enter the backpack

**Ownership note:** All `Item*` returned by create methods are owning pointers. Once passed to `moveToBackpack` or `equip`, the backpack or equipment slot array takes ownership.

---

### SaveManager (static)

**Purpose:** Serialises and deserialises the full game state to a plain-text file.

**Save format:** Sectioned plain text using tags (`[PLAYER]`, `[INVENTORY]`, `[EQUIPPED_ARMOR]`, `[EQUIPPED_WEAPONS]`, `[ABILITIES]`). Numeric fields use whitespace separation. Item names are always written last on their line so `getline` can safely read multi-word names.

**Key design decision - `LoadTag`:** `PlayerCharacter`'s constructor applies race bonuses. On load, `restoreStats` already includes those bonuses from the saved data. To prevent double application, a `LoadTag{}` overload of the `Player` and `PlayerCharacter` constructors skips `applyRaceBonuses`.

**Stream safety:** `file.exceptions(failbit | badbit)` is set before reading so parse failures throw `std::ios_base::failure` rather than silently producing garbage values.

---

### LootGenerator (static)

**Purpose:** Procedurally generates items appropriate to the current dungeon depth and places them into the player's backpack.

**Tier system:** Total stat value of a generated item determines its tier (WORN -> COMMON -> STURDY -> FINE -> SUPERIOR -> LEGENDARY). Tier drives the name prefix.

**Name construction:** Base name reflects the dominant stat (e.g. high-strength armor -> "Plate", high-agility -> "Leather"). Slot suffix appended at the end. Potion names are derived from heal amount only.

**Depth scaling:** Monster weight increases with depth; potion weight decreases. Stat roll ranges scale with `depth / 2`.

---

### Dungeon

**Purpose:** Holds the 10x10 room grid and handles procedural generation.

**Generation algorithm:**
1. Clear all rooms to `EMPTY`
2. Place `ENTRANCE` at a random top-row column, `EXIT` at a random bottom-row column
3. `makeGuaranteedPath` - random walk from entrance to exit, opening passages at each step. Loop condition `x != exitX || y != exitY` ensures both axes are resolved.
4. `addRandomPassages` - iterates all cells and randomly opens extra passages (40% per direction) to break linearity
5. `scatterRooms` - assigns random `RoomType` to all non-entrance/exit rooms using depth-weighted probabilities

**Passage model:** Each `Room` has four `bool` flags (north, south, east, west). Two adjacent rooms must both have their facing flags set for a passage to exist. `openAdjacentPassages` sets both sides open.

---

### DungeonController

**Purpose:** Manages dungeon exploration - player movement, room reveals, and encounter dispatch.

**State enum:** `EXPLORING`, `MONSTER_ENCOUNTER`, `TREASURE_ENCOUNTER`, `TRAP_ENCOUNTER`, `FLOOR_COMPLETE`, `EXIT`

---

### CombatController

**Purpose:** Manages a single combat encounter between the player and one monster.

**State enum:** `PLAYER_TURN`, `MONSTER_TURN`, `RESULT`

**Turn flow:**
```
PLAYER_TURN -> (player chooses action) -> MONSTER_TURN
MONSTER_TURN -> (monster attacks) -> tickCooldowns() -> PLAYER_TURN
Either turn -> monster or player HP reaches 0 -> RESULT
```

**Hit chance formula:**
```
hitChance = 70 + (attackerAgi - defenderAgi) * 3
forced to be around [10, 95]
```

**Cooldown management:** `cooldowns_` is an `unordered_map<string, int>` owned by `CombatController` and passed by reference to `AbilityMenu`. `tickCooldowns()` decrements all values by 1 at the end of `MONSTER_TURN`. Cooldowns reset between fights because the map is destroyed with `CombatController`.

**Outcome enum:** `WON`, `LOST`, `FLED`. Returned to `DungeonController` which decides what happens to the room.

---

### CombatController - Flee mechanic

```
fleeChance = 50 + (playerAgi - monsterAgi) * 5
clamped to [20, 90]
```

---

### AbilityMenu

**Purpose:** Displays and executes ability use, inside or outside combat. Although not really implemented outside combat.

**Two construction modes:**
- `AbilityMenu(Player*)` - view only, no monster target, dummy cooldown map
- `AbilityMenu(Player*, Monster*, unordered_map&)` - full combat mode with live cooldown table

**Ability resolution order:**
1. Spend AP
2. Calculate total effect (base + scalar stat)
3. Apply effect to target (heal self, damage enemy, or apply buff)
4. Set cooldown in the shared map

**Result enum:** `NONE`, `ABILITY_USED`, `BACK`. `ABILITY_USED` signals `CombatController` to advance to `MONSTER_TURN`.

---

### Inventory

**Purpose:** Displays backpack contents and allows equipping or using items.

**Multi-digit selection:** Uses `readInt()` (from `Utils`) which reads a full line and parses it as an integer, supporting item indices above 9.

---

### Monster

**Purpose:** A simple combat opponent. Inherits `StatBlock` for stat storage and buff application.

**Key members:**
- `unique_ptr<PointWell> HP`
- `expType expReward` - granted to player on the monster's death
- `damagetype minDamage, maxDamage`

**Creation:** `makeMonsterForDepth(int depth)` selects a random monster from the appropriate tier and applies some stat scaling per floor.

---

## Systems Documentation

### Save System - File Format

```
[PLAYER]
CharacterName
classID
raceID
daysAlive
gold
depthReached
level
currentEXP
expToNextLevel
currentHP maxHP
currentAP maxAP
baseStr baseInt baseAgi baseArm baseElRes

[INVENTORY]
itemCount
ARMOR str int agi arm elres slotID name
WEAPON str int agi arm elres minDmg maxDmg is2H slotID name
POTION healAmount quantity name

[EQUIPPED_ARMOR]
str int agi arm elres slotID name
END

[EQUIPPED_WEAPONS]
str int agi arm elres minDmg maxDmg is2H slotID name

[ABILITIES]
HPEffect cost cooldown target scalar_ hasBuff (duration isDebuff str int agi arm elres name)
name_
END
```
The stats of the buff are only present if hasBuff == 1. Additionally, name_ is also conditionally on a sperate line to allow for getline usage for both names.


**ClassID enum values:** CLERIC=1, WARRIOR=2, ROGUE=3, WIZARD=4

**RaceID enum values:** HUMAN=1, ELF=2, DWARF=3, DARKELF=4

**ArmorSlot enum values:** HEAD=0, CHEST=1, LEGS=2, FEET=3, HANDS=4

**WeaponSlot enum values:** MELEE=0, RANGED=1

---

### Item System - Lifecycle

```
ItemManager::create*()
    -> new Item(new Delegate())    <- allocated on heap

ItemManager::moveToBackpack()
    -> backpack.push_back(item)    <- backpack takes ownership

ItemManager::equip()
    -> equippedSlot = item         <- slot takes ownership
    -> removeFromBackpack(item)    <- removed from backpack

ItemManager::use()  (potions only)
    -> decrement quantity
    -> if quantity == 0:
        item->markedForDeletion = true
        cleanUpBackpack()         <- erases from vector and deletes

PlayerCharacter::~PlayerCharacter()
    -> delete equippedArmors[i]
    -> delete equippedWeapons[i]   <- equipment slots cleaned up on destruction
```
