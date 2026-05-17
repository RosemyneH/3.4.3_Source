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

#ifndef TRINITYCORE_TEMPSUMMON_H
#define TRINITYCORE_TEMPSUMMON_H

#include "Creature.h"

struct SummonPropertiesEntry;

class TC_GAME_API TempSummon : public Creature
{
    public:
        explicit TempSummon(SummonPropertiesEntry const* properties, WorldObject* owner, bool isWorldObject);
        virtual ~TempSummon();
        void Update(uint32 diff) override;
        virtual void InitStats(WorldObject* summoner, Milliseconds duration);
        virtual void InitSummon(WorldObject* summoner);
        void UpdateObjectVisibilityOnCreate() override;
        void UpdateObjectVisibilityOnDestroy() override;
        virtual void UnSummon(uint32 msTime = 0);
        void RemoveFromWorld() override;
        void SetTempSummonType(TempSummonType type);
        void SaveToDB(uint32 /*mapid*/, std::vector<Difficulty> const& /*spawnDifficulties*/) override { }
        WorldObject* GetSummoner() const;
        Unit* GetSummonerUnit() const;
        Creature* GetSummonerCreatureBase() const;
        GameObject* GetSummonerGameObject() const;
        ObjectGuid GetSummonerGUID() const { return m_summonerGUID; }
        TempSummonType GetSummonType() const { return m_type; }
        Milliseconds GetTimer() const { return m_timer; }
        void RefreshTimer() { m_timer = m_lifetime; }
        void ModifyTimer(Milliseconds mod) { m_timer += mod; m_lifetime += mod; }
        Optional<uint32> GetCreatureIdVisibleToSummoner() const { return m_creatureIdVisibleToSummoner; }
        Optional<uint32> GetDisplayIdVisibleToSummoner() const { return m_displayIdVisibleToSummoner; }
        bool CanFollowOwner() const { return m_canFollowOwner; }
        void SetCanFollowOwner(bool can) { m_canFollowOwner = can; }

        SummonPropertiesEntry const* const m_Properties;

        std::string GetDebugInfo() const override;

    protected:
        std::ptrdiff_t FindUsableTotemSlot(Unit const* summoner) const;

    private:
        bool IsSharingTotemSlotWith(ObjectGuid objectGuid) const;
        TempSummonType m_type;
        Milliseconds m_timer;
        Milliseconds m_lifetime;
        ObjectGuid m_summonerGUID;
        Optional<uint32> m_creatureIdVisibleToSummoner;
        Optional<uint32> m_displayIdVisibleToSummoner;
        bool m_canFollowOwner;
};

class TC_GAME_API Minion : public TempSummon
{
    public:
        Minion(SummonPropertiesEntry const* properties, Unit* owner, bool isWorldObject);
        void InitStats(WorldObject* summoner, Milliseconds duration) override;
        void RemoveFromWorld() override;
        void setDeathState(DeathState s) override;
        Unit* GetOwner() const { return m_owner; }

        // Warlock pets
        bool IsPetImp() const { return GetEntry() == NPC_IMP; }
        bool IsPetFelhunter() const { return GetEntry() == NPC_FELHUNTER; }
        bool IsPetVoidwalker() const { return GetEntry() == NPC_VOIDWALKER; }
        bool IsPetSayaad() const { return GetEntry() == NPC_SUCCUBUS || GetEntry() == NPC_INCUBUS; }
        bool IsPetDoomguard() const { return GetEntry() == NPC_DOOMGUARD; }
        bool IsPetFelguard() const { return GetEntry() == NPC_FELGUARD; }
        bool IsWarlockPet() const { return IsPetImp() || IsPetFelhunter() || IsPetVoidwalker() || IsPetSayaad() || IsPetDoomguard() || IsPetFelguard(); }

        // Death Knight pets
        bool IsPetGhoul() const { return GetEntry() == NPC_GHOUL; } // Ghoul may be guardian or pet
        bool IsRisenAlly() const { return GetEntry() == NPC_RISEN_ALLY; }

        // Shaman pet
        bool IsSpiritWolf() const { return GetEntry() == NPC_FERAL_SPIRIT; } // Spirit wolf from feral spirits

        bool IsGuardianPet() const;

        std::string GetDebugInfo() const override;
    protected:
        Unit* const m_owner;
};

class TC_GAME_API Guardian : public Minion
{
    public:
        Guardian(SummonPropertiesEntry const* properties, Unit* owner, bool isWorldObject);
        void InitStats(WorldObject* summoner, Milliseconds duration) override;
        bool InitStatsForLevel(uint8 level);
        void InitSummon(WorldObject* summoner) override;

        bool UpdateStats(Stats stat) override;
        bool UpdateAllStats() override;
        void UpdateResistances(uint32 school) override;
        void UpdateArmor() override;
        void UpdateMaxHealth() override;
        void UpdateMaxPower(Powers power) override;
        void UpdateAttackPowerAndDamage(bool ranged = false) override;
        void UpdateDamagePhysical(WeaponAttackType attType) override;

        int32 GetBonusDamage() const { return m_bonusSpellDamage; }
        float GetBonusStatFromOwner(Stats stat) const { return m_statFromOwner[stat]; }
        void SetBonusDamage(int32 damage);
        std::string GetDebugInfo() const override;
    protected:
        int32   m_bonusSpellDamage;
        float   m_statFromOwner[MAX_STATS];
};

class TC_GAME_API Puppet : public Minion
{
    public:
        Puppet(SummonPropertiesEntry const* properties, Unit* owner);
        void InitStats(WorldObject* summoner, Milliseconds duration) override;
        void InitSummon(WorldObject* summoner) override;
        void Update(uint32 time) override;
};

class TC_GAME_API ForcedUnsummonDelayEvent : public BasicEvent
{
public:
    ForcedUnsummonDelayEvent(TempSummon& owner) : BasicEvent(), m_owner(owner) { }
    bool Execute(uint64 e_time, uint32 p_time) override;

private:
    TempSummon& m_owner;
};
#endif
