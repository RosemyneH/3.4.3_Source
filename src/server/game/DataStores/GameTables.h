/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GameTables_h__
#define GameTables_h__

#include "SharedDefines.h"
#include "Common.h"
#include <vector>

enum InventoryType : uint8;

// All Gt* DBC store data for 100 levels, some by 100 per class/race
#define GT_MAX_LEVEL    100
// gtOCTClassCombatRatingScalar.dbc stores data for 32 ratings, look at MAX_COMBAT_RATING for real used amount
#define GT_MAX_RATING   32

// alistar: TODO move to db?
const std::unordered_map<uint32, std::array<uint32, 4>> NonCombatPetInfo =
{
    { 2671, { 7937, 0, 0, 0 } },
    { 7380, { 5585, 0, 0, 0 } },
    { 7381, { 5555, 0, 0, 0 } },
    { 7382, { 5554, 0, 0, 0 } },
    { 7383, { 5448, 0, 0, 0 } },
    { 7384, { 5586, 0, 0, 0 } },
    { 7385, { 5556, 0, 0, 0 } },
    { 7386, { 9989, 0, 0, 0 } },
    { 7387, { 5207, 0, 0, 0 } },
    { 7388, { 6189, 0, 0, 0 } },
    { 7389, { 6190, 0, 0, 0 } },
    { 7390, { 6191, 0, 0, 0 } },
    { 7391, { 6192, 0, 0, 0 } },
    { 7392, { 304, 0, 0, 0 } },
    { 7393, { 304, 0, 0, 0 } },
    { 7394, { 5369, 0, 0, 0 } },
    { 7395, { 2177, 0, 0, 0 } },
    { 7543, { 6288, 0, 0, 0 } },
    { 7544, { 6290, 0, 0, 0 } },
    { 7545, { 6291, 0, 0, 0 } },
    { 7546, { 6292, 0, 0, 0 } },
    { 7547, { 6293, 0, 0, 0 } },
    { 7548, { 6294, 0, 0, 0 } },
    { 7549, { 6295, 0, 0, 0 } },
    { 7550, { 901, 0, 0, 0 } },
    { 7551, { 6296, 0, 0, 0 } },
    { 7552, { 6297, 0, 0, 0 } },
    { 7553, { 4615, 0, 0, 0 } },
    { 7554, { 6298, 0, 0, 0 } },
    { 7555, { 6299, 0, 0, 0 } },
    { 7556, { 6300, 0, 0, 0 } },
    { 7558, { 4626, 0, 0, 0 } },
    { 7559, { 6302, 0, 0, 0 } },
    { 7560, { 328, 0, 0, 0 } },
    { 7561, { 2955, 0, 0, 0 } },
    { 7562, { 2957, 0, 0, 0 } },
    { 7563, { 2956, 0, 0, 0 } },
    { 7565, { 1206, 0, 0, 0 } },
    { 7566, { 2954, 0, 0, 0 } },
    { 7567, { 6303, 0, 0, 0 } },
    { 7568, { 2954, 0, 0, 0 } },
    { 7569, { 1986, 0, 0, 0 } },
    { 7570, { 1824, 0, 0, 0 } },
    { 8376, { 7920, 0, 0, 0 } },
    { 9656, { 8909, 0, 0, 0 } },
    { 9657, { 8910, 0, 0, 0 } },
    { 9662, { 6294, 0, 0, 0 } },
    { 9936, { 9209, 0, 0, 0 } },
    { 10259, { 9563, 9572, 0, 0 } },
    { 10598, { 9997, 0, 0, 0 } },
    { 11236, { 9554, 0, 0, 0 } },
    { 11325, { 10990, 0, 0, 0 } },
    { 11326, { 10992, 0, 0, 0 } },
    { 11327, { 10993, 0, 0, 0 } },
    { 12419, { 901, 0, 0, 0 } },
    { 14421, { 1072, 0, 0, 0 } },
    { 14629, { 14657, 0, 0, 0 } },
    { 14630, { 14658, 0, 0, 0 } },
    { 14631, { 14659, 0, 0, 0 } },
    { 14632, { 14660, 0, 0, 0 } },
    { 14633, { 14661, 0, 0, 0 } },
    { 14755, { 14778, 0, 0, 0 } },
    { 14756, { 14779, 0, 0, 0 } },
    { 14878, { 14938, 0, 0, 0 } },
    { 15186, { 15369, 0, 0, 0 } },
    { 15356, { 15393, 0, 0, 0 } },
    { 15357, { 15397, 0, 0, 0 } },
    { 15358, { 15398, 0, 0, 0 } },
    { 15359, { 15396, 0, 0, 0 } },
    { 15360, { 15394, 0, 0, 0 } },
    { 15361, { 15395, 0, 0, 0 } },
    { 15429, { 15436, 0, 0, 0 } },
    { 15661, { 15595, 0, 0, 0 } },
    { 15698, { 15660, 0, 0, 0 } },
    { 15699, { 10269, 0, 0, 0 } },
    { 15705, { 15663, 0, 0, 0 } },
    { 15706, { 15904, 0, 0, 0 } },
    { 15710, { 15671, 0, 0, 0 } },
    { 16069, { 15984, 0, 0, 0 } },
    { 16085, { 15992, 0, 0, 0 } },
    { 16445, { 15398, 0, 0, 0 } },
    { 16456, { 16189, 0, 0, 0 } },
    { 16547, { 16259, 0, 0, 0 } },
    { 16548, { 16257, 0, 0, 0 } },
    { 16549, { 2176, 0, 0, 0 } },
    { 16701, { 16587, 0, 0, 0 } },
    { 17254, { 16942, 0, 0, 0 } },
    { 17255, { 16943, 0, 0, 0 } },
    { 17709, { 15905, 0, 0, 0 } },
    { 18381, { 17723, 0, 0, 0 } },
    { 18839, { 18269, 0, 0, 0 } },
    { 20245, { 9989, 0, 0, 0 } },
    { 20246, { 9990, 0, 0, 0 } },
    { 20247, { 5556, 0, 0, 0 } },
    { 20408, { 19600, 0, 0, 0 } },
    { 20472, { 4626, 0, 0, 0 } },
    { 21008, { 19985, 0, 0, 0 } },
    { 21009, { 19986, 0, 0, 0 } },
    { 21010, { 19987, 0, 0, 0 } },
    { 21018, { 19999, 0, 0, 0 } },
    { 21055, { 20026, 0, 0, 0 } },
    { 21056, { 20029, 0, 0, 0 } },
    { 21063, { 20037, 0, 0, 0 } },
    { 21064, { 20027, 0, 0, 0 } },
    { 21076, { 20042, 0, 0, 0 } },
    { 21266, { 20155, 20160, 0, 0 } },
    { 22328, { 15382, 0, 0, 0 } },
    { 22445, { 20996, 0, 0, 0 } },
    { 22943, { 22349, 0, 0, 0 } },
    { 23114, { 21304, 0, 0, 0 } },
    { 23198, { 21328, 0, 0, 0 } },
    { 23231, { 21381, 0, 0, 0 } },
    { 23234, { 21362, 0, 0, 0 } },
    { 23258, { 21382, 0, 0, 0 } },
    { 23266, { 21393, 0, 0, 0 } },
    { 23274, { 16633, 0, 0, 0 } },
    { 23439, { 21155, 0, 0, 0 } },
    { 23909, { 21900, 0, 0, 0 } },
    { 24388, { 22388, 0, 0, 0 } },
    { 24389, { 22389, 0, 0, 0 } },
    { 24480, { 22459, 0, 0, 0 } },
    { 24753, { 22629, 0, 0, 0 } },
    { 24968, { 22776, 0, 0, 0 } },
    { 25062, { 22855, 0, 0, 0 } },
    { 25109, { 22903, 0, 0, 0 } },
    { 25110, { 22966, 22990, 22991, 22992 } },
    { 25146, { 21304, 0, 0, 0 } },
    { 25147, { 22938, 0, 0, 0 } },
    { 25706, { 8409, 0, 0, 0 } },
    { 26050, { 23507, 0, 0, 0 } },
    { 26056, { 23506, 0, 0, 0 } },
    { 26119, { 23574, 0, 0, 0 } },
    { 26243, { 15923, 0, 0, 0 } },
    { 27017, { 26139, 0, 0, 0 } },
    { 27217, { 24393, 0, 0, 0 } },
    { 27346, { 24620, 0, 0, 0 } },
    { 27914, { 25002, 0, 0, 0 } },
    { 28267, { 25265, 0, 0, 0 } },
    { 28470, { 25457, 0, 0, 0 } },
    { 28513, { 4185, 0, 0, 0 } },
    { 28883, { 28456, 0, 0, 0 } },
    { 29089, { 25900, 0, 0, 0 } },
    { 29147, { 28089, 0, 0, 0 } },
    { 29726, { 26452, 0, 0, 0 } },
    { 30379, { 304, 0, 0, 0 } },
    { 31575, { 27627, 0, 0, 0 } },
    { 32589, { 28214, 0, 0, 0 } },
    { 32590, { 28215, 0, 0, 0 } },
    { 32591, { 28084, 0, 0, 0 } },
    { 32592, { 28217, 0, 0, 0 } },
    { 32595, { 28216, 0, 0, 0 } },
    { 32643, { 14273, 0, 0, 0 } },
    { 32791, { 6302, 0, 0, 0 } },
    { 32818, { 21774, 0, 0, 0 } },
    { 32841, { 16189, 0, 0, 0 } },
    { 32939, { 28397, 0, 0, 0 } },
    { 33188, { 28482, 0, 0, 0 } },
    { 33194, { 28489, 0, 0, 0 } },
    { 33197, { 4732, 0, 0, 0 } },
    { 33198, { 15470, 0, 0, 0 } },
    { 33199, { 14473, 0, 0, 0 } },
    { 33200, { 16205, 0, 0, 0 } },
    { 33205, { 28493, 0, 0, 0 } },
    { 33219, { 28502, 0, 0, 0 } },
    { 33226, { 28507, 0, 0, 0 } },
    { 33227, { 16910, 0, 0, 0 } },
    { 33238, { 28946, 0, 0, 0 } },
    { 33239, { 28948, 0, 0, 0 } },
    { 33274, { 28539, 0, 0, 0 } },
    { 33529, { 25384, 0, 0, 0 } },
    { 33530, { 25173, 0, 0, 0 } },
    { 33578, { 28734, 0, 0, 0 } },
    { 33810, { 115700, 115701, 115702, 0 } },
    { 34031, { 29060, 0, 0, 0 } },
    { 34320, { 29251, 0, 0, 0 } },
    { 34364, { 11709, 0, 0, 0 } },
    { 34587, { 29279, 0, 0, 0 } },
    { 34694, { 29348, 0, 0, 0 } },
    { 34724, { 29372, 0, 0, 0 } },
    { 34770, { 29404, 0, 0, 0 } },
    { 34930, { 29605, 0, 0, 0 } },
    { 35387, { 29802, 0, 0, 0 } },
    { 35394, { 29806, 0, 0, 0 } },
    { 35395, { 29807, 0, 0, 0 } },
    { 35396, { 29805, 0, 0, 0 } },
    { 35397, { 29810, 0, 0, 0 } },
    { 35398, { 29808, 0, 0, 0 } },
    { 35399, { 29809, 0, 0, 0 } },
    { 35400, { 29803, 0, 0, 0 } },
    { 35468, { 29819, 0, 0, 0 } },
    { 36482, { 30157, 0, 0, 0 } },
    { 36511, { 30409, 0, 0, 0 } },
    { 36607, { 30356, 0, 0, 0 } },
    { 36871, { 30462, 0, 0, 0 } },
    { 36908, { 30412, 0, 0, 0 } },
    { 36909, { 30413, 0, 0, 0 } },
    { 36910, { 30402, 0, 0, 0 } },
    { 36911, { 30414, 0, 0, 0 } },
    { 36979, { 30507, 0, 0, 0 } },
    { 37865, { 31174, 0, 0, 0 } },
    { 38374, { 31073, 0, 0, 0 } },
    { 40198, { 31722, 0, 0, 0 } },
    { 40295, { 22778, 0, 0, 0 } },
    { 40624, { 31956, 0, 0, 0 } },
    { 40703, { 32031, 0, 0, 0 } },
    { 42078, { 32670, 0, 0, 0 } },
    { 213605, { 115636, 0, 0, 0 } },
    { 181485, { 106231, 0, 0, 0 } },
    { 194870, { 108702, 0, 0, 0 } },
    { 196534, { 109224, 0, 0, 0 } },
    { 200900, { 112805, 0, 0, 0 } },
    { 211025, { 114285, 0, 0, 0 } },
    { 40721, { 28734, 0, 0, 0 } },
    { 222858 , { 119244, 0, 0, 0 } },
};

