# RPG - User Manual

---

## Table of Contents

1. [Getting Started](#getting-started)
2. [Main Menu](#main-menu)
3. [Character Creation](#character-creation)
4. [Town](#town)
5. [Dungeon](#dungeon)
6. [Combat](#combat)
7. [Inventory](#inventory)
8. [Stats Reference](#stats-reference)
9. [Controls Reference](#controls-reference)

---

## Getting Started

Launch the game by running the executable. The game runs entirely in a console window - all input is keyboard only. You will be taken to the main menu.

---

## Main Menu

```
Choose an option:
1 = New Game
2 = Load Game
3 = Exit
```

- **New Game** - begins character creation. Any existing save file is not affected until you sleep in the Tavern or Slums for the first time.
- **Load Game** - loads your most recent save and takes you directly to the Town. If no save file exists the game will tell you and return to this menu.
- **Exit** - closes the game.

---

## Character Creation

Character creation has four steps. You can go back to any previous step from the final confirmation screen.

### Step 1 - Choose a Class

Your class determines your starting stats, which abilities you learn, and how you grow when you level up.

| Class   | HP | AP | Strength | Intelligence | Agility | Playstyle                                                    |
|---------|----|----|----------|--------------|---------|--------------------------------------------------------------|
| Cleric  | 14 | 10 | 3        | 5            | 1       | Balanced fighter with healing and holy damage abilities      |
| Warrior | 18 | 10 | 6        | 2            | 2       | Heavy melee fighter, highest HP and strength                 |
| Rogue   | 12 | 12 | 3        | 3            | 6       | Fast and evasive, high agility improves hit chance and dodge |
| Wizard  | 10 | 14 | 1        | 8            | 2       | Frail but powerful, abilities scale with Intelligence        |

After reading the class description, press **y** to confirm or **n** to go back and choose again.

### Step 2 - Choose a Race

Your race adds bonus stats on top of your class base stats.

| Race     | HP | AP | Strength | Intelligence | Agility | Notes                                     |
|----------|----|----|----------|--------------|---------|-------------------------------------------|
| Human    | +2 | +0 | +2       | +0           | +1      | Well-rounded, suits any class             |
| Elf      | +0 | +0 | +0       | +3           | +2      | Best for spellcasters and rogues          |
| Dwarf    | +3 | +0 | +1       | +0           | +1      | Tough and resilient, suits warriors       |
| Dark Elf | +0 | +0 | +0       | +2           | +3      | Extreme agility, suits rogues and wizards |

Press **y** to confirm your race or **n** to go back.

### Step 3 - Choose a Name

Type your character's name and press Enter.

**Name rules:**
- At least 2 letters
- No more than 20 characters
- Letters, spaces, hyphens, and apostrophes are allowed
- Numbers and special characters are not allowed

Examples of valid names: `Aryn`, `Al-Hassan`, `O'Brien`, `Dark Knight`

### Step 4 - Final Confirmation

Review your full character - class, race, and name. You are given the option to go back to any step:

```
1 = Back to Class Selection
2 = Back to Race Selection
3 = Back to Choosing Name
y = Confirm and begin
```

Pressing **y** creates your character and takes you to the Town.

---

## Town

The Town is your home base between dungeon runs. It is also where your game is saved.

### Town Screen
**Your game is saved when you enter the town.** - Also prints a message

The top of every screen shows your current status:

```
----------------------------------Day: 3----------------------------------
HP: 14/18          EXP: 120/200          AP: 8/10
```

- **Day** - how many days your character has lived
- **HP** - current and maximum health points
- **EXP** - experience points toward your next level up
- **AP** - current and maximum ability points

### Town Options

```
1 = Go to Slums
2 = Go to the Tavern
3 = Leave for Dungeon
```

### Tavern

Sleeping in the Tavern costs **gold** depending on your current level and gives you a full rest - your HP and AP are restored and you level up if you have enough EXP. 

### Slums

Sleeping in the Slums is free but you sleep with one eye open - you do not heal but you level up.

### Leaving for the Dungeon

You can leave for the dungeon without resting. You will not heal or level up. A confirmation screen warns you of this before you commit.

### Dungeon Depth Selection

After you sleep, (or directly leave) you enter a dungeon. You can select a checkpoint if you've reached a higher depth from one of the following in your save (except 1 which is always available):
- 1
- 4
- 7
- 10

---

## Dungeon

### Reading the Map

```
--------- Depth 1 ---------
? ? ? ? ?   ? ? ? ?
? ? ? ? ?   ? ? ? ?
? ? ? ? ?     ? ? ?
? ? ? ? ?     ? ? ?
? ? ? ?     ? ? ? ?
? ? ?       ? ? ? ?
? ? ?   ? ? ? ? ? ?
? ? ?   ? ? ? ? ? ?
? ? ?         ? ? ?
? ? ? ? ? ?       ?

"?" = not cleared     " " = cleared
```

### Movement

You can only move through open passages. Available directions are shown on screen:

```
w = North   s = South   d = East   a = West
```

If a direction is not shown, there is no passage that way - pressing that key will give an error.

### Room Types

**Empty** - you walk through without anything happening. The room is marked cleared.

**Monster** - a combat encounter starts immediately. See the [Combat](#combat) section.
- If you win, the room is cleared.
- If you flee, the room is not cleared.

**Treasure** - an item is automatically added to your backpack and some gold. Check your inventory with **i** to see what you found.

**Trap** - you take damage scaled to the dungeon depth. Higher floors have stronger traps. If a trap drops you to 0 HP, you die and are returned to the main menu.

**Exit** - you are given a choice:
```
1 = Descend deeper (Floor 2)
2 = Return to town
```
Descending generates a new, harder floor. Returning to town saves automatically.

### Dungeon Depth

Each floor below the first is harder:
- Monster stats scale with depth
- Traps deal more damage
- Loot becomes more powerful
- Monster encounter frequency increases

### Dungeon Actions

From the exploration screen you can always:

| Key       | Action                 |
|-----------|------------------------|
| `w/a/s/d` | Move                   |
| `c`       | View character details |
| `i`       | Open inventory         |

---

## Combat

Combat is turn-based. You always act first.

### Combat Screen

```
--------- Combat ---------
You:          HP 14 / 18
Skeleton:     HP 11 / 11 [-Stun(1)]
--------------------------
You attacked Skeleton for 3 damage.
Skeleton's attack missed!
[Skeleton's Disorient wore off]

--- Your Turn ---
1 = Melee Attack
3 = Use Ability
4 = Use Item
5 = Flee
```

The top section shows both combatants' current and maximum HP as well as buffs with their cooldowns. Below that is a log of what happened last round. At the bottom are your available options.

### Your Options

**1 - Melee Attack**
Your character swings their equipped melee weapon. If no weapon is equipped, your base strength is used for a weak unarmed strike. Hitting is not guaranteed - your agility vs the monster's agility determines the chance to connect.

**2 - Ranged Attack** *(only shown if you have a ranged weapon equipped)*
Fires your ranged weapon. Ranged attacks gain a bonus from agility. No ranged weapon means this option does not appear.

**3 - Use Ability**
Opens your ability list. Abilities cost AP and may have cooldowns. See [Abilities](#abilities) below.

**4 - Use Item**
Opens a filtered view of your inventory showing only usable items (potions). Using an item does **not** end your turn - you still act this round.

**5 - Flee**
Attempt to escape. Success is based on your agility versus the monster's agility - a faster character flees more reliably. The minimum flee chance is 20% and the maximum is 90%. If you fail, the monster still attacks. The room is not cleared if you escape.

### Abilities

Your abilities are listed with their AP cost, cooldown, and target shown. You cannot use an ability if:
- You do not have enough AP
- The ability is on cooldown (turns remaining shown next to it)

**Ability types:**
- **Damage (Enemy target)** - deals magic damage reduced by the monster's element resistance
- **Heal (Self target)** - restores HP, often scaled by Intelligence
- **Buff (Self target)** - improves your stats for a number of turns

Cooldowns count down one turn per round. They reset between fights.

### Damage and Defence

**Hit chance:**
```
Base 70% + (your Agility - enemy Agility) x 3%
Clamped between 10% and 95%
```

**Damage reduction:**
```
Final damage = attack damage - target's Armor (minimum 1)
Magic damage = ability damage - target's Element Resistance (minimum 1)
```

### Death

If your HP reaches 0, the combat ends immediately with a death screen. You are returned to the main menu. **Your last save is not deleted** - you can load and try again from where you last entered town.

---

## Inventory

Open inventory from any page after character creation by pressing **i**.

### Item List

```
-----------------------------Inventory-----------------------
Gold: 42
[1] (Weapon) Sturdy Sword
[2] (Armor)  Fine Leather Chest
[3] (Potion) Health Potion x2
[10] (Armor) Common Plate Helm

Enter item number to inspect, or 0 = back, c = character
```

Type the number next to an item and press Enter. Numbers above 9 work - type the full number (e.g. `10`) and press Enter.

Press **0** to go back. Press **c** to view your character details without leaving the inventory.

### Armour Detail

```
-----------------------------Armor---------------------------
Name:               | Worn Armor Legs
Strength:           | 0
Intelligence:       | 0
Agility:            | 0
Armor:              | 2
Element Resistance: | 0

-----------Currently Equipped in This Slot-----------
Nothing equipped in this slot.

-------------------------------------------------------------
e = equip     r = destroy     b = back
```

Press **e** to equip. If you already have something in that slot, the old item goes back to your backpack automatically.

### Weapon Detail

```
-----------------------Weapon--------------------------
Name:               | Sturdy Enchanted Shortbow
Damage:             | 3 - 7
Two-handed:         | No
Strength:           | 0
Intelligence:       | 1
Agility:            | 0

-----------Currently Equipped in This Slot-----------
Name:               | Sturdy Shortbow
Damage:             | 3 - 7
Two-handed:         | No
Strength:           | 1
Intelligence:       | 0
Agility:            | 0

-------------------------------------------------------------
e = equip     r = destroy     b = back
```

Press **e** to equip in the appropriate slot (Melee or Ranged). Two-handed weapons occupy the melee slot.

### Potion Detail

```
-----------------------------Potion--------------------------
Name:               | Health Potion
Heals:              | 25 HP
Quantity:           | 2

u = use     b = back
```

Press **u** to drink the potion. This heals you immediately. Quantity decreases by one. When the last potion is consumed the item is removed from your inventory automatically.

**Note:** You cannot use a potion if your HP is already full and the potion has no buff effect.

---

## Stats Reference

### Primary Stats

| Stat                   | What it does                                                                     |
|------------------------|----------------------------------------------------------------------------------|
| **Strength**           | Increases melee damage (1/4 of Strength added to melee attacks)                  |
| **Intelligence**       | Increases ability damage and healing for spells that scale with it               |
| **Agility**            | Increases hit chance, dodge chance, and flee success rate                        |
| **Armor**              | Reduces all incoming physical damage (minimum 1 damage always gets through)      |
| **Element Resistance** | Reduces incoming magic damage and ability damage (minimum 1 always gets through) |

### HP and AP

| Stat                   | What it does                                                                |
|------------------------|-----------------------------------------------------------------------------|
| **HP (Health Points)** | Reaching 0 means death. Restored by sleeping, potions, or healing abilities |
| **AP (Magic Points)**  | Spent to use abilities. Restored by sleeping.                               |

### Level and EXP

Defeating monsters grants EXP. When your EXP reaches the threshold for your current level, you level up the next time you sleep in town. Levelling up increases your HP, AP, and all base stats by half their base value. The EXP threshold doubles each level.

### How Stats Combine

Your displayed **Total** stat is:

```
Total = Base Class Stat + Race Bonus + Equipment Bonuses + Active Buff Bonuses
```

The character screen shows total stats. The base stats screen in character creation shows class and race values before equipment.

---

## Controls Reference

### Universal (available after character creation)

| Key        | Action                         |
|------------|--------------------------------|
| `c`        | View character details         |
| `i`        | Open inventory                 |
| `b` or `0` | Go back / close current screen |

### Town

| Key       | Action                        |
|-----------|-------------------------------|
| `1`       | Go to Slums                   |
| `2`       | Go to the Tavern              |
| `3`       | Leave for Dungeon             |
| `y` / `n` | Confirm / Cancel sleep prompt |

### Dungeon - Exploration

| Key | Action                       |
|-----|------------------------------|
| `w` | Move North (if passage open) |
| `s` | Move South (if passage open) |
| `d` | Move East (if passage open)  |
| `a` | Move West (if passage open)  |

### Dungeon - Floor Complete

| Key | Action                |
|-----|-----------------------|
| `1` | Descend to next floor |
| `2` | Return to town        |

### Combat

| Key | Action                             |
|-----|------------------------------------|
| `1` | Melee attack                       |
| `2` | Ranged attack (if weapon equipped) |
| `3` | Use ability                        |
| `4` | Use item (potions only)            |
| `5` | Flee                               |

### Inventory

| Key                                     | Action                         |
|-----------------------------------------|--------------------------------|
| `1`-`9` (or multi-digit number + Enter) | Inspect item                   |
| `e`                                     | Equip selected armor or weapon |
| `u`                                     | Use selected potion            |
| `0` or `b`                              | Go back                        |
| `c`                                     | View character                 |

### Ability Menu (outside combat)

| Key     | Action              |
|---------|---------------------|
| `1`-`9` | View ability detail |
| `b`     | Go back             |

### Ability Menu (in combat)

| Key     | Action                    |
|---------|---------------------------|
| `1`-`9` | Select ability            |
| `u`     | Use selected ability      |
| `b`     | Go back to combat options |