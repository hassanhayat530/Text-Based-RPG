# Text-Based RPG

A console RPG written in C++. Create a character, rest and save in town, then descend through procedurally generated dungeon floors fighting monsters, collecting loot, and levelling up. All input is keyboard only.

## Features

- **Four classes** (Cleric, Warrior, Rogue, Wizard) and **four races** (Human, Elf, Dwarf, Dark Elf), each with different stats and abilities
- **Town hub** with a Tavern (paid full rest), Slums (free rest, no healing), and automatic saving
- **Procedural dungeons** on a 10x10 grid with a guaranteed path from entrance to exit, plus empty, monster, treasure, and trap rooms
- **Depth scaling**: deeper floors bring stronger monsters and traps, better loot, and more frequent encounters, with checkpoints at depths 1, 4, 7, and 10
- **Turn-based combat** with melee and ranged attacks, abilities with AP costs and cooldowns, potions, buffs, and fleeing
- **Procedural loot** with tiered items from Worn to Legendary, and an inventory for equipping, using, and destroying items
- **Save and load** through a plain-text save file

## Building

The build output is not included in this repository, so you need to build the project before running it.

1. Clone the repository.
2. Open the solution (`.sln`) in Visual Studio.
3. Build the project (Build > Build Solution) and run the resulting executable.

The game clears the screen with the Windows `CLS` command, so it is intended for Windows.

## Documentation

| File | Contents |
|---|---|
| [`USER_MANUAL.md`](USER_MANUAL.md) | How to play: menus, character creation, town, dungeon, combat, inventory, stats, and a full controls reference |
| [`PROGRAMMING_DOCUMENTATION.md`](PROGRAMMING_DOCUMENTATION.md) | How the code works: architecture, state machine pattern, ownership model, class reference, and the save file format |

## Code Overview

`GameController` owns the player and moves between the game phases. Each phase is handled by its own controller:

```
GameController
├── CharacterCreation   new game flow
├── Town                rest, save, choose next action
└── DungeonController   exploration
    ├── Dungeon         map generation
    ├── CombatController
    │   └── AbilityMenu
    └── Inventory
```

Supporting static systems handle items (`ItemManager`), saving (`SaveManager`), and loot generation (`LootGenerator`). Every controller follows the same pattern of a `Page` enum and a `done` flag that drives its run loop.