struct GtBarberShopCostBaseEntry
{
    float Cost = 0.0f;
};

struct GtBaseMPEntry
{
    float Rogue = 0.0f;
    float Druid = 0.0f;
    float Hunter = 0.0f;
    float Mage = 0.0f;
    float Paladin = 0.0f;
    float Priest = 0.0f;
    float Shaman = 0.0f;
    float Warlock = 0.0f;
    float Warrior = 0.0f;
    float DeathKnight = 0.0f;
    float Monk = 0.0f;
    float DemonHunter = 0.0f;
};

struct GtBattlePetXPEntry
{
    float Wins = 0.0f;
    float Xp = 0.0f;
};

struct GtCombatRatingsEntry
{
    float WeaponSkill = 0.0f;
    float DefenseSkill = 0.0f;
    float Dodge = 0.0f;
    float Parry = 0.0f;
    float Block = 0.0f;
    float HitMelee = 0.0f;
    float HitRanged = 0.0f;
    float HitSpell = 0.0f;
    float CritMelee = 0.0f;
    float CritRanged = 0.0f;
    float CritSpell = 0.0f;
    float HitTakenMelee = 0.0f;
    float HitTakenRanged = 0.0f;
    float HitTakenSpell = 0.0f;
    float CritTakenMelee = 0.0f;
    float CritTakenRanged = 0.0f;
    float CritTakenSpell = 0.0f;
    float HasteMelee = 0.0f;
    float HasteRanged = 0.0f;
    float HasteSpell = 0.0f;
    float Unknown0 = 0.0f;
    float Unknown1 = 0.0f;
    float Unknown2 = 0.0f;
    float Expertise = 0.0f;
    float ArmorPenetration = 0.0f;
    float Unknown5 = 0.0f;
    float Unknown6 = 0.0f;
    float Unknown7 = 0.0f;
    float Unknown8 = 0.0f;
    float Unknown9 = 0.0f;
    float Unknown10 = 0.0f;
    float Unknown11 = 0.0f;
};

