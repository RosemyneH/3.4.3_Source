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

#ifndef TRINITYCORE_ARENATEAM_H
#define TRINITYCORE_ARENATEAM_H

#include "Define.h"
#include "DatabaseEnvFwd.h"
#include "ObjectGuid.h"
#include <list>
#include <string>

class WorldSession;
class WorldPacket;
class Player;
class Group;

struct PersonalArenaStats
{
    uint16 WeekGames        = 0;
    uint16 WeekWins         = 0;
    uint16 SeasonGames      = 0;
    uint16 SeasonWins       = 0;
    uint16 PersonalRating   = 0;
    uint16 MatchMakerRating = 0;
    uint32 Rank             = 0;
};

enum ArenaTeamCommandTypes
{
    ERR_ARENA_TEAM_CREATE_S                 = 0x00,
    ERR_ARENA_TEAM_INVITE_SS                = 0x01,
    ERR_ARENA_TEAM_QUIT_S                   = 0x03,
    ERR_ARENA_TEAM_FOUNDER_S                = 0x0E
};

enum ArenaTeamCommandErrors
{
    ERR_ARENA_TEAM_CREATED                  = 0x00,
    ERR_ARENA_TEAM_INTERNAL                 = 0x01,
    ERR_ALREADY_IN_ARENA_TEAM               = 0x02,
    ERR_ALREADY_IN_ARENA_TEAM_S             = 0x03,
    ERR_INVITED_TO_ARENA_TEAM               = 0x04,
    ERR_ALREADY_INVITED_TO_ARENA_TEAM_S     = 0x05,
    ERR_ARENA_TEAM_NAME_INVALID             = 0x06,
    ERR_ARENA_TEAM_NAME_EXISTS_S            = 0x07,
    ERR_ARENA_TEAM_LEADER_LEAVE_S           = 0x08,
    ERR_ARENA_TEAM_PERMISSIONS              = 0x08,
    ERR_ARENA_TEAM_PLAYER_NOT_IN_TEAM       = 0x09,
    ERR_ARENA_TEAM_PLAYER_NOT_IN_TEAM_SS    = 0x0A,
    ERR_ARENA_TEAM_PLAYER_NOT_FOUND_S       = 0x0B,
    ERR_ARENA_TEAM_NOT_ALLIED               = 0x0C,
    ERR_ARENA_TEAM_IGNORING_YOU_S           = 0x13,
    ERR_ARENA_TEAM_TARGET_TOO_LOW_S         = 0x15,
    ERR_ARENA_TEAM_TARGET_TOO_HIGH_S        = 0x16,
    ERR_ARENA_TEAM_TOO_MANY_MEMBERS_S       = 0x17,
    ERR_ARENA_TEAM_NOT_FOUND                = 0x1B,
    ERR_ARENA_TEAMS_LOCKED                  = 0x1E,
    ERR_ARENA_TEAM_TOO_MANY_CREATE          = 0x21,
};

enum ArenaTeamEvents
{
    ERR_ARENA_TEAM_JOIN_SS                  = 4,            // player name + arena team name
    ERR_ARENA_TEAM_LEAVE_SS                 = 5,            // player name + arena team name
    ERR_ARENA_TEAM_REMOVE_SSS               = 6,            // player name + arena team name + captain name
    ERR_ARENA_TEAM_LEADER_IS_SS             = 7,            // player name + arena team name
    ERR_ARENA_TEAM_LEADER_CHANGED_SSS       = 8,            // old captain + new captain + arena team name
    ERR_ARENA_TEAM_DISBANDED_S              = 9             // captain name + arena team name
};

// EnumUtils: DESCRIBE THIS
enum ArenaTeamTypes
{
    ARENA_TEAM_2v2      = 2,
    ARENA_TEAM_3v3      = 3,
    ARENA_TEAM_5v5      = 5
};

struct TC_GAME_API ArenaTeamMember
{
    ObjectGuid Guid;
    std::string Name;
    uint8 Class;

    void ModifyPersonalRating(int32 mod, uint32 type);
    void ModifyMatchmakerRating(int32 mod, uint32 slot);
};

#define MAX_ARENA_SLOT 3                                    // 0..2 slots

class TC_GAME_API ArenaTeam
{
    public:
        ArenaTeam(Group* group, uint8 type);
        ArenaTeam(Player* player, uint8 type);
        ~ArenaTeam();

        typedef std::list<ArenaTeamMember> MemberList;

        uint32 GetId() const { return TeamId; }
        uint32 GetType() const { return Type; }
        uint8 GetSlot() const { return GetSlotByType(GetType()); }
        static uint8 GetSlotByType(uint32 type);
        static uint8 GetTypeBySlot(uint8 slot);
        std::string const& GetName() const { return TeamName; }

        uint32 GetRating() const;
        uint32 GetAverageMMR(Group* group) const;

        size_t GetMembersSize() const { return Members.size(); }
        bool Empty() const { return Members.empty(); }
        MemberList::iterator m_membersBegin() { return Members.begin(); }
        MemberList::iterator m_membersEnd() { return Members.end(); }
        bool IsMember(ObjectGuid guid) const;

        ArenaTeamMember* GetMember(ObjectGuid guid);

        bool IsFighting() const;

        void SaveToDB(bool forceMemberSave = false);

        void BroadcastPacket(WorldPacket* packet);
        void NotifyStatsChanged();

        void SendStats(WorldSession* session);

        int32 GetMatchmakerRatingMod(uint32 ownRating, uint32 opponentRating, bool won);
        int32 GetRatingMod(uint32 ownRating, uint32 opponentRating, bool won);
        float GetChanceAgainst(uint32 ownRating, uint32 opponentRating);
        int32 WonAgainst(uint32 Own_MMRating, uint32 Opponent_MMRating, int32& rating_change);
        void MemberWon(Player* player, uint32 againstMatchmakerRating, int32 MatchmakerRatingChange);
        int32 LostAgainst(uint32 Own_MMRating, uint32 Opponent_MMRating, int32& rating_change);
        void MemberLost(Player* player, uint32 againstMatchmakerRating, int32 MatchmakerRatingChange = -12);
        void OfflineMemberLost(ObjectGuid guid, uint32 againstMatchmakerRating, int32 MatchmakerRatingChange = -12);

        void FinishWeek();

protected:
        uint8 Type;
        uint32 TeamId;
        std::string TeamName = "Unknown";
        MemberList Members;

private:
    void CreateTeam(Player* player);
};
#endif
