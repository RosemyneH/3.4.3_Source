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

#ifndef RestMgr_h__
#define RestMgr_h__

#include "Define.h"
#include <ctime>

class Player;

enum PlayerRestState : uint8
{
    REST_STATE_RESTED           = 0x01,
    REST_STATE_NOT_RAF_LINKED   = 0x02,
    REST_STATE_RAF_LINKED       = 0x06
};

enum RestFlag : uint32
{
    REST_FLAG_IN_TAVERN         = 0x1,
    REST_FLAG_IN_CITY           = 0x2,
    REST_FLAG_IN_FACTION_AREA   = 0x4 // used with AREA_FLAG_REST_ZONE_*
};

class TC_GAME_API RestMgr
{
    friend class Player;
public:
    explicit RestMgr(Player* player);
    ~RestMgr() { }

    float GetRestBonus() const { return _restBonus; }

    void SetRestBonus(float restBonus);
    void AddRestBonus(float restBonus);

    bool HasRestFlag(RestFlag restFlag) const { return (_restFlagMask & restFlag) != 0; }
    void SetRestFlag(RestFlag restFlag);
    void RemoveRestFlag(RestFlag restFlag);

    uint32 GetRestBonusFor(uint32 xp);
    uint32 GetInnTriggerID() const { return _innAreaTriggerId; }
    void SetInnTriggerID(uint32 id) { _innAreaTriggerId = id; }

    void Update(time_t now);

protected:
    void LoadRestBonus(PlayerRestState state, float restBonus);
    float CalcExtraPerSec(float bubble) const;

private:
    Player* _player;
    time_t _restTime;
    uint32 _innAreaTriggerId;
    float _restBonus;
    uint32 _restFlagMask;
};

#endif // RestMgr_h__