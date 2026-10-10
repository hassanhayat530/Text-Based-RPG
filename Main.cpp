#include <iostream>
#include "PlayerCharacter.h"
#include "ItemManager.h"

//int main() {
//
//	CoreStats plateArmorStats;
//	plateArmorStats.armor = 5;
//	plateArmorStats.elementRes = 3;
//
//	Item* plateMail = ItemManager::createArmor("Shiny Plate Armor", CoreStats(0,0,0,5,3), ArmorSlot::CHEST);
//	Item* leatherArmor = ItemManager::createArmor("Plain Leather Armor", CoreStats(0,0,0,2,3), ArmorSlot::CHEST);
//	Item* longSword = ItemManager::createWeapn("Long Sword", CoreStats(), WeaponSlot::MELEE, 3, 9);
//
//	ItemManager::equip(plateMail, &p1);
//	ItemManager::equip(leatherArmor, &p1);
//	ItemManager::equip(longSword, &p1);
//
//	
//
//	std::cout << "health before taking damage: " << p1.getCurrentHP() << '/' << p1.getMaxHP() << '\n';
//	p1.takeDamage(20);
//	std::cout << "health after taking damage: " << p1.getCurrentHP() << '/' << p1.getMaxHP() << '\n';
//	Item* healPotion = ItemManager::createPotion("Minor Heal Potion", 3u, 3u);
//	ItemManager::moveToBackpack(healPotion, &p1);
//	ItemManager::use(healPotion, &p1);
//	std::cout << "health after using potion: " << p1.getCurrentHP() << '/' << p1.getMaxHP() << '\n';
//
//	ItemManager::moveToBackpack(
//		ItemManager::createWeapn("Rusty Hand Axe", CoreStats(), WeaponSlot::MELEE, 2, 4),
//		&p1);
//
//	{
//		auto inv = p1.getBackpackList();
//		std::cout << "Inventory: ";
//		for (auto it : inv) {
//			std::cout << *it << " ";
//		}
//
//		ItemManager::use(healPotion, &p1);
//		ItemManager::use(healPotion, &p1);
//	}
//
//	std::cout << '\n';
//
//	{
//		auto inv = p1.getBackpackList();
//		std::cout << "Inventory (after using health potions): ";
//		for (auto it : inv) {
//			std::cout << *it << " ";
//		}
//	}
//
//	return 0;
//}