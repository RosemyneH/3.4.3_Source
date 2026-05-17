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

#include "DB2Stores.h"
#include "GameTables.h"
#include "ItemTemplate.h"
#include "Player.h"
#include "World.h"

int32 const SocketColorToGemTypeMask[6] =
{
    0,
    SOCKET_COLOR_META,
    SOCKET_COLOR_RED,
    SOCKET_COLOR_YELLOW,
    SOCKET_COLOR_BLUE,
    SOCKET_COLOR_PRISMATIC,
};

char const* ItemTemplate::GetName(LocaleConstant locale) const
{
    if (!strlen(ExtendedData->Display[locale]))
        return GetDefaultLocaleName();

    return ExtendedData->Display[locale];
}

bool ItemTemplate::HasSignature() const
{
    return GetMaxStackSize() == 1 &&
        GetClass() != ITEM_CLASS_CONSUMABLE &&
        GetClass() != ITEM_CLASS_QUEST &&
        !HasFlag(ITEM_FLAG_NO_CREATOR) &&
        GetId() != 6948; /*Hearthstone*/
}

bool ItemTemplate::CanChangeEquipStateInCombat() const
{
    switch (GetInventoryType())
    {
        case INVTYPE_RELIC:
        case INVTYPE_SHIELD:
        case INVTYPE_HOLDABLE:
            return true;
        default:
            break;
    }

    switch (GetClass())
    {
        case ITEM_CLASS_WEAPON:
        case ITEM_CLASS_PROJECTILE:
            return true;
    }

    return false;
}

uint32 ItemTemplate::GetSkill() const
{
    static uint32 const itemWeaponSkills[MAX_ITEM_SUBCLASS_WEAPON] =
    {
        SKILL_AXES,             SKILL_TWO_HANDED_AXES, SKILL_BOWS,          SKILL_GUNS,              SKILL_MACES,
        SKILL_TWO_HANDED_MACES, SKILL_POLEARMS,        SKILL_SWORDS,        SKILL_TWO_HANDED_SWORDS, 0,
        SKILL_STAVES,           0,                     0,                   SKILL_FIST_WEAPONS,      0,
        SKILL_DAGGERS,          SKILL_THROWN,          SKILL_ASSASSINATION, SKILL_CROSSBOWS,         SKILL_WANDS,
        SKILL_FISHING
    };

    static uint32 const itemArmorSkills[MAX_ITEM_SUBCLASS_ARMOR] =
    {
        0, SKILL_CLOTH, SKILL_LEATHER, SKILL_MAIL, SKILL_PLATE_MAIL, 0, SKILL_SHIELD, 0, 0, 0, 0
    };

    switch (GetClass())
    {
        case ITEM_CLASS_WEAPON:
            if (GetSubClass() >= MAX_ITEM_SUBCLASS_WEAPON)
                return 0;
            else
                return itemWeaponSkills[GetSubClass()];
        case ITEM_CLASS_ARMOR:
            if (GetSubClass() >= MAX_ITEM_SUBCLASS_ARMOR)
                return 0;
            else
                return itemArmorSkills[GetSubClass()];
        default:
            return 0;
    }
}

char const* ItemTemplate::GetDefaultLocaleName() const
{
    return ExtendedData->Display[sWorld->GetDefaultDbcLocale()];
}

float ItemTemplate::GetDPS() const
{
    const uint32 Delay = GetDelay();

    if (!Delay)
        return 0.f;

    float temp = 0.f;
    for (uint8 i = 0; i < MAX_ITEM_PROTO_DAMAGES; ++i)
        temp += BasicData->MinDamage[i] + BasicData->MaxDamage[i];

    return temp * 500.f / Delay;
}

bool ItemTemplate::IsUsableByLootSpecialization(Player const* player, bool alwaysAllowBoundToAccount) const
{
    if (HasFlag(ITEM_FLAG_IS_BOUND_TO_ACCOUNT) && alwaysAllowBoundToAccount)
        return true;

    uint32 spec = player->GetLootSpecId();
    if (!spec)
        spec = AsUnderlyingType(player->GetPrimarySpecialization());
    if (!spec)
        spec = player->GetDefaultSpecId();

    ChrSpecializationEntry const* chrSpecialization = sChrSpecializationStore.LookupEntry(spec);
    if (!chrSpecialization)
        return false;

    std::size_t levelIndex = 0;
    if (player->GetLevel() >= 110)
        levelIndex = 2;
    else if (player->GetLevel() > 40)
        levelIndex = 1;

    return Specializations[levelIndex].test(CalculateItemSpecBit(chrSpecialization));
}

std::size_t ItemTemplate::CalculateItemSpecBit(ChrSpecializationEntry const* spec)
{
    return (spec->ClassID - 1) * MAX_SPECIALIZATIONS + spec->OrderIndex;
}

int16 ItemTemplate::GetShieldBlockValue(uint32 itemLevel) const
{
    GtShieldBlockRegularEntry const* blockEntry = sShieldBlockRegularGameTable.GetRow(itemLevel);
    return static_cast<int16>(GetShieldBlockRegularColumnForQuality(blockEntry, static_cast<ItemQualities>(GetQuality())));
}

int32 ItemTemplate::getFeralBonus(int32 extraDPS /*= 0*/) const
{
    constexpr uint32 feralApEnabledInventoryTypeMaks = 1 << INVTYPE_WEAPON | 1 << INVTYPE_2HWEAPON | 1 << INVTYPE_WEAPONMAINHAND | 1 << INVTYPE_WEAPONOFFHAND;

    // 0x02A5F3 - is mask for Melee weapon from ItemSubClassMask.dbc
    if (BasicData->ClassID == ITEM_CLASS_WEAPON && (1 << BasicData->InventoryType) & feralApEnabledInventoryTypeMaks)
    {
        int32 bonus = int32((extraDPS + GetDPS()) * 14.0f) - 767;
        if (bonus < 0)
            return 0;
        return bonus;
    }

    return 0;
}
