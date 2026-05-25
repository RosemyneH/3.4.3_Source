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

/*
This placeholder for the instance is needed for dungeon finding to be able
to give credit after the boss defined in lastEncounterDungeon is killed.
Without it, the party doing random dungeon won't get satchel of spoils and
gets instead the deserter debuff.
*/

#include "ScriptMgr.h"
#include "InstanceScript.h"

enum StockadeData
{
    DATA_RANDOLPH_MOLOCH = 0,
    DATA_LORD_OVERHEAT   = 1,
    DATA_HOGGER          = 2
};

DungeonEncounterData const encounters[] =
{
    { DATA_RANDOLPH_MOLOCH, {{ 1146 }} },
    { DATA_LORD_OVERHEAT,   {{ 1145 }} },
    { DATA_HOGGER,          {{ 1144 }} }
};

class instance_the_stockade : public InstanceMapScript
{
public:
    instance_the_stockade() : InstanceMapScript("instance_the_stockade", 34) { }

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_the_stockade_InstanceMapScript(map);
    }

    struct instance_the_stockade_InstanceMapScript : public InstanceScript
    {
        instance_the_stockade_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
        {
            SetHeaders("STK");
            SetBossNumber(3);
            LoadDungeonEncounterData(encounters);
        }
    };
};

void AddSC_instance_the_stockade()
{
    new instance_the_stockade();
}