struct GtCombatRatingsMultByILvl
{
    float ArmorMultiplier = 0.0f;
    float WeaponMultiplier = 0.0f;
    float TrinketMultiplier = 0.0f;
    float JewelryMultiplier = 0.0f;
};

struct GtHpPerStaEntry
{
    float Health = 0.0f;
};

struct GtItemSocketCostPerLevelEntry
{
    float SocketCost = 0.0f;
};

struct GtNpcManaCostScalerEntry
{
    float Scaler = 0.0f;
};

struct GtOCTRegenHPEntry
{
    float Warrior = 0.0f;
    float Paladin = 0.0f;
    float Hunter = 0.0f;
    float Rogue = 0.0f;
    float Priest = 0.0f;
    float DeathKnight = 0.0f;
    float Shaman = 0.0f;
    float Mage = 0.0f;
    float Warlock = 0.0f;
    float Monk = 0.0f;
    float Druid = 0.0f;
};

struct GtOCTRegenMPEntry
{
    float Warrior = 0.0f;
    float Paladin = 0.0f;
    float Hunter = 0.0f;
    float Rogue = 0.0f;
    float Priest = 0.0f;
    float DeathKnight = 0.0f;
    float Shaman = 0.0f;
    float Mage = 0.0f;
    float Warlock = 0.0f;
    float Monk = 0.0f;
    float Druid = 0.0f;
};

