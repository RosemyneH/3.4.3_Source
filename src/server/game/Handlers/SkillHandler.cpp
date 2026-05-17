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
#include "Common.h"
#include "DB2Stores.h"
#include "GossipDef.h"
#include "Log.h"
#include "ObjectAccessor.h"
#include "Pet.h"
#include "Player.h"
#include "SpellPackets.h"
#include "TalentPackets.h"
#include "MiscPackets.h"

void WorldSession::HandleLearnTalentOpcode(WorldPackets::Talent::LearnTalent& packet)
{
    if (_player->LearnTalent(packet.TalentID, packet.RequestedRank))
        _player->SendTalentsInfoData(false);
}

void WorldSession::HandleLearnPreviewTalentsOpcode(WorldPackets::Talent::LearnPreviewTalents& packet)
{
    for (WorldPackets::Talent::TalentInfo const& talent : packet.Talents)
        _player->LearnTalent(talent.TalentID, talent.Rank);

    _player->SendTalentsInfoData(false);
}

void WorldSession::HandleConfirmRespecWipeOpcode(WorldPackets::Talent::ConfirmRespecWipe& confirmRespecWipe)
{
    Creature* unit = GetPlayer()->GetNPCIfCanInteractWith(confirmRespecWipe.RespecMaster, UNIT_NPC_FLAG_TRAINER, UNIT_NPC_FLAG_2_NONE);
    if (!unit)
    {
        TC_LOG_DEBUG("network", "WORLD: HandleConfirmRespecWipeOpcode - {} not found or you can't interact with him.", confirmRespecWipe.RespecMaster.ToString());
        return;
    }

    const uint8 respecType = confirmRespecWipe.RespecType;
    const bool petRespec = (respecType == SPEC_RESET_TALENTS) ? false : true;

    if (respecType != SPEC_RESET_TALENTS && respecType != SPEC_RESET_PET_TALENTS)
    {
        TC_LOG_DEBUG("network", "WORLD: HandleConfirmRespecWipeOpcode - reset type {} is not implemented.", respecType);
        return;
    }

    if (!unit->CanResetTalents(_player, petRespec))
        return;

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    if (petRespec)
    {
        Pet* pet = _player->GetPet();
        if (pet == nullptr || !pet->ResetTalents())
            return;
    }
    else
    {
        if (!_player->ResetTalents())
            return;
    }

    _player->SendTalentsInfoData(petRespec);
    unit->CastSpell(_player, 14867, true);                  //spell: "Untalent Visual Effect"
}

void WorldSession::HandleUnlearnSkillOpcode(WorldPackets::Spells::UnlearnSkill& packet)
{
    SkillRaceClassInfoEntry const* rcEntry = sDB2Manager.GetSkillRaceClassInfo(packet.SkillLine, GetPlayer()->GetRace(), GetPlayer()->GetClass());
    if (!rcEntry || !(rcEntry->Flags & SKILL_FLAG_UNLEARNABLE))
        return;

    GetPlayer()->SetSkill(packet.SkillLine, 0, 0, 0);
}

void WorldSession::HandleRemoveGlyphOpcode(WorldPackets::Talent::RemoveGlyph& packet)
{
    if (packet.GlyphSlot >= MAX_GLYPH_SLOT_INDEX)
    {
        TC_LOG_DEBUG("network", "Client sent wrong glyph slot number in opcode CMSG_REMOVE_GLYPH {}", packet.GlyphSlot);
        return;
    }

    if (uint32 glyph = _player->GetGlyph(packet.GlyphSlot))
    {
        if (GlyphPropertiesEntry const* gp = sGlyphPropertiesStore.LookupEntry(glyph))
        {
            _player->RemoveAurasDueToSpell(gp->SpellID);
            _player->SetGlyph(packet.GlyphSlot, 0);
            _player->SendTalentsInfoData(false);
        }
    }
}

void WorldSession::HandleShowTradeSkill(WorldPackets::Misc::ShowTradeSkill& packet)
{
    if (!sSkillLineStore.LookupEntry(packet.SkillLineID) || !sSpellMgr->GetSpellInfo(packet.SpellID, DIFFICULTY_NONE))
        return;

    Player* player = ObjectAccessor::FindPlayer(packet.PlayerGUID);
    if (!player)
        return;

    std::set<uint32> relatedSkills;
    relatedSkills.insert(packet.SkillLineID);

    for (SkillLineEntry const* skillLine : sSkillLineStore)
    {
        if (skillLine->ParentSkillLineID != packet.SkillLineID)
            continue;

        if (!player->HasSkill(skillLine->ParentSkillLineID))
            continue;

        relatedSkills.insert(skillLine->ParentSkillLineID);
    }

    const auto IsSpellPartOfSkillLine = [](uint32 skillId, uint32 spellId) -> bool
    {
        SkillLineAbilityMapBounds skillBounds = sSpellMgr->GetSkillLineAbilityMapBounds(spellId);
        for (SkillLineAbilityMap::const_iterator itr = skillBounds.first; itr != skillBounds.second; ++itr)
            if (itr->second->SkillLine == int32(skillId) &&
                (itr->second->TrivialSkillLineRankHigh != 0 && itr->second->TrivialSkillLineRankLow != 0))
                return true;

        return false;
    };

    std::set<uint32> profSpells;
    for (auto const& v : player->GetSpellMap())
    {
        if (v.second.state == PLAYERSPELL_REMOVED)
            continue;

        if (!v.second.active || v.second.disabled)
            continue;

        for (auto const& s : relatedSkills)
        {
            if (IsSpellPartOfSkillLine(s, v.first))
                profSpells.insert(v.first);
        }
    }

    if (profSpells.empty())
        return;

    WorldPackets::Misc::ShowTradeSkillResponse response;
    response.PlayerGUID = packet.PlayerGUID;
    response.SpellId = packet.SpellID;

    for (uint32 const& x : profSpells)
        response.KnownAbilitySpellIDs.push_back(x);

    for (uint32 const& v : relatedSkills)
    {
        response.SkillLineIDs.push_back(v);
        response.SkillRanks.push_back(player->GetSkillValue(v));
        response.SkillMaxRanks.push_back(player->GetMaxSkillValue(v));
    }

    _player->SendDirectMessage(response.Write());
}

void WorldSession::HandleLearnPetTalentOpcode(WorldPackets::Talent::LearnPetTalent& packet)
{
    _player->LearnPetTalent(packet.PetGUID, packet.TalentID, packet.Rank);
    _player->SendTalentsInfoData(true);
}

void WorldSession::HandleLearnPetPreviewTalentsOpcode(WorldPackets::Talent::LearnPetPreviewTalents& packet)
{
    for (const auto& talent : packet.Talents)
        _player->LearnPetTalent(packet.PetGUID, talent.TalentID, talent.Rank);

    _player->SendTalentsInfoData(true);
}