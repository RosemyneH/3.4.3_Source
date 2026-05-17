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

#include "ArenaTeamMgr.h"
#include "DatabaseEnv.h"
#include "Define.h"
#include "Log.h"
#include "Util.h"
#include "World.h"

ArenaTeamMgr::ArenaTeamMgr()
{
    NextArenaTeamId = 1;
}

ArenaTeamMgr::~ArenaTeamMgr()
{
    for (ArenaTeamContainer::iterator itr = ArenaTeamStore.begin(); itr != ArenaTeamStore.end(); ++itr)
        delete itr->second;
}

ArenaTeamMgr* ArenaTeamMgr::instance()
{
    static ArenaTeamMgr instance;
    return &instance;
}

// Arena teams collection
ArenaTeam* ArenaTeamMgr::GetArenaTeamById(uint32 arenaTeamId) const
{
    ArenaTeamContainer::const_iterator itr = ArenaTeamStore.find(arenaTeamId);
    if (itr != ArenaTeamStore.end())
        return itr->second;
    return nullptr;
}

void ArenaTeamMgr::AddPersonalArenaTeam(Player* player, uint8 slot, const PersonalArenaStats& stats)
{
    PersonalArenaTeamStore[player->GetGUID()][slot] = stats;
}

void ArenaTeamMgr::LoadPersonalArenaTeams()
{
    uint32 oldMSTime = getMSTime();

    
    QueryResult result = CharacterDatabase.Query("SELECT `guid`, `slot`, `weekGames`, `weekWins`, `seasonGames`, `seasonWins`, `personalRating`, `matchMakerRating`, `rank` FROM character_arena_stats");

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 personal arena teams. DB table `character_arena_stats` is empty!");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        ObjectGuid::LowType guid = fields[0].GetUInt64();
        uint8  slot              = fields[1].GetUInt8();
        uint16 weekGames         = fields[2].GetUInt16();
        uint16 weekWins          = fields[3].GetUInt16();
        uint16 seasonGames       = fields[4].GetUInt16();
        uint16 seasonWins        = fields[5].GetUInt16();
        uint16 personalRating    = fields[6].GetUInt16();
        uint16 matchMakerRating  = fields[7].GetUInt16();
        uint32 rank              = fields[8].GetUInt32();

        ObjectGuid playerGUID = ObjectGuid::Create<HighGuid::Player>(guid);
        PersonalArenaTeamStore[playerGUID][slot] = { weekGames, weekWins, seasonGames, seasonWins, personalRating, matchMakerRating, rank };

        ++count;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded {} personal arena teams in {} ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void ArenaTeamMgr::AddArenaTeam(ArenaTeam* arenaTeam)
{
    ArenaTeam*& team = ArenaTeamStore[arenaTeam->GetId()];
    ASSERT((team == nullptr) || (team == arenaTeam), "Duplicate arena team with ID %u", arenaTeam->GetId());
    team = arenaTeam;
}

void ArenaTeamMgr::RemoveArenaTeam(uint32 arenaTeamId)
{
    ArenaTeamStore.erase(arenaTeamId);
}

uint32 ArenaTeamMgr::GenerateArenaTeamId()
{
    if (NextArenaTeamId >= 0xFFFFFFFE)
    {
        TC_LOG_ERROR("bg.battleground", "Arena team ids overflow!! Can't continue, shutting down server. ");
        World::StopNow(ERROR_EXIT_CODE);
    }
    return NextArenaTeamId++;
}