struct GtRegenHPPerSptEntry
{
    float Warrior = 0.0f;
    float Paladin = 0.0f;
    float Hunter = 0.0f;
    float Rogue = 0.0f;
    float Priest = 0.0f;
    float DeathKnight = 0.0f;
    float Shaman = 0.0f;
    float Mage = 0.0f;
    float Warlock = 0.0f;
    float Monk = 0.0f;
    float Druid = 0.0f;
};

struct GtRegenMPPerSptEntry
{
    float Warrior = 0.0f;
    float Paladin = 0.0f;
    float Hunter = 0.0f;
    float Rogue = 0.0f;
    float Priest = 0.0f;
    float DeathKnight = 0.0f;
    float Shaman = 0.0f;
    float Mage = 0.0f;
    float Warlock = 0.0f;
    float Monk = 0.0f;
    float Druid = 0.0f;
};

struct GtShieldBlockRegularEntry
{
    float Poor = 0.0f;
    float Standard = 0.0f;
    float Good = 0.0f;
    float Superior = 0.0f;
    float Epic = 0.0f;
    float Legendary = 0.0f;
    float Artifact = 0.0f;
    float ScalingStat = 0.0f;
};

struct GtSpellScalingEntry
{
    float Rogue = 0.0f;
    float Druid = 0.0f;
    float Hunter = 0.0f;
    float Mage = 0.0f;
    float Paladin = 0.0f;
    float Priest = 0.0f;
    float Shaman = 0.0f;
    float Warlock = 0.0f;
    float Warrior = 0.0f;
    float DeathKnight = 0.0f;
    float Monk = 0.0f;
    float DemonHunter = 0.0f;
    float Item = 0.0f;
    float Consumable = 0.0f;
    float Gem1 = 0.0f;
    float Gem2 = 0.0f;
    float Gem3 = 0.0f;
    float Health = 0.0f;
};

