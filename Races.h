#ifndef RACES_H
#define RACES_H

#include "PointWell.h"
#include <string>

enum class RaceID { HUMAN, ELF, DWARF, DARKELF };

class Race {
public:
	virtual ~Race() = default; // also missing a virtual destructor!
	virtual std::string getRaceName() const = 0;
	virtual RaceID getRaceID() const = 0;
	virtual welltype getBonusHP() const = 0;
	virtual welltype getBonusAP() const = 0;
	virtual stattype getBonusStr() const = 0;
	virtual stattype getBonusInt() const = 0;
	virtual stattype getBonusAgi() const = 0;
};

class Human final : public Race{
public:
	std::string getRaceName() const override { return "Human"; }
	RaceID getRaceID() const override { return RaceID::HUMAN; }
	welltype getBonusHP()  const override { return 2u; }
	welltype getBonusAP()  const override { return 0u; }
	stattype getBonusStr() const override { return 2u; }
	stattype getBonusInt() const override { return 0u; }
	stattype getBonusAgi() const override { return 1u; }
};

class Elf final : public Race {
public:
	std::string getRaceName() const override { return "Elf"; }
	RaceID getRaceID() const override { return RaceID::ELF; }
	welltype getBonusHP()  const override { return 0u; }
	welltype getBonusAP()  const override { return 0u; }
	stattype getBonusStr() const override { return 0u; }
	stattype getBonusInt() const override { return 3u; }
	stattype getBonusAgi() const override { return 2u; }
};

class Dwarf final : public Race {
public:
	std::string getRaceName() const override { return "Dwarf"; }
	RaceID getRaceID() const override { return RaceID::DWARF; }
	welltype getBonusHP()  const override { return 3u; }
	welltype getBonusAP()  const override { return 0u; }
	stattype getBonusStr() const override { return 1u; }
	stattype getBonusInt() const override { return 0u; }
	stattype getBonusAgi() const override { return 1u; }
};

class DarkElf final : public Race {
public:
	std::string getRaceName() const override { return "Dark Elf"; }
	RaceID getRaceID() const override { return RaceID::DARKELF; }
	welltype getBonusHP()  const override { return 0u; }
	welltype getBonusAP()  const override { return 0u; }
	stattype getBonusStr() const override { return 0u; }
	stattype getBonusInt() const override { return 2u; }
	stattype getBonusAgi() const override { return 3u; }
};

#endif // !RACES_H

