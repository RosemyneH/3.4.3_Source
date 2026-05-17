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

#include "RestMgr.h"
#include "GameTime.h"
#include "Log.h"
#include "Player.h"
#include "Random.h"
#include "World.h"
#include "WorldSession.h"

RestMgr::RestMgr(Player* player)
:   _player(player),
    _restTime(0),
    _innAreaTriggerId(0),
    _restFlagMask(0),
    _restBonus(0.0f)
{
}

void RestMgr::SetRestBonus(float restBonus)
{
    // Reset restBonus (XP only) for max level players
    if (_player->GetLevel() >= sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
        restBonus = 0;

    if (restBonus < 0)
        restBonus = 0;

    int32 next_level_xp = _player->m_activePlayerData->NextLevelXP;

    float rest_bonus_max = float(next_level_xp) * 1.5f / 2;

    if (restBonus > rest_bonus_max)
        _restBonus = rest_bonus_max;
    else
        _restBonus = restBonus;

    // update data for client
    if (_player->GetsRecruitAFriendBonus(true) && (_player->GetSession()->IsARecruiter() || _player->GetSession()->GetRecruiterId() != 0))
        _player->SetRestState(REST_STATE_RAF_LINKED);
    else
    {
        if (_restBonus > 10)
            _player->SetRestState(REST_STATE_RESTED);
        else if (_restBonus <= 1)
            _player->SetRestState(REST_STATE_NOT_RAF_LINKED);
    }

    // RestTickUpdate
    _player->SetRestThreshold(uint32(_restBonus));
}

void RestMgr::AddRestBonus(float restBonus)
{
    // Don't add extra rest bonus to max level players.
    if (_player->GetLevel() >= sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
        restBonus = 0;

    float totalRestBonus = GetRestBonus() + restBonus;
    SetRestBonus(totalRestBonus);
}

void RestMgr::SetRestFlag(RestFlag restFlag)
{
    uint32 oldRestMask = _restFlagMask;
    _restFlagMask |= restFlag;

    if (!oldRestMask && _restFlagMask) // only set flag/time on the first rest state
    {
        _restTime = GameTime::GetGameTime();
        _player->SetPlayerFlag(PLAYER_FLAGS_RESTING);
    }
}

void RestMgr::RemoveRestFlag(RestFlag restFlag)
{
    uint32 oldRestMask = _restFlagMask;
    _restFlagMask &= ~restFlag;

    if (oldRestMask && !_restFlagMask) // only remove flag/time on the last rest state remove
    {
        _restTime = 0;
        _player->RemovePlayerFlag(PLAYER_FLAGS_RESTING);
    }
}

uint32 RestMgr::GetRestBonusFor(uint32 xp)
{
    uint32 rested_bonus = uint32(GetRestBonus()); // xp for each rested bonus

    if (rested_bonus > xp) // max rested_bonus == xp or (r+x) = 200% xp
        rested_bonus = xp;

    uint32 rested_loss = rested_bonus;

    SetRestBonus(GetRestBonus() - rested_loss);

    TC_LOG_DEBUG("entities.player", "RestMgr::GetRestBonus: Player '{}' ({}) gain {} xp (+{} Rested Bonus). Rested points={}", _player->GetGUID().ToString().c_str(), _player->GetName().c_str(), xp + rested_bonus, rested_bonus, GetRestBonus());
    return rested_bonus;
}

void RestMgr::Update(time_t now)
{
    if (roll_chance_i(3) && _restTime > 0) // freeze update
    {
        time_t timeDiff = now - _restTime;
        if (timeDiff >= 10)
        {
            _restTime = now;

            float bubble = 0.125f * sWorld->getRate(RATE_REST_INGAME);
            AddRestBonus(timeDiff * CalcExtraPerSec(bubble));
        }
    }
}

void RestMgr::LoadRestBonus(PlayerRestState state, float restBonus)
{
    _restBonus = restBonus;
    _player->SetRestState(state);
    _player->SetRestThreshold(uint32(restBonus));
}

float RestMgr::CalcExtraPerSec(float bubble) const
{
    return float(_player->m_activePlayerData->NextLevelXP) / 72000.0f * bubble;
}