struct GtStaminaMultByILvl
{
    float ArmorMultiplier = 0.0f;
    float WeaponMultiplier = 0.0f;
    float TrinketMultiplier = 0.0f;
    float JewelryMultiplier = 0.0f;
};

struct GtChanceToMeleeCrit
{
    float Warrior = 0.0f;
    float Paladin = 0.0f;
    float Hunter = 0.0f;
    float Rogue = 0.0f;
    float Priest = 0.0f;
    float DeathKnight = 0.0f;
    float Shaman = 0.0f;
    float Mage = 0.0f;
    float Warlock = 0.0f;
    float Monk = 0.0f;
    float Druid = 0.0f;
};

struct GtChanceToMeleeCritBase
{
    float Warrior = 0.0f;
    float Paladin = 0.0f;
    float Hunter = 0.0f;
    float Rogue = 0.0f;
    float Priest = 0.0f;
    float DeathKnight = 0.0f;
    float Shaman = 0.0f;
    float Mage = 0.0f;
    float Warlock = 0.0f;
    float Monk = 0.0f;
    float Druid = 0.0f;
};

struct GtChanceToSpellCrit
{
    float Warrior = 0.0f;
    float Paladin = 0.0f;
    float Hunter = 0.0f;
    float Rogue = 0.0f;
    float Priest = 0.0f;
    float DeathKnight = 0.0f;
    float Shaman = 0.0f;
    float Mage = 0.0f;
    float Warlock = 0.0f;
    float Monk = 0.0f;
    float Druid = 0.0f;
};

struct GtChanceToSpellCritBase
{
    float Warrior = 0.0f;
    float Paladin = 0.0f;
    float Hunter = 0.0f;
    float Rogue = 0.0f;
    float Priest = 0.0f;
    float DeathKnight = 0.0f;
    float Shaman = 0.0f;
    float Mage = 0.0f;
    float Warlock = 0.0f;
    float Monk = 0.0f;
    float Druid = 0.0f;
};

template<class T>
class GameTable
{
public:
    T const* GetRow(uint32 row) const
    {
        if (row >= _data.size())
            return nullptr;

        return &_data[row];
    }

    std::size_t GetTableRowCount() const { return _data.size(); }

    void SetData(std::vector<T> data) { _data = std::move(data); }

private:
    std::vector<T> _data;
};

TC_GAME_API extern GameTable<GtBarberShopCostBaseEntry>             sBarberShopCostBaseGameTable;
TC_GAME_API extern GameTable<GtBaseMPEntry>                         sBaseMPGameTable;
TC_GAME_API extern GameTable<GtBattlePetXPEntry>                    sBattlePetXPGameTable;
TC_GAME_API extern GameTable<GtCombatRatingsEntry>                  sCombatRatingsGameTable;
TC_GAME_API extern GameTable<GtCombatRatingsMultByILvl>             sCombatRatingsMultByILvlGameTable;
TC_GAME_API extern GameTable<GtHpPerStaEntry>                       sHpPerStaGameTable;
TC_GAME_API extern GameTable<GtItemSocketCostPerLevelEntry>         sItemSocketCostPerLevelGameTable;
TC_GAME_API extern GameTable<GtNpcManaCostScalerEntry>              sNpcManaCostScalerGameTable;
TC_GAME_API extern GameTable<GtOCTRegenHPEntry>                     sOCTRegenHPGameTable;
TC_GAME_API extern GameTable<GtOCTRegenMPEntry>                     sOCTRegenMPGameTable;
TC_GAME_API extern GameTable<GtRegenHPPerSptEntry>                  sRegenHPPerSptGameTable;
TC_GAME_API extern GameTable<GtRegenMPPerSptEntry>                  sRegenMPPerSptGameTable;
TC_GAME_API extern GameTable<GtShieldBlockRegularEntry>             sShieldBlockRegularGameTable;
TC_GAME_API extern GameTable<GtSpellScalingEntry>                   sSpellScalingGameTable;
TC_GAME_API extern GameTable<GtChanceToMeleeCrit>                   sChanceToMeleeCrit;
TC_GAME_API extern GameTable<GtChanceToMeleeCritBase>               sChanceToMeleeCritBase;
TC_GAME_API extern GameTable<GtChanceToSpellCrit>                   sChanceToSpellCrit;
TC_GAME_API extern GameTable<GtChanceToSpellCritBase>               sChanceToSpellCritBase;

