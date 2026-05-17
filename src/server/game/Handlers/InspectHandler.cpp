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

#include "WorldSession.h"
#include "AchievementMgr.h"
#include "Guild.h"
#include "GuildMgr.h"
#include "InspectPackets.h"
#include "Log.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "World.h"
#include "ArenaTeamMgr.h"

void WorldSession::HandleInspectOpcode(WorldPackets::Inspect::Inspect& inspect)
{
    Player* player = ObjectAccessor::GetPlayer(*_player, inspect.Target);
    if (!player)
    {
        TC_LOG_DEBUG("network", "WorldSession::HandleInspectOpcode: Target {} not found.", inspect.Target.ToString());
        return;
    }

    TC_LOG_DEBUG("network", "WorldSession::HandleInspectOpcode: Target {}.", inspect.Target.ToString());

    if (!GetPlayer()->IsWithinDistInMap(player, INSPECT_DISTANCE, false))
        return;

    if (GetPlayer()->IsValidAttackTarget(player))
        return;

    WorldPackets::Inspect::InspectResult inspectResult;
    inspectResult.DisplayInfo.Initialize(player);

    if (GetPlayer()->CanBeGameMaster() || sWorld->getIntConfig(CONFIG_TALENTS_INSPECTING) + (GetPlayer()->GetEffectiveTeam() == player->GetEffectiveTeam()) > 1)
    {
        /*
        PlayerTalentMap const* talents = player->GetTalentMap(player->GetActiveTalentGroup());
        for (PlayerTalentMap::value_type const& v : *talents)
            if (v.second != PLAYERSPELL_REMOVED)
                inspectResult.Talents.push_back(v.first);
        */

        for (uint8 i = 0; i < (1 + player->GetBonusTalentGroupCount()); ++i)
        {
            PlayerTalentMap const& talentMap = player->GetPlayerTalentMap(i);

            uint8 count = 0;
            for (auto const& pair : talentMap)
            {
                if (pair.second.State == PLAYERSPELL_REMOVED)
                    continue;

                TalentEntry const* talentInfo = sTalentStore.LookupEntry(pair.first);
                if (!talentInfo)
                    continue;

                SpellInfo const* spellEntry = sSpellMgr->GetSpellInfo(talentInfo->SpellRank[pair.second.Rank], DIFFICULTY_NONE);
                if (!spellEntry)
                    continue;

                inspectResult.Talents[count].Id = pair.first;
                inspectResult.Talents[count].Rank = pair.second.Rank;
                ++count;
            }
        }
/*
        inspectResult.TalentTraits.Level = player->GetLevel();
        inspectResult.TalentTraits.ChrSpecializationID = AsUnderlyingType(player->GetPrimarySpecialization());
        if (UF::TraitConfig const* traitConfig = player->GetTraitConfig(player->m_activePlayerData->ActiveCombatTraitConfigID))
            inspectResult.TalentTraits.Config = WorldPackets::Traits::TraitConfig(*traitConfig);*/
    }

    /*if (Guild* guild = sGuildMgr->GetGuildById(player->GetGuildId()))
    {
        inspectResult.GuildData.emplace();
        inspectResult.GuildData->GuildGUID = guild->GetGUID();
        inspectResult.GuildData->NumGuildMembers = guild->GetMembersCount();
        inspectResult.GuildData->AchievementPoints = guild->GetAchievementMgr().GetAchievementPoints();
    }*/

    inspectResult.ItemLevel = int32(player->GetAverageItemLevel());
    inspectResult.LifetimeMaxRank = player->m_activePlayerData->LifetimeMaxRank;
    inspectResult.TodayHK = player->m_activePlayerData->TodayHonorableKills;
    inspectResult.YesterdayHK = player->m_activePlayerData->YesterdayHonorableKills;
    inspectResult.LifetimeHK = player->m_activePlayerData->LifetimeHonorableKills;
    inspectResult.HonorLevel = player->m_playerData->HonorLevel;

    SendPacket(inspectResult.Write());
}

void WorldSession::HandleQueryInspectAchievements(WorldPackets::Inspect::QueryInspectAchievements& inspect)
{
    Player* player = ObjectAccessor::GetPlayer(*_player, inspect.Guid);
    if (!player)
    {
        TC_LOG_DEBUG("network", "WorldSession::HandleQueryInspectAchievements: [{}] inspected unknown Player [{}]", GetPlayer()->GetGUID().ToString(), inspect.Guid.ToString());
        return;
    }

    TC_LOG_DEBUG("network", "WorldSession::HandleQueryInspectAchievements: [{}] inspected Player [{}]", GetPlayer()->GetGUID().ToString(), inspect.Guid.ToString());

    if (!GetPlayer()->IsWithinDistInMap(player, INSPECT_DISTANCE, false))
        return;

    if (GetPlayer()->IsValidAttackTarget(player))
        return;

    player->SendRespondInspectAchievements(GetPlayer());
}

