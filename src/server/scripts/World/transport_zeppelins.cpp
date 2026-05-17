#include "GameObjectAI.h"
#include "WorldState.h"

enum ZeppelinEvent
{
    EVENT_UC_FROM_GROMGOL_ARRIVAL = 15312,
    EVENT_GROMGOL_FROM_UC_ARRIVAL = 15314,
    EVENT_OG_FROM_UC_ARRIVAL      = 15318,
    EVENT_UC_FROM_OG_ARRIVAL      = 15320,
    EVENT_OG_FROM_GROMGOL_ARRIVAL = 15322,
    EVENT_GROMGOL_FROM_OG_ARRIVAL = 15324,
    EVENT_WK_ARRIVAL              = 15431,
    EVENT_VL_FROM_UC_ARRIVAL      = 19126,
    EVENT_UC_FROM_VL_ARRIVAL      = 19127,
    EVENT_OG_FROM_BT_ARRIVAL      = 19137,
    EVENT_BT_FROM_OG_ARRIVAL      = 19139,
    EVENT_OG_FROM_TB_ARRIVAL      = 21868,
    EVENT_TB_FROM_OG_ARRIVAL      = 21870,
    EVENT_OG_TO_GROMGOL_DEPARTURE = 15323,
    EVENT_GROMGOL_TO_OG_DEPARTURE = 15325,
    EVENT_OG_TO_UC_DEPARTURE      = 15319,
    EVENT_UC_TO_OG_DEPARTURE      = 15321,
    EVENT_UC_TO_GROMGOL_DEPARTURE = 15313,
    EVENT_GROMGOL_TO_UC_DEPARTURE = 15315,
};

enum ZeppelinMaster
{
    NPC_NEZRAZ             = 3149,
    NPC_HINDENBURG         = 3150,
    NPC_FREZZA             = 9564,
    NPC_ZAPETTA            = 9566,
    NPC_SNURK_BUCKSQUICK   = 12136,
    NPC_SQUIBBY_OVERSPECK  = 12137,
    NPC_HARROWMEISER       = 23823,
    NPC_GREEB_RAMROCKET    = 26537,
    NPC_NARGO_SCREWBORE    = 26538,
    NPC_MEEFI_FARTHROTTLE  = 26539,
    NPC_DRENK_SPANNERSPARK = 26540,
    NPC_ZELLI_HOTNOZZLE    = 34765,
    NPC_KRENDLE_BIGPOCKETS = 34766,
};

enum ZeppelinPassenger
{
    // The Thundercaller
    NPC_SKY_CAPTAIN_CLOUDKICKER = 25077,
    NPC_CHIEF_OFFICER_COPPERNUT = 25070,
    // The Purple Princess
    NPC_SKY_CAPTAIN_CABLELAMP   = 25105,
    NPC_WATCHER_UMJIN           = 25107,
};

constexpr float SEARCH_RANGE_ZEPPELIN_MASTER = 32.0f;

 // 175080 The Iron Eagle - Grom'gol to Orgrimmar
struct go_transport_the_iron_eagle : GameObjectAI
{
    go_transport_the_iron_eagle(GameObject* object) : GameObjectAI(object) { };

    void EventInform(uint32 eventId) override
    {
        sWorldState->HandleConditionStateChange(WORLD_STATE_CONDITION_THE_IRON_EAGLE, static_cast<WorldStateConditionState>(eventId));
        switch (eventId)
        {
            case EVENT_GROMGOL_FROM_OG_ARRIVAL:
                if (Creature* creature = me->FindNearestCreature(NPC_NEZRAZ, SEARCH_RANGE_ZEPPELIN_MASTER))
                    creature->AI()->Talk(0);
                break;
            case EVENT_OG_FROM_GROMGOL_ARRIVAL:
                if (Creature* creature = me->FindNearestCreature(NPC_SNURK_BUCKSQUICK, 150.0f))
                    creature->AI()->Talk(0);
                break;
            default:
                return;
        }
    }
};

// 164871 The Thundercaller - Undercity to Orgrimmar
struct go_transport_the_thundercaller : GameObjectAI
{
    go_transport_the_thundercaller(GameObject* object) : GameObjectAI(object) { };

    void EventInform(uint32 eventId) override
    {
        sWorldState->HandleConditionStateChange(WORLD_STATE_CONDITION_THE_THUNDERCALLER, static_cast<WorldStateConditionState>(eventId));
        switch (eventId)
        {
            case EVENT_OG_FROM_UC_ARRIVAL:
                if (Creature* creature = me->FindNearestCreature(NPC_FREZZA, SEARCH_RANGE_ZEPPELIN_MASTER))
                    creature->AI()->Talk(0);
                break;
            case EVENT_UC_FROM_OG_ARRIVAL:
                if (Creature* creature = me->FindNearestCreature(NPC_ZAPETTA, SEARCH_RANGE_ZEPPELIN_MASTER))
                    creature->AI()->Talk(0);
                break;
            case EVENT_OG_TO_UC_DEPARTURE:
                break;
            case EVENT_UC_TO_OG_DEPARTURE:
                if (Creature* creature = me->FindNearestCreature(NPC_ZAPETTA, SEARCH_RANGE_ZEPPELIN_MASTER))
                    creature->AI()->Talk(1);
                break;
            default:
                return;
        }
    }
};

// 176495 The Purple Princess - Grom'Gol to Undercity
struct go_transport_the_purple_princess : GameObjectAI
{
    go_transport_the_purple_princess(GameObject* object) : GameObjectAI(object) { };

    void EventInform(uint32 eventId) override
    {
        sWorldState->HandleConditionStateChange(WORLD_STATE_CONDITION_THE_PURPLE_PRINCESS, static_cast<WorldStateConditionState>(eventId));
        switch (eventId)
        {
            case EVENT_GROMGOL_FROM_UC_ARRIVAL:
                if (Creature* creature = me->FindNearestCreature(NPC_SQUIBBY_OVERSPECK, SEARCH_RANGE_ZEPPELIN_MASTER))
                    creature->AI()->Talk(0);
                break;
            case EVENT_UC_FROM_GROMGOL_ARRIVAL:
                if (Creature* creature = me->FindNearestCreature(NPC_HINDENBURG, SEARCH_RANGE_ZEPPELIN_MASTER))
                    creature->AI()->Talk(0);
                break;
            case EVENT_UC_TO_GROMGOL_DEPARTURE:
            case EVENT_GROMGOL_TO_UC_DEPARTURE:
                break;
            default:
                return;
        }
    }
};

void AddSC_transport_zeppelins()
{
    RegisterGameObjectAI(go_transport_the_iron_eagle);
    RegisterGameObjectAI(go_transport_the_thundercaller);
    RegisterGameObjectAI(go_transport_the_purple_princess);
}