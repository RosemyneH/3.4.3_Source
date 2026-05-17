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
#include "ObjectMgr.h"
#include "ArenaPackets.h"
#include "ArenaTeam.h"

void WorldSession::HandleArenaTeamRosterOpcode(WorldPackets::Arena::ArenaTeamRoster& packet)
{
    if (packet.TeamSlot > 2)
        return;

    WorldPackets::Arena::ArenaTeamRosterResponse arenaTeamRosterResponse;
    arenaTeamRosterResponse.TeamSize = ArenaTeam::GetTypeBySlot(packet.TeamSlot);
    SendPacket(arenaTeamRosterResponse.Write());
}
