#ifndef WORLD_STATE_H
#define WORLD_STATE_H

#include "Player.h"
#include <atomic>

enum WorldStateCondition
{
    // Zeppelins
    WORLD_STATE_CONDITION_THE_THUNDERCALLER   = 164871,
    WORLD_STATE_CONDITION_THE_IRON_EAGLE      = 175080,
    WORLD_STATE_CONDITION_THE_PURPLE_PRINCESS = 176495,
};

enum WorldStateConditionState
{
    WORLD_STATE_CONDITION_STATE_NONE = 0,
};

// Intended for implementing server wide scripts, note: all behaviour must be safeguarded towards multithreading
class WorldState
{
public:
    WorldState();
    virtual ~WorldState();
    static WorldState* instance();
    bool IsConditionFulfilled(WorldStateCondition conditionId, WorldStateConditionState state = WORLD_STATE_CONDITION_STATE_NONE) const;
    void HandleConditionStateChange(WorldStateCondition conditionId, WorldStateConditionState state);
private:
    std::map<WorldStateCondition, std::atomic<WorldStateConditionState>> _transportStates; // atomic to avoid having to lock
};

#define sWorldState WorldState::instance()
#endif