TC_GAME_API void LoadGameTables(std::string const& dataPath);

template<class T>
inline float GetGameTableColumnForClass(T const* row, int32 class_)
{
    switch (class_)
    {
        case CLASS_WARRIOR:
            return row->Warrior;
        case CLASS_PALADIN:
            return row->Paladin;
        case CLASS_HUNTER:
            return row->Hunter;
        case CLASS_ROGUE:
            return row->Rogue;
        case CLASS_PRIEST:
            return row->Priest;
        case CLASS_DEATH_KNIGHT:
            return row->DeathKnight;
        case CLASS_SHAMAN:
            return row->Shaman;
        case CLASS_MAGE:
            return row->Mage;
        case CLASS_WARLOCK:
            return row->Warlock;
        case CLASS_DRUID:
            return row->Druid;
        default:
            break;
    }

    return 0.0f;
}

template<class T>
inline float GetRegenGameTableColumnForClass(T const* row, int32 class_)
{
    switch (class_)
    {
    case CLASS_WARRIOR:
        return row->Warrior;
    case CLASS_PALADIN:
        return row->Paladin;
    case CLASS_HUNTER:
        return row->Hunter;
    case CLASS_ROGUE:
        return row->Rogue;
    case CLASS_PRIEST:
        return row->Priest;
    case CLASS_DEATH_KNIGHT:
        return row->DeathKnight;
    case CLASS_SHAMAN:
        return row->Shaman;
    case CLASS_MAGE:
        return row->Mage;
    case CLASS_WARLOCK:
        return row->Warlock;
    case CLASS_MONK:
        return row->Monk;
    case CLASS_DRUID:
        return row->Druid;
    default:
        break;
    }

    return 0.0f;
}

inline float GetSpellScalingColumnForClass(GtSpellScalingEntry const* row, int32 class_)
{
    switch (class_)
    {
        case CLASS_WARRIOR:
            return row->Warrior;
        case CLASS_PALADIN:
            return row->Paladin;
        case CLASS_HUNTER:
            return row->Hunter;
        case CLASS_ROGUE:
            return row->Rogue;
        case CLASS_PRIEST:
            return row->Priest;
        case CLASS_DEATH_KNIGHT:
            return row->DeathKnight;
        case CLASS_SHAMAN:
            return row->Shaman;
        case CLASS_MAGE:
            return row->Mage;
        case CLASS_WARLOCK:
            return row->Warlock;
        case CLASS_DRUID:
            return row->Druid;
        case -1:
        case -7:
            return row->Item;
        case -2:
            return row->Consumable;
        case -3:
            return row->Gem1;
        case -4:
            return row->Gem2;
        case -5:
            return row->Gem3;
        case -6:
            return row->Health;
        default:
            break;
    }

    return 0.0f;
}

inline float GetBattlePetXPPerLevel(GtBattlePetXPEntry const* row)
{
    return row->Wins * row->Xp;
}

inline float GetShieldBlockRegularColumnForQuality(GtShieldBlockRegularEntry const* row, ItemQualities quality)
{
    switch (quality)
    {
        case ITEM_QUALITY_POOR:
            return row->Poor;
        case ITEM_QUALITY_NORMAL:
            return row->Standard;
        case ITEM_QUALITY_UNCOMMON:
            return row->Good;
        case ITEM_QUALITY_RARE:
            return row->Superior;
        case ITEM_QUALITY_EPIC:
            return row->Epic;
        case ITEM_QUALITY_LEGENDARY:
            return row->Legendary;
        case ITEM_QUALITY_ARTIFACT:
            return row->Artifact;
        case ITEM_QUALITY_HEIRLOOM:
            return row->ScalingStat;
        default:
            break;
    }

    return 0.0f;
}

#endif // GameTables_h__
