#include "pch.h"
#include "CppUnitTest.h"
#include "Warrior.h"
#include "Rogue.h"
#include "Cleric.h"
#include "Wizard.h"
#include "Types.h"
#include "ItemManager.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace RPGSystemsUnitTest
{
	TEST_CLASS(RPGSystemsUnitTest)
	{
	public:
        TEST_METHOD(Core_Abilities) {
            {
                Ability default_abil;
                Assert::AreEqual(std::string("unnamed"), default_abil.name_);
                Assert::AreEqual(1u, default_abil.HPEffect_);
                Assert::IsNull(default_abil.givenBuff_);
                Assert::AreEqual(0u, default_abil.cost_);
                Assert::AreEqual(1u, default_abil.cooldown_);
                Assert::AreEqual((int)AbilityTarget::SELF, (int)default_abil.target_);
                Assert::AreEqual((int)AbilityScalar::NONE, (int)default_abil.scalar_);
            }
            {
                Ability constructed_abil("custom_name", 2u,
                    new Buff("thicc", 1, 0, 0, 1, 1, 5, false),
                    3u, 4u, AbilityTarget::ENEMY, AbilityScalar::STR);
                Assert::AreEqual(std::string("custom_name"), constructed_abil.name_);
                Assert::IsNotNull(constructed_abil.givenBuff_);
                Assert::AreEqual(2u, constructed_abil.HPEffect_);
                Assert::AreEqual(3u, constructed_abil.cost_);
                Assert::AreEqual(4u, constructed_abil.cooldown_);
                Assert::AreEqual((int)AbilityTarget::ENEMY, (int)constructed_abil.target_);
                Assert::AreEqual((int)AbilityScalar::STR, (int)constructed_abil.scalar_);
            }
        }

        TEST_METHOD(Core_Buffs) {
            {
                Buff a_buff;
                Assert::AreEqual(std::string("undefined"), a_buff.name);
                Assert::AreEqual(0, (int)a_buff.buffedStats.strength);
                Assert::AreEqual(0, (int)a_buff.buffedStats.intellect);
                Assert::AreEqual(0, (int)a_buff.buffedStats.agility);
                Assert::AreEqual(0, (int)a_buff.buffedStats.armor);
                Assert::AreEqual(0, (int)a_buff.buffedStats.elementRes);
                Assert::AreEqual(2, (int)a_buff.duration);
                Assert::IsFalse(a_buff.isDebuff);
            }
            {
                Buff a_buff("somename", CoreStats(1, 2, 3, 4, 5), 3, true);
                Assert::AreEqual(1, (int)a_buff.buffedStats.strength);
                Assert::AreEqual(2, (int)a_buff.buffedStats.intellect);
                Assert::AreEqual(3, (int)a_buff.buffedStats.agility);
                Assert::AreEqual(4, (int)a_buff.buffedStats.armor);
                Assert::AreEqual(5, (int)a_buff.buffedStats.elementRes);
                Assert::AreEqual(3, (int)a_buff.duration);
                Assert::IsTrue(a_buff.isDebuff);
            }
            {
                Buff b_buff("somename", 1, 2, 3, 4, 5, 3, true);
                Assert::AreEqual(1, (int)b_buff.buffedStats.strength);
                Assert::AreEqual(2, (int)b_buff.buffedStats.intellect);
                Assert::AreEqual(3, (int)b_buff.buffedStats.agility);
                Assert::AreEqual(4, (int)b_buff.buffedStats.armor);
                Assert::AreEqual(5, (int)b_buff.buffedStats.elementRes);
                Assert::AreEqual(3, (int)b_buff.duration);
                Assert::IsTrue(b_buff.isDebuff);
            }
        }

        TEST_METHOD(Core_Stats) {
            CoreStats statsDefault;
            Assert::AreEqual(0, (int)statsDefault.strength);
            Assert::AreEqual(0, (int)statsDefault.intellect);
            Assert::AreEqual(0, (int)statsDefault.agility);
            Assert::AreEqual(0, (int)statsDefault.armor);
            Assert::AreEqual(0, (int)statsDefault.elementRes);
            CoreStats statsa(5);
            Assert::AreEqual(5, (int)statsa.strength);
            Assert::AreEqual(5, (int)statsa.intellect);
            Assert::AreEqual(5, (int)statsa.agility);
            Assert::AreEqual(5, (int)statsa.armor);
            Assert::AreEqual(5, (int)statsa.elementRes);
            CoreStats statsb(1, 2, 3, 4, 5);
            Assert::AreEqual(1, (int)statsb.strength);
            Assert::AreEqual(2, (int)statsb.intellect);
            Assert::AreEqual(3, (int)statsb.agility);
            Assert::AreEqual(4, (int)statsb.armor);
            Assert::AreEqual(5, (int)statsb.elementRes);
            statsDefault += statsa;
            Assert::AreEqual(5, (int)statsDefault.strength);
            Assert::AreEqual(5, (int)statsDefault.intellect);
            Assert::AreEqual(5, (int)statsDefault.agility);
            Assert::AreEqual(5, (int)statsDefault.armor);
            Assert::AreEqual(5, (int)statsDefault.elementRes);
            statsDefault -= statsa;
            Assert::AreEqual(0, (int)statsDefault.strength);
            Assert::AreEqual(0, (int)statsDefault.intellect);
            Assert::AreEqual(0, (int)statsDefault.agility);
            Assert::AreEqual(0, (int)statsDefault.armor);
            Assert::AreEqual(0, (int)statsDefault.elementRes);
            statsDefault = statsb;
            Assert::AreEqual(1, (int)statsb.strength);
            Assert::AreEqual(2, (int)statsb.intellect);
            Assert::AreEqual(3, (int)statsb.agility);
            Assert::AreEqual(4, (int)statsb.armor);
            Assert::AreEqual(5, (int)statsb.elementRes);
        }

        TEST_METHOD(WeaponItem) {
            Item* aMagicWeapon = ItemManager::createWeapon("Magic Weapon", CoreStats(5), WeaponSlot::MELEE, 1, 3, false);
            Assert::IsNotNull(aMagicWeapon);
            Assert::IsTrue(ItemManager::isItemWeapon(aMagicWeapon));
            Assert::IsFalse(ItemManager::isItemArmor(aMagicWeapon));
            Assert::IsFalse(ItemManager::isItemPotion(aMagicWeapon));
            // const cast check
            {
                const Weapon* weapon = dynamic_cast<const Weapon*>(aMagicWeapon->getItem());
                Assert::IsNotNull(weapon);  // make sure cast doesn't fail
                Assert::AreEqual(std::string("Magic Weapon"), weapon->name);
                Assert::AreEqual(5, (int)weapon->stats.strength);
                Assert::AreEqual(5, (int)weapon->stats.intellect);
                Assert::AreEqual(5, (int)weapon->stats.agility);
                Assert::AreEqual(5, (int)weapon->stats.armor);
                Assert::AreEqual(5, (int)weapon->stats.elementRes);
                Assert::AreEqual((int)WeaponSlot::MELEE, (int)weapon->slot);
                Assert::AreEqual(1, (int)weapon->minDamage);
                Assert::AreEqual(3, (int)weapon->maxDamage);
                Assert::IsFalse(weapon->is2H);
                //delete weapon;  // should have a compiler error
            }

            // modify weapon test
            {
                Weapon* weapon_mod = nullptr;
                ItemManager::castItemToWeapon(aMagicWeapon, weapon_mod);
                Assert::IsNotNull(weapon_mod);  // make sure cast doesn't fail
                Assert::AreEqual(std::string("Magic Weapon"), weapon_mod->name);
                Assert::AreEqual(5, (int)weapon_mod->stats.strength);
                Assert::AreEqual(5, (int)weapon_mod->stats.intellect);
                Assert::AreEqual(5, (int)weapon_mod->stats.agility);
                Assert::AreEqual(5, (int)weapon_mod->stats.armor);
                Assert::AreEqual(5, (int)weapon_mod->stats.elementRes);
                Assert::AreEqual((int)WeaponSlot::MELEE, (int)weapon_mod->slot);
                Assert::AreEqual(1, (int)weapon_mod->minDamage);
                Assert::AreEqual(3, (int)weapon_mod->maxDamage);
                Assert::IsFalse(weapon_mod->is2H);

                // change things
                weapon_mod->name = "+1 Magic Weapon";
                weapon_mod->stats.strength += 1;
                weapon_mod->stats.intellect += 1;
                weapon_mod->stats.agility += 1;
                weapon_mod->stats.armor += 1;
                weapon_mod->stats.elementRes += 1;
                weapon_mod->minDamage += 1;
                weapon_mod->maxDamage += 1;
                Assert::AreEqual(std::string("+1 Magic Weapon"), weapon_mod->name);
                Assert::AreEqual(6, (int)weapon_mod->stats.strength);
                Assert::AreEqual(6, (int)weapon_mod->stats.intellect);
                Assert::AreEqual(6, (int)weapon_mod->stats.agility);
                Assert::AreEqual(6, (int)weapon_mod->stats.armor);
                Assert::AreEqual(6, (int)weapon_mod->stats.elementRes);
                Assert::AreEqual((int)WeaponSlot::MELEE, (int)weapon_mod->slot);
                Assert::AreEqual(2, (int)weapon_mod->minDamage);
                Assert::AreEqual(4, (int)weapon_mod->maxDamage);
                Assert::IsFalse(weapon_mod->is2H);
            }

            ItemManager::deleteItem(aMagicWeapon);
            Assert::IsNull(aMagicWeapon);
        }

        TEST_METHOD(ArmorItems) {
            Item* armor_item = ItemManager::createArmor("SomeArmor", CoreStats(1, 2, 3, 4, 5), ArmorSlot::HEAD);
            Assert::IsNotNull(armor_item);
            Assert::IsFalse(ItemManager::isItemWeapon(armor_item));
            Assert::IsTrue(ItemManager::isItemArmor(armor_item));
            Assert::IsFalse(ItemManager::isItemPotion(armor_item));

            // const casts
            {
                const Armor* armor = dynamic_cast<const Armor*>(armor_item->getItem());
                Assert::IsNotNull(armor);
                Assert::AreEqual(std::string("SomeArmor"), armor->name);
                Assert::AreEqual(1, (int)armor->stats.strength);
                Assert::AreEqual(2, (int)armor->stats.intellect);
                Assert::AreEqual(3, (int)armor->stats.agility);
                Assert::AreEqual(4, (int)armor->stats.armor);
                Assert::AreEqual(5, (int)armor->stats.elementRes);
                Assert::AreEqual((int)ArmorSlot::HEAD, (int)armor->slot);
            }


            // mutable casts
            {
                Armor* armor_mod = nullptr;
                ItemManager::castItemToArmor(armor_item, armor_mod);
                Assert::IsNotNull(armor_mod);
                Assert::AreEqual(std::string("SomeArmor"), armor_mod->name);
                Assert::AreEqual(1, (int)armor_mod->stats.strength);
                Assert::AreEqual(2, (int)armor_mod->stats.intellect);
                Assert::AreEqual(3, (int)armor_mod->stats.agility);
                Assert::AreEqual(4, (int)armor_mod->stats.armor);
                Assert::AreEqual(5, (int)armor_mod->stats.elementRes);
                Assert::AreEqual((int)ArmorSlot::HEAD, (int)armor_mod->slot);

                // make changes
                armor_mod->name = "+1 Scale Armor";
                armor_mod->stats.strength = 2;
                armor_mod->stats.intellect = 2;
                armor_mod->stats.agility = 2;
                armor_mod->stats.armor = 5;
                armor_mod->stats.elementRes = 1;
                armor_mod->slot = ArmorSlot::CHEST;

                Assert::AreEqual(std::string("+1 Scale Armor"), armor_mod->name);
                Assert::AreEqual(2, (int)armor_mod->stats.strength);
                Assert::AreEqual(2, (int)armor_mod->stats.intellect);
                Assert::AreEqual(2, (int)armor_mod->stats.agility);
                Assert::AreEqual(5, (int)armor_mod->stats.armor);
                Assert::AreEqual(1, (int)armor_mod->stats.elementRes);
                Assert::AreEqual((int)ArmorSlot::CHEST, (int)armor_mod->slot);
            }
            ItemManager::deleteItem(armor_item);
            Assert::IsNull(armor_item);
        }

        TEST_METHOD(PotionItems) {
            Item* random_potion = ItemManager::createPotion("Random Potion", 3, 4, nullptr);
            Assert::IsNotNull(random_potion);
            Assert::IsFalse(ItemManager::isItemWeapon(random_potion));
            Assert::IsFalse(ItemManager::isItemArmor(random_potion));
            Assert::IsTrue(ItemManager::isItemPotion(random_potion));

            // const cast
            {
                const Potion* const_potion = dynamic_cast<const Potion*>(random_potion->getItem());
                Assert::AreEqual(std::string("Random Potion"), const_potion->name);
                Assert::AreEqual(3, (int)const_potion->healAmount);
                Assert::AreEqual(4, (int)const_potion->quantity);
                Assert::IsNull(const_potion->getBuff());
            }

            // modify potion
            {
                Potion* potion = nullptr;
                ItemManager::castItemToPotion(random_potion, potion);
                Assert::AreEqual(std::string("Random Potion"), potion->name);
                Assert::AreEqual(3, (int)potion->healAmount);
                Assert::AreEqual(4, (int)potion->quantity);

                // modify
                potion->name = "Super Potion";
                potion->healAmount = 10;
                potion->quantity = 1;
                potion->setBuff(new Buff("RAWR!", CoreStats(5), 5, false));
                Assert::IsNotNull(potion->getBuff());
                Assert::AreEqual(std::string("Super Potion"), potion->name);
                Assert::AreEqual(10, (int)potion->healAmount);
                Assert::AreEqual(1, (int)potion->quantity);
                Assert::AreEqual(5, (int)potion->getBuff()->duration);
                Assert::AreEqual(std::string("RAWR!"), potion->getBuff()->name);
                Assert::AreEqual(5, (int)potion->getBuff()->buffedStats.strength);
                Assert::AreEqual(5, (int)potion->getBuff()->buffedStats.intellect);
                Assert::AreEqual(5, (int)potion->getBuff()->buffedStats.agility);
                Assert::AreEqual(5, (int)potion->getBuff()->buffedStats.armor);
                Assert::AreEqual(5, (int)potion->getBuff()->buffedStats.elementRes);
                Assert::IsFalse(potion->getBuff()->isDebuff);

                // modify to Poison Potion
                potion->name = "Poison Potion";
                potion->healAmount = 0;
                potion->quantity = 1;
                potion->setBuff(new Buff("Poisoned", CoreStats(2), 3, true));  // -2 to all stats for 3 rounds
                Assert::IsNotNull(potion->getBuff());
                Assert::AreEqual(std::string("Poison Potion"), potion->name);
                Assert::AreEqual(0, (int)potion->healAmount);
                Assert::AreEqual(1, (int)potion->quantity);
                Assert::AreEqual(3, (int)potion->getBuff()->duration);
                Assert::AreEqual(std::string("Poisoned"), potion->getBuff()->name);
                Assert::AreEqual(2, (int)potion->getBuff()->buffedStats.strength);
                Assert::AreEqual(2, (int)potion->getBuff()->buffedStats.intellect);
                Assert::AreEqual(2, (int)potion->getBuff()->buffedStats.agility);
                Assert::AreEqual(2, (int)potion->getBuff()->buffedStats.armor);
                Assert::AreEqual(2, (int)potion->getBuff()->buffedStats.elementRes);
                Assert::IsTrue(potion->getBuff()->isDebuff);
            }

            ItemManager::deleteItem(random_potion);
            Assert::IsNull(random_potion);
        }
		
		//TEST_METHOD(WarriorTest)
		//{
		//	PlayerCharacter p1(std::make_unique<Warrior>());
		//	Assert::AreEqual((int)Warrior::BASEHP, (int)p1.getMaxHP());

		//	p1.gainEXP(100u);
		//	Assert::AreEqual(2, (int)p1.getLevel());
		//	Assert::AreEqual(std::string("PowerAttack"), p1.getAbilityList().front().name_);
		//	Assert::AreEqual((int)Warrior::BASEHP + (int)((Warrior::BASEHP / 2.f)), (int)p1.getMaxHP());
		//}

  //      TEST_METHOD(Player_Cleric) {
  //          PlayerCharacter p1(std::make_unique<Cleric>());
  //          Assert::AreEqual((int)Cleric::BASEHP, (int)p1.getMaxHP());
  //          Assert::AreEqual((int)Cleric::BASEAP, (int)p1.getMaxAP());
  //          Assert::AreEqual(std::string("Heal"), p1.getAbilityList().front().name_);

  //          p1.gainEXP(100u);
  //          // level 2
  //          Assert::AreEqual(2, (int)p1.getLevel());
  //          Assert::AreEqual(std::string("Smite"), p1.getAbilityList()[1].name_);
  //          Assert::AreEqual((int)(Cleric::BASEHP + (Cleric::BASEHP / 2.f)), (int)p1.getMaxHP());
  //          Assert::AreEqual((int)(Cleric::BASEAP + (Cleric::BASEAP / 2.f)), (int)p1.getMaxAP());
  //      }
  //      TEST_METHOD(Player_Rogue) {
  //          PlayerCharacter p1(std::make_unique<Rogue>());
  //          Assert::AreEqual((int)Rogue::BASEHP, (int)p1.getMaxHP());

  //          p1.gainEXP(100u);
  //          // level 2
  //          Assert::AreEqual(2, (int)p1.getLevel());
  //          Assert::AreEqual(std::string("PreciseAttack"), p1.getAbilityList().front().name_);
  //          Assert::AreEqual((int)(Rogue::BASEHP + (Rogue::BASEHP / 2.f)), (int)p1.getMaxHP());
  //      }
  //      TEST_METHOD(Player_Warrior) {
  //          PlayerCharacter p1(std::make_unique<Warrior>());
  //          Assert::AreEqual((int)Warrior::BASEHP, (int)p1.getMaxHP());

  //          p1.gainEXP(100u);
  //          // level 2
  //          Assert::AreEqual(2, (int)p1.getLevel());
  //          Assert::AreEqual(std::string("PowerAttack"), p1.getAbilityList().front().name_);
  //          Assert::AreEqual((int)(Warrior::BASEHP + (Warrior::BASEHP / 2.f)), (int)p1.getMaxHP());
  //      }
  //      TEST_METHOD(Player_Wizard) {
  //          PlayerCharacter p1(std::make_unique<Wizard>());
  //          Assert::AreEqual((int)Wizard::BASEHP, (int)p1.getMaxHP());
  //          Assert::AreEqual((int)Wizard::BASEAP, (int)p1.getMaxAP());
  //          Assert::AreEqual(std::string("Firebolt"), p1.getAbilityList().front().name_);

  //          p1.gainEXP(100u);
  //          // level 2
  //          Assert::AreEqual(2, (int)p1.getLevel());
  //          Assert::AreEqual(std::string("IceBolt"), p1.getAbilityList()[1].name_);
  //          Assert::AreEqual((int)(Wizard::BASEHP + (Wizard::BASEHP / 2.f)), (int)p1.getMaxHP());
  //          Assert::AreEqual((int)(Wizard::BASEAP + (Wizard::BASEAP / 2.f)), (int)p1.getMaxAP());
  //      }
  //      TEST_METHOD(Equip_Player) {
  //          // a player to equip
  //          PlayerCharacter rogue(std::make_unique<Rogue>());

  //          // equip a full suite of items
  //          Assert::IsTrue(ItemManager::equip(ItemManager::createWeapon("Dagger", CoreStats(), WeaponSlot::MELEE, 1, 3, false), &rogue));
  //          Assert::IsTrue(ItemManager::equip(ItemManager::createWeapon("Bow", CoreStats(), WeaponSlot::RANGED, 1, 3, false), &rogue));
  //          Assert::IsTrue(ItemManager::equip(ItemManager::createArmor("Helm", CoreStats(0, 0, 0, 1, 1), ArmorSlot::HEAD), &rogue));
  //          Assert::IsTrue(ItemManager::equip(ItemManager::createArmor("Armor", CoreStats(0, 0, 0, 1, 1), ArmorSlot::CHEST), &rogue));
  //          Assert::IsTrue(ItemManager::equip(ItemManager::createArmor("Pants", CoreStats(0, 0, 0, 1, 1), ArmorSlot::LEGS), &rogue));
  //          Assert::IsTrue(ItemManager::equip(ItemManager::createArmor("Shoes", CoreStats(0, 0, 0, 1, 1), ArmorSlot::FEET), &rogue));
  //          Assert::IsTrue(ItemManager::equip(ItemManager::createArmor("Gloves", CoreStats(0, 0, 0, 1, 1), ArmorSlot::HANDS), &rogue));
  //          Assert::IsTrue(ItemManager::equip(ItemManager::createArmor("Silver Ring", CoreStats(1), ArmorSlot::RING1), &rogue));
  //          Assert::IsTrue(ItemManager::equip(ItemManager::createArmor("Gold Ring", CoreStats(2), ArmorSlot::RING2), &rogue));
  //          Assert::IsTrue(ItemManager::equip(ItemManager::createArmor("Necklace", CoreStats(0, 0, 0, 1, 1), ArmorSlot::NECK), &rogue));

  //          // check slots
  //          Assert::IsNotNull(rogue.getEquippedWeaponAt((unsigned long long)WeaponSlot::MELEE));
  //          Assert::IsNotNull(rogue.getEquippedWeaponAt((unsigned long long)WeaponSlot::RANGED));
  //          Assert::IsNotNull(rogue.getEquippedArmorAt((unsigned long long)ArmorSlot::HEAD));
  //          Assert::IsNotNull(rogue.getEquippedArmorAt((unsigned long long)ArmorSlot::CHEST));
  //          Assert::IsNotNull(rogue.getEquippedArmorAt((unsigned long long)ArmorSlot::LEGS));
  //          Assert::IsNotNull(rogue.getEquippedArmorAt((unsigned long long)ArmorSlot::FEET));
  //          Assert::IsNotNull(rogue.getEquippedArmorAt((unsigned long long)ArmorSlot::HANDS));
  //          Assert::IsNotNull(rogue.getEquippedArmorAt((unsigned long long)ArmorSlot::RING1));
  //          Assert::IsNotNull(rogue.getEquippedArmorAt((unsigned long long)ArmorSlot::RING2));
  //          Assert::IsNotNull(rogue.getEquippedArmorAt((unsigned long long)ArmorSlot::NECK));

  //          // equipping a potion should fail and go into inventory
  //          Assert::IsFalse(ItemManager::equip(ItemManager::createPotion("ArmorPot", 0, 1, new Buff("ArmorBuff", CoreStats(0, 0, 0, 3, 0), 5)), &rogue));

  //          auto backpack = rogue.getBackpackList();
  //          Assert::AreEqual(1, (int)backpack.size());

  //      }

  //      TEST_METHOD(Enemy_Monster) {
  //          Monster monster1(10, 2, 4);
  //          Assert::AreEqual(10, (int)monster1.HP.getMax());
  //          Assert::AreEqual(2, (int)monster1.getMinDamage());
  //          Assert::AreEqual(4, (int)monster1.getMaxDamage());

  //          int damage_rando = 0;
  //          for (int i = 0; i < 10; ++i) {
  //              damage_rando = monster1.attack();
  //              Assert::IsTrue((damage_rando >= (int)monster1.getMinDamage()));
  //              Assert::IsTrue((damage_rando <= (int)monster1.getMaxDamage()));
  //          }

  //      }

  //      TEST_METHOD(Simple_Battle) {
  //          PlayerCharacter rogue(std::make_unique<Rogue>());
  //          Assert::IsTrue(ItemManager::equip(ItemManager::createWeapon("Dagger", CoreStats(), WeaponSlot::MELEE, 1, 3, false), &rogue));

  //          Monster monster1(10, 2, 4);

  //          while (monster1.HP.getCurrent() > 0 && rogue.getCurrentHP() > 0) {
  //              monster1.HP.reduceCurrent(rogue.meleeAttack());
  //              rogue.takeDamage(monster1.attack());
  //          }

  //          Assert::IsTrue((monster1.HP.getCurrent() == 0) || (rogue.getCurrentHP() == 0));

  //      }
	};
}
