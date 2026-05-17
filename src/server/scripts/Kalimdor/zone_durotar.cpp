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

#include "CreatureAIImpl.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"
#include "Containers.h"

enum VoodooSpells
{
    SPELL_BREW      = 16712, // Special Brew
    SPELL_GHOSTLY   = 16713, // Ghostly
    SPELL_HEX1      = 16707, // Hex
    SPELL_HEX2      = 16708, // Hex
    SPELL_HEX3      = 16709, // Hex
    SPELL_GROW      = 16711, // Grow
    SPELL_LAUNCH    = 16716, // Launch (Whee!)
};

// 17009 - Voodoo
class spell_voodoo : public SpellScript
{
    PrepareSpellScript(spell_voodoo);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_BREW, SPELL_GHOSTLY, SPELL_HEX1, SPELL_HEX2, SPELL_HEX3, SPELL_GROW, SPELL_LAUNCH });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        uint32 spellid = RAND(SPELL_BREW, SPELL_GHOSTLY, RAND(SPELL_HEX1, SPELL_HEX2, SPELL_HEX3), SPELL_GROW, SPELL_LAUNCH);
        if (Unit* target = GetHitUnit())
            GetCaster()->CastSpell(target, spellid, false);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_voodoo::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

enum TrayExirEvents
{
    EVENT_DANCE = 1,
};

class ResetEmoteStateEvent : public BasicEvent
{
public:
    ResetEmoteStateEvent(Creature* me)
        : m_Me(me)
    {
    }

    bool Execute(uint64 /*time*/, uint32 /*diff*/) override
    {
        if (m_Me && m_Me->IsAlive())
            m_Me->SetEmoteState(EMOTE_ONESHOT_NONE);

        return true;
    }

private:
    Creature* m_Me;
};

class npc_master_vornal : public CreatureScript
{
public:
    npc_master_vornal() 
        : CreatureScript("npc_master_vornal") 
    { 
    }

    struct npc_master_vornalAI : public ScriptedAI
    {
        npc_master_vornalAI(Creature* creature) 
            : ScriptedAI(creature)
        {
            m_Events.ScheduleEvent(EVENT_DANCE, 5s);
        }

        void UpdateAI(uint32 diff) override
        {
            m_Events.Update(diff);

            if (m_Events.ExecuteEvent() == EVENT_DANCE)
            {
                ++m_Count;

                if (m_Count >= 36)
                    m_Count = 1;

                uint8 amount = 1;
                if (m_Count % 2 == 0)
                    ++amount;

                for (uint8 i = 0; i != amount; ++i)
                {
                    uint32 entry = Trinity::Containers::SelectRandomContainerElement(m_CreatureIds);
                    if (Creature* npc = me->FindNearestCreature(entry, 60.0f))
                    {
                        npc->SetEmoteState(EMOTE_ONESHOT_DANCE);

                        const Milliseconds value(urandms(30, 60));
                        npc->m_Events.AddEvent(new ResetEmoteStateEvent(npc), npc->m_Events.CalculateTime(value));
                    }
                }

                m_Events.Repeat(15s);
            }
        }

    private:
        EventMap m_Events;
        uint8 m_Count = 0;
        const std::array<uint32, 12> m_CreatureIds = { 10369, 5942, 3186, 3187, 6408, 5880, 7953, 3185, 3184, 3933, 3194, 11814 };
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_master_vornalAI(creature);
    }
};

void AddSC_durotar()
{
    // Ours
    new npc_master_vornal();

    // Theirs
    RegisterSpellScript(spell_voodoo);
}
