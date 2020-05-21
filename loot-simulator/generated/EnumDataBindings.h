//-------------------------------------------------------------------------------
//
// EnumDataBindings.h
//

#pragma once

// This is a generated file! Any changes here will be lost!!!

#include <nlohmann/json/json.hpp>

namespace LootSimulator {

//===============================================================================

enum struct MonsterType : int32_t
{
    NONE = -1,
    GOBLIN = 0,
    SKELETON,
    DRAGON,
    ZOMBIE,
    NUM_TYPES
};

enum struct TreasureType : int32_t
{
    NONE = -1,
    REGENERATION_RING = 0,
    CURSED_RING,
    NOTHING,
    HEATER_SHIELD,
    KITE_SHIELD,
    GOLD_PILE,
    RUSTY_SWORD,
    GODLY_SWORD,
    SMALL_SHIELD,
    SHARP_SWORD,
    MAGIC_STAFF,
    APPLE,
    AMULET_OF_DESTRUCTION,
    NUM_TYPES
};

NLOHMANN_JSON_SERIALIZE_ENUM(MonsterType,
    {
        { MonsterType::NONE, "none" },
        { MonsterType::GOBLIN, "goblin" },
        { MonsterType::SKELETON, "skeleton" },
        { MonsterType::DRAGON, "dragon" },
        { MonsterType::ZOMBIE, "zombie" },
    });

NLOHMANN_JSON_SERIALIZE_ENUM(TreasureType,
    {
        { TreasureType::NONE, "none" },
        { TreasureType::REGENERATION_RING, "regenerationRing" },
        { TreasureType::CURSED_RING, "cursedRing" },
        { TreasureType::NOTHING, "nothing" },
        { TreasureType::HEATER_SHIELD, "heaterShield" },
        { TreasureType::KITE_SHIELD, "kiteShield" },
        { TreasureType::GOLD_PILE, "goldPile" },
        { TreasureType::RUSTY_SWORD, "rustySword" },
        { TreasureType::GODLY_SWORD, "godlySword" },
        { TreasureType::SMALL_SHIELD, "smallShield" },
        { TreasureType::SHARP_SWORD, "sharpSword" },
        { TreasureType::MAGIC_STAFF, "magicStaff" },
        { TreasureType::APPLE, "apple" },
        { TreasureType::AMULET_OF_DESTRUCTION, "amuletOfDestruction" },
    });

//===============================================================================

} // namespace LootSimulator