void WorldSession::HandleInspectPvPOpcode(WorldPackets::Inspect::InspectPvP& inspect)
{
    Player* player = ObjectAccessor::GetPlayer(*_player, inspect.Target);
    if (!player)
    {
        TC_LOG_DEBUG("network", "WorldSession::HandleInspectPvPOpcode: [{}] inspected unknown Player [{}]", GetPlayer()->GetGUID().ToString(), inspect.Target.ToString());
        return;
    }

    const auto& it = sArenaTeamMgr->GetPersonalArenaTeams().find(player->GetGUID());
    if (it == sArenaTeamMgr->GetPersonalArenaTeams().end())
        return;

    WorldPackets::Inspect::InspectPvPResult result;
    result.Target = inspect.Target;

    for (uint8 i = 0; i != player->GetArenaTeamSize(); ++i)
    {
        WorldPackets::Inspect::PVPBracketData data;
        data.Bracket          = i;
        data.Rating           = it->second.at(i).PersonalRating;
        data.Rank             = it->second.at(i).Rank;
        data.WeeklyPlayed     = it->second.at(i).WeekGames;
        data.WeeklyWon        = it->second.at(i).WeekWins;
        data.SeasonPlayed     = it->second.at(i).SeasonGames;
        data.SeasonWon        = it->second.at(i).SeasonWins;
        data.WeeklyBestRating = it->second.at(i).PersonalRating;
        data.SeasonBestRating = it->second.at(i).PersonalRating;
        data.UnknownRating    = it->second.at(i).PersonalRating; // from sniffs this always matched seasons best rating
        data.PvpTierID        = 0;
        data.Disqualified     = false;

        result.Bracket.emplace_back(std::move(data));
    }

    // 3.4.3 client sends an empty bracket with id 4
    WorldPackets::Inspect::PVPBracketData data;
    data.Bracket = 4;
    result.Bracket.emplace_back(std::move(data));

    SendPacket(result.Write());
}

void WorldSession::HandleRequestHonorStatsOpcode(WorldPackets::Inspect::RequestHonorStats& inspect)
{
    Player* player = ObjectAccessor::GetPlayer(*_player, inspect.Target);
    if (!player)
    {
        TC_LOG_DEBUG("network", "WorldSession::HandleRequestHonorStatsOpcode: [{}] inspected unknown Player [{}]", GetPlayer()->GetGUID().ToString(), inspect.Target.ToString());
        return;
    }

    if (!GetPlayer()->IsWithinDistInMap(player, INSPECT_DISTANCE, false))
        return;

    if (GetPlayer()->IsValidAttackTarget(player))
        return;

    WorldPackets::Inspect::InspectHonorStatsResult result;
    result.Target = inspect.Target;
    
    result.LifetimeMaxRank = player->m_activePlayerData->LifetimeMaxRank;
    
    result.TodayHK = player->m_activePlayerData->TodayHonorableKills;
    result.TodayDK = player->m_activePlayerData->TodayDishonorableKills;
    
    result.YesterdayHK = player->m_activePlayerData->YesterdayHonorableKills;
    result.YesterdayDK = player->m_activePlayerData->YesterdayDishonorableKills;
    
    result.LastWeekHK = player->m_activePlayerData->LastWeekHonorableKills;
    result.LastWeekDK = player->m_activePlayerData->LastWeekDishonorableKills;
    
    result.ThisWeekHK = player->m_activePlayerData->ThisWeekHonorableKills;
    result.ThisWeekDK = player->m_activePlayerData->ThisWeekDishonorableKills;

    result.LifeTimeHK = player->m_activePlayerData->LifetimeHonorableKills;
    result.LifeTimeDK = player->m_activePlayerData->LifetimeDishonorableKills;
    
    result.YesterdayHonor = player->m_activePlayerData->YesterdayContribution;
    result.LastWeekHonor = player->m_activePlayerData->LastWeekContribution;
    result.ThisWeekHonor = player->m_activePlayerData->ThisWeekContribution;

    result.Standing = 0; // alistar: not sure what goes here

    result.RankProgress = player->m_activePlayerData->PvpRankProgress;

    SendPacket(result.Write());
}