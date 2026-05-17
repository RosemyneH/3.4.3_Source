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

#pragma once

#include "Packet.h"
#include "CharacterPackets.h"
#include "DBCEnums.h"
#include "ItemPacketsCommon.h"
#include "ObjectGuid.h"
#include "RaceMask.h"
#include "SharedDefines.h"

class Item;
class Player;

namespace WorldPackets
{
    namespace Inspect
    {
        class Inspect final : public ClientPacket
        {
        public:
            Inspect(WorldPacket&& packet) : ClientPacket(CMSG_INSPECT, std::move(packet)) { }

            void Read() override;

            ObjectGuid Target;
        };

        class InspectPvP final : public ClientPacket
        {
        public:
            InspectPvP(WorldPacket&& packet) : ClientPacket(CMSG_INSPECT_PVP, std::move(packet)) {}

            void Read() override;

            ObjectGuid Target;
        };

        class RequestHonorStats final : public ClientPacket
        {
        public:
            RequestHonorStats(WorldPacket&& packet) : ClientPacket(CMSG_REQUEST_HONOR_STATS, std::move(packet)) {}

            void Read() override;

            ObjectGuid Target;
        };

        struct InspectEnchantData
        {
            InspectEnchantData(uint32 id, uint8 index) : Id(id), Index(index) { }

            uint32 Id = 0;
            uint8 Index = 0;
        };

        struct AzeriteEssenceData
        {
            uint32 Index = 0;
            uint32 AzeriteEssenceID = 0;
            uint32 Rank = 0;
            bool SlotUnlocked = false;
        };

        struct InspectItemData
        {
            InspectItemData(::Item const* item, uint8 index);

            ObjectGuid CreatorGUID;
            Item::ItemInstance Item;
            uint8 Index = 0;
            bool Usable = false;
            std::vector<InspectEnchantData> Enchants;
            std::vector<Item::ItemGemData> Gems;
            std::vector<int32> AzeritePowers;
            std::vector<AzeriteEssenceData> AzeriteEssences;
        };

        struct PlayerModelDisplayInfo
        {
            ObjectGuid GUID;
            std::vector<InspectItemData> Items;
            std::string Name;
            int32 SpecializationID = 0;
            uint8 GenderID = GENDER_NONE;
            uint8 Race = RACE_NONE;
            uint8 ClassID = CLASS_NONE;
            std::vector<Character::ChrCustomizationChoice> Customizations;

            void Initialize(Player const* player);
        };

        struct InspectGuildData
        {
            ObjectGuid GuildGUID;
            int32 NumGuildMembers = 0;
            int32 AchievementPoints = 0;
        };

        struct PVPBracketData
        {
            int32 Rating = 0;
            int32 Rank = 0;
            int32 WeeklyPlayed = 0;
            int32 WeeklyWon = 0;
            int32 SeasonPlayed = 0;
            int32 SeasonWon = 0;
            int32 WeeklyBestRating = 0;
            int32 SeasonBestRating = 0;
            int32 PvpTierID = 0;
            int32 Unused1 = 0;
            int32 UnknownRating = 0;
            int32 Unused2 = 0;
            int32 Unused3 = 0;
            int32 Unused4 = 0;
            int32 Unused5 = 0;
            int32 Unused6 = 0;
            int32 Unused7 = 0;
            uint8 Bracket = 0;
            bool Disqualified = false;

            PVPBracketData() = default;

            PVPBracketData(PVPBracketData&& other) noexcept
                : Rating(std::exchange(other.Rating, 0)),
                Rank(std::exchange(other.Rank, 0)),
                WeeklyPlayed(std::exchange(other.WeeklyPlayed, 0)),
                WeeklyWon(std::exchange(other.WeeklyWon, 0)),
                SeasonPlayed(std::exchange(other.SeasonPlayed, 0)),
                SeasonWon(std::exchange(other.SeasonWon, 0)),
                WeeklyBestRating(std::exchange(other.WeeklyBestRating, 0)),
                SeasonBestRating(std::exchange(other.SeasonBestRating, 0)),
                PvpTierID(std::exchange(other.PvpTierID, 0)),
                Unused1(std::exchange(other.Unused1, 0)),
                UnknownRating(std::exchange(other.UnknownRating, 0)),
                Unused2(std::exchange(other.Unused2, 0)),
                Unused3(std::exchange(other.Unused3, 0)),
                Unused4(std::exchange(other.Unused4, 0)),
                Unused5(std::exchange(other.Unused5, 0)),
                Unused6(std::exchange(other.Unused6, 0)),
                Unused7(std::exchange(other.Unused7, 0)),
                Bracket(std::exchange(other.Bracket, 0)),
                Disqualified(std::exchange(other.Disqualified, false))
            {}
        };

        struct TalentData
        {
            uint32 Id  = 0;
            uint8 Rank = 0;
        };

        class InspectResult final : public ServerPacket
        {
        public:
            InspectResult() : ServerPacket(SMSG_INSPECT_RESULT, 1900) { }

            WorldPacket const* Write() override;

            PlayerModelDisplayInfo DisplayInfo;
            std::array<TalentData, 71> Talents { };
            Optional<int32> AzeriteLevel;
            int32 ItemLevel = 0;
            uint32 LifetimeHK = 0;
            uint32 HonorLevel = 0;
            uint16 TodayHK = 0;
            uint16 YesterdayHK = 0;
            uint8 LifetimeMaxRank = 0;
        };

        class InspectHonorStatsResult final : public ServerPacket
        {
        public:
            InspectHonorStatsResult() : ServerPacket(SMSG_INSPECT_HONOR_STATS, 12 + 42) {}

            WorldPacket const* Write() override;

            ObjectGuid Target;
            uint8 LifetimeMaxRank = 0;
            uint16 TodayHK = 0;
            uint16 TodayDK = 0;
            uint16 YesterdayHK = 0;
            uint16 YesterdayDK = 0;
            uint16 LastWeekHK = 0;
            uint16 LastWeekDK = 0;
            uint16 ThisWeekHK = 0;
            uint16 ThisWeekDK = 0;
            uint32 LifeTimeHK = 0;
            uint32 LifeTimeDK = 0;
            uint32 YesterdayHonor = 0;
            uint32 LastWeekHonor = 0;
            uint32 ThisWeekHonor = 0;
            uint32 Standing = 0;
            uint8 RankProgress = 0;
        };

        class InspectPvPResult final : public ServerPacket
        {
        public:
            InspectPvPResult() : ServerPacket(SMSG_INSPECT_PVP, 360) {}

            WorldPacket const* Write() override;

            ObjectGuid Target;
            uint8 Unk = 192; // from sniffs was always 192
            std::vector<PVPBracketData> Bracket;

        };

        class QueryInspectAchievements final : public ClientPacket
        {
        public:
            QueryInspectAchievements(WorldPacket&& packet) : ClientPacket(CMSG_QUERY_INSPECT_ACHIEVEMENTS, std::move(packet)) { }

            void Read() override;

            ObjectGuid Guid;
        };

        /// RespondInspectAchievements in AchievementPackets
    }
}
