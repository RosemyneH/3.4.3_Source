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

#ifndef TRINITYCORE_PET_DEFINES_H
#define TRINITYCORE_PET_DEFINES_H

#include "Define.h"
#include "Optional.h"
#include <array>
#include <string>
#include <vector>

enum ReactStates : uint8;

enum PetType : uint8
{
    SUMMON_PET              = 0,
    HUNTER_PET              = 1,
    MAX_PET_TYPE            = 4
};

#define MAX_PET_STABLES         4

// stored in character_pet.slot
enum PetSaveMode : int16
{
    PET_SAVE_AS_DELETED        = -1,                        // not saved in fact
    PET_SAVE_AS_CURRENT        =  0,                        // in current slot (with player)
    PET_SAVE_FIRST_STABLE_SLOT =  1,
    PET_SAVE_LAST_STABLE_SLOT  = MAX_PET_STABLES,          // last in DB stable slot index (including), all higher have same meaning as PET_SAVE_NOT_IN_SLOT
    PET_SAVE_NOT_IN_SLOT       = 100                       // for avoid conflict with stable size grow will use 100
};

enum HappinessState
{
    UNHAPPY = 1,
    CONTENT = 2,
    HAPPY   = 3
};

enum PetStableFlags : uint8
{
    PET_STABLE_ACTIVE   = 0x1,
    PET_STABLE_INACTIVE = 0x2
};

enum PetSpellState
{
    PETSPELL_UNCHANGED = 0,
    PETSPELL_CHANGED   = 1,
    PETSPELL_NEW       = 2,
    PETSPELL_REMOVED   = 3
};

enum PetSpellType
{
    PETSPELL_NORMAL = 0,
    PETSPELL_FAMILY = 1,
    PETSPELL_TALENT = 2
};

enum class PetActionFeedback : uint8
{
    None            = 0,
    Dead            = 1,
    NoTarget        = 2,
    InvalidTarget   = 3,
    NoPath          = 4
};

enum PetTalk
{
    PET_TALK_SPECIAL_SPELL  = 0,
    PET_TALK_ATTACK         = 1
};

enum PetTypes
{
    // Hunter
    NPC_TRAP_SNAKE_VENOMUS              = 19833,
    NPC_TRAP_SNAKE_VIPER                = 19921,
    // Warlock                          
    NPC_FELGUARD                        = 17252,
    NPC_VOIDWALKER                      = 1860,
    NPC_FELHUNTER                       = 417,
    NPC_SUCCUBUS                        = 1863,
    NPC_INCUBUS                         = 185317,
    NPC_IMP                             = 416,
    NPC_INFERNAL                        = 89,
    NPC_DOOMGUARD                       = 11859,
    // Mage                             
    NPC_WATER_ELEMENTAL                 = 510,
    NPC_WATER_ELEMENTAL_P               = 37994,
    NPC_MIRROR_IMAGE                    = 31216,
    // Druid                            
    NPC_TREANT                          = 1964,
    // Shaman                           
    NPC_EARTH_ELEMENTAL                 = 15352,
    NPC_FIRE_ELEMENTAL                  = 15438,
    // Death Knight                     
    NPC_GHOUL                           = 26125,
    NPC_EBON_GARGOYLE                   = 27829,
    NPC_BLOODWORM                       = 28017,
    NPC_ARMY_OF_DEAD_GHOUL              = 24207,
    NPC_DANCING_RUNE_WEAPON             = 27893,
    NPC_RISEN_ALLY                      = 30230,
    // Shaman                           
    NPC_FERAL_SPIRIT                    = 29264,
    // Priest                           
    NPC_SHADOWNFIEND                    = 19668
};

// Used by companions (minipets) and quest slot summons
constexpr float DEFAULT_FOLLOW_DISTANCE = 2.5f;
constexpr float DEFAULT_FOLLOW_DISTANCE_PET = 3.f;
constexpr float DEFAULT_FOLLOW_ANGLE = float(M_PI);

enum class PetTameResult : uint8
{
    Ok                      = 0,
    InvalidCreature         = 1,
    TooMany                 = 2,
    CreatureAlreadyOwned    = 3,
    NotTameable             = 4,
    AnotherSummonActive     = 5,
    UnitsCantTame           = 6,
    NoPetAvailable          = 7,
    InternalError           = 8,
    TooHighLevel            = 9,
    Dead                    = 10,
    NotDead                 = 11,
    CantControlExotic       = 12,
    InvalidSlot             = 13,
    EliteTooHighLevel       = 14
};

enum class StableResult : uint8
{
    NotEnoughMoney        = 1,                              // "you don't have enough money"
    InvalidSlot           = 3,                              // "That slot is locked"
    StableSuccess         = 8,                              // stable success
    UnstableSuccess       = 9,                              // unstable/swap success
    BuySlotSuccess        = 10,                             // buy slot success
    CantControlExotic     = 11,                             // "you are unable to control exotic creatures"
    InternalError         = 12,                             // "Internal pet error"
};

constexpr uint32 CALL_PET_SPELL_ID = 883;
constexpr uint32 PET_SUMMONING_DISORIENTATION = 32752;

class PetStable
{
public:
    struct PetInfo
    {
        PetInfo() { }

        std::string Name;
        std::string ActionBar;
        uint32 PetNumber = 0;
        uint32 CreatureId = 0;
        uint32 DisplayId = 0;
        uint32 Experience = 0;
        uint32 Health = 0;
        uint32 Mana = 0;
        uint32 Happiness = 0;
        uint32 LastSaveTime = 0;
        uint32 CreatedBySpellId = 0;
        uint8 Level = 0;
        ReactStates ReactState = ReactStates(0);
        PetType Type = MAX_PET_TYPE;
        bool WasRenamed = false;
    };

    Optional<PetInfo> CurrentPet;                                   // PET_SAVE_AS_CURRENT
    std::array<Optional<PetInfo>, MAX_PET_STABLES> StabledPets;     // PET_SAVE_FIRST_STABLE_SLOT - PET_SAVE_LAST_STABLE_SLOT
    uint32 MaxStabledPets = 0;
    std::vector<PetInfo> UnslottedPets;                             // PET_SAVE_NOT_IN_SLOT

    PetInfo const* GetUnslottedHunterPet() const
    {
        return UnslottedPets.size() == 1 && UnslottedPets[0].Type == HUNTER_PET ? &UnslottedPets[0] : nullptr;
    }
};

#endif
