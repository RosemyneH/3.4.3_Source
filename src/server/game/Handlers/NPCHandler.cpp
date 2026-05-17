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
#include "Battleground.h"
#include "BattlegroundMgr.h"
#include "Common.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "DatabaseEnv.h"
#include "DB2Stores.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "GossipDef.h"
#include "Item.h"
#include "ItemPackets.h"
#include "Log.h"
#include "Map.h"
#include "NPCPackets.h"
#include "ObjectMgr.h"
#include "Pet.h"
#include "PetPackets.h"
#include "Player.h"
#include "ReputationMgr.h"
#include "SpellInfo.h"
#include "Trainer.h"
#include "WorldPacket.h"

enum class TabardVendorType : int32
{
    Guild       = 0,
    Personal    = 1,
};

void WorldSession::HandleTabardVendorActivateOpcode(WorldPackets::NPC::TabardVendorActivate const& tabardVendorActivate)
{
    Creature* unit = GetPlayer()->GetNPCIfCanInteractWith(tabardVendorActivate.Vendor, UNIT_NPC_FLAG_TABARDDESIGNER, UNIT_NPC_FLAG_2_NONE);
    if (!unit)
    {
        TC_LOG_DEBUG("network", "WORLD: HandleTabardVendorActivateOpcode - {} not found or you can not interact with him.", tabardVendorActivate.Vendor.ToString());
        return;
    }

    TabardVendorType type = TabardVendorType(tabardVendorActivate.Type);
    if (type != TabardVendorType::Guild && type != TabardVendorType::Personal)
        return;

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    SendTabardVendorActivate(tabardVendorActivate.Vendor, TabardVendorType(tabardVendorActivate.Type));
}

void WorldSession::SendTabardVendorActivate(ObjectGuid guid, TabardVendorType type)
{
    WorldPackets::NPC::NPCInteractionOpenResult npcInteraction;
    npcInteraction.Npc = guid;
    npcInteraction.InteractionType = [&]
    {
        switch (type)
        {
            case TabardVendorType::Guild:
                return PlayerInteractionType::GuildTabardVendor;
            case TabardVendorType::Personal:
                return PlayerInteractionType::PersonalTabardVendor;
            default:
                ABORT_MSG("Unsupported tabard vendor type %d", AsUnderlyingType(type));
        }
    }();
    npcInteraction.Success = true;
    SendPacket(npcInteraction.Write());
}

void WorldSession::SendShowMailBox(ObjectGuid guid)
{
    WorldPackets::NPC::NPCInteractionOpenResult npcInteraction;
    npcInteraction.Npc = guid;
    npcInteraction.InteractionType = PlayerInteractionType::MailInfo;
    npcInteraction.Success = true;
    SendPacket(npcInteraction.Write());
}

void WorldSession::HandleTrainerListOpcode(WorldPackets::NPC::Hello& packet)
{
    Creature* npc = GetPlayer()->GetNPCIfCanInteractWith(packet.Unit, UNIT_NPC_FLAG_TRAINER, UNIT_NPC_FLAG_2_NONE);
    if (!npc)
    {
        TC_LOG_DEBUG("network", "WorldSession::SendTrainerList - {} not found or you can not interact with him.", packet.Unit.ToString());
        return;
    }

    SendTrainerList(npc);
}

void WorldSession::SendTrainerList(Creature* npc)
{
    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    Trainer::Trainer const* trainer = sObjectMgr->GetTrainer(npc->GetEntry());
    if (!trainer)
    {
        TC_LOG_DEBUG("network", "WorldSession::SendTrainerList - trainer spells not found for trainer {}", npc->GetGUID().ToString());
        return;
    }

    if (!trainer->IsTrainerValidForPlayer(_player))
    {
        TC_LOG_DEBUG("network", "WorldSession: SendTrainerList - trainer {} not valid for player {}", npc->GetGUID().ToString(), GetPlayerInfo());
        return;
    }

    _player->PlayerTalkClass->GetInteractionData().Reset();
    _player->PlayerTalkClass->GetInteractionData().SourceGuid = npc->GetGUID();
    _player->PlayerTalkClass->GetInteractionData().TrainerId = npc->GetEntry();
    trainer->SendSpells(npc, _player, GetSessionDbLocaleIndex());
}

void WorldSession::HandleTrainerBuySpellOpcode(WorldPackets::NPC::TrainerBuySpell& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_TRAINER_BUY_SPELL {}, learn spell id is: {}", packet.TrainerGUID.ToString(), packet.SpellID);

    Creature* npc = GetPlayer()->GetNPCIfCanInteractWith(packet.TrainerGUID, UNIT_NPC_FLAG_TRAINER, UNIT_NPC_FLAG_2_NONE);
    if (!npc)
    {
        TC_LOG_DEBUG("network", "WORLD: HandleTrainerBuySpellOpcode - {} not found or you can not interact with him.", packet.TrainerGUID.ToString());
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    if (_player->PlayerTalkClass->GetInteractionData().SourceGuid != packet.TrainerGUID)
        return;

    // alistar: disabled..
    /*if (_player->PlayerTalkClass->GetInteractionData().TrainerId != uint32(packet.TrainerID))
        return;*/

    Trainer::Trainer const* trainer = sObjectMgr->GetTrainer(npc->GetEntry()); // alistar: we use npc entry instead of it's trainerId
    if (!trainer)
        return;

    trainer->TeachSpell(npc, _player, packet.SpellID);
}

void WorldSession::HandleGossipHelloOpcode(WorldPackets::NPC::Hello& packet)
{
    // alistar: from sniffs client sends SMSG_GOSSIP_COMPLETE after CMSG_TALK_TO_GOSSIP
    _player->PlayerTalkClass->SendCloseGossip();

    Creature* unit = GetPlayer()->GetNPCIfCanInteractWith(packet.Unit, UNIT_NPC_FLAG_GOSSIP, UNIT_NPC_FLAG_2_NONE);
    if (!unit)
    {
        TC_LOG_DEBUG("network", "WORLD: HandleGossipHelloOpcode - {} not found or you can not interact with him.", packet.Unit.ToString());
        return;
    }

    // set faction visible if needed
    if (FactionTemplateEntry const* factionTemplateEntry = sFactionTemplateStore.LookupEntry(unit->GetFaction()))
        _player->GetReputationMgr().SetVisible(factionTemplateEntry);

    GetPlayer()->RemoveAurasWithInterruptFlags(SpellAuraInterruptFlags::Interacting);

    // Stop the npc if moving
    if (uint32 pause = unit->GetMovementTemplate().GetInteractionPauseTimer())
        unit->PauseMovement(pause);
    unit->SetHomePosition(unit->GetPosition());

    // If spiritguide, no need for gossip menu, just put player into resurrect queue
    if (unit->IsAreaSpiritHealer())
    {
        Battleground* bg = _player->GetBattleground();
        if (bg)
        {
            bg->AddPlayerToResurrectQueue(unit->GetGUID(), _player->GetGUID());
            sBattlegroundMgr->SendAreaSpiritHealerQueryOpcode(_player, bg, unit->GetGUID());
            return;
        }
    }

    _player->PlayerTalkClass->ClearMenus();
    if (!unit->AI()->OnGossipHello(_player))
    {
//        _player->TalkedToCreature(unit->GetEntry(), unit->GetGUID());
        _player->PrepareGossipMenu(unit, _player->GetGossipMenuForSource(unit), true);
        _player->SendPreparedGossip(unit);
    }
}

void WorldSession::HandleGossipSelectOptionOpcode(WorldPackets::NPC::GossipSelectOption& packet)
{
    GossipMenuItem const* gossipMenuItem = _player->PlayerTalkClass->GetGossipMenu().GetItem(packet.GossipOptionID);
    if (!gossipMenuItem)
        return;

    // Prevent cheating on C++ scripted menus
    if (_player->PlayerTalkClass->GetInteractionData().SourceGuid != packet.GossipUnit)
        return;

    Creature* unit = nullptr;
    GameObject* go = nullptr;
    if (packet.GossipUnit.IsCreatureOrVehicle())
    {
        unit = GetPlayer()->GetNPCIfCanInteractWith(packet.GossipUnit, UNIT_NPC_FLAG_GOSSIP, UNIT_NPC_FLAG_2_NONE);
        if (!unit)
        {
            TC_LOG_DEBUG("network", "WORLD: HandleGossipSelectOptionOpcode - {} not found or you can't interact with him.", packet.GossipUnit.ToString());
            return;
        }
    }
    else if (packet.GossipUnit.IsGameObject())
    {
        go = _player->GetGameObjectIfCanInteractWith(packet.GossipUnit);
        if (!go)
        {
            TC_LOG_DEBUG("network", "WORLD: HandleGossipSelectOptionOpcode - {} not found or you can't interact with it.", packet.GossipUnit.ToString());
            return;
        }
    }
    else
    {

        TC_LOG_DEBUG("network", "WORLD: HandleGossipSelectOptionOpcode - unsupported {}.", packet.GossipUnit.ToString());
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    if ((unit && unit->GetScriptId() != unit->LastUsedScriptID) || (go && go->GetScriptId() != go->LastUsedScriptID))
    {
        TC_LOG_DEBUG("network", "WORLD: HandleGossipSelectOptionOpcode - Script reloaded while in use, ignoring and set new scipt id");
        if (unit)
            unit->LastUsedScriptID = unit->GetScriptId();

        if (go)
            go->LastUsedScriptID = go->GetScriptId();
        _player->PlayerTalkClass->SendCloseGossip();
        return;
    }

    if (!packet.PromotionCode.empty())
    {
        if (unit)
        {
            if (!unit->AI()->OnGossipSelectCode(_player, packet.GossipID, gossipMenuItem->OrderIndex, packet.PromotionCode.c_str()))
                _player->OnGossipSelect(unit, packet.GossipOptionID, packet.GossipID);
        }
        else
        {
            if (!go->AI()->OnGossipSelectCode(_player, packet.GossipID, gossipMenuItem->OrderIndex, packet.PromotionCode.c_str()))
                _player->OnGossipSelect(go, packet.GossipOptionID, packet.GossipID);
        }
    }
    else
    {
        if (unit)
        {
            if (!unit->AI()->OnGossipSelect(_player, packet.GossipID, gossipMenuItem->OrderIndex))
                _player->OnGossipSelect(unit, packet.GossipOptionID, packet.GossipID);
        }
        else
        {
            if (!go->AI()->OnGossipSelect(_player, packet.GossipID, gossipMenuItem->OrderIndex))
                _player->OnGossipSelect(go, packet.GossipOptionID, packet.GossipID);
        }
    }
}

void WorldSession::HandleSpiritHealerActivate(WorldPackets::NPC::SpiritHealerActivate& packet)
{
    Creature* unit = GetPlayer()->GetNPCIfCanInteractWith(packet.Healer, UNIT_NPC_FLAG_SPIRIT_HEALER, UNIT_NPC_FLAG_2_NONE);
    if (!unit)
    {
        TC_LOG_DEBUG("network", "WORLD: HandleSpiritHealerActivateOpcode - {} not found or you can not interact with him.", packet.Healer.ToString());
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    SendSpiritResurrect();
}

void WorldSession::SendSpiritResurrect()
{
    _player->ResurrectPlayer(0.5f, true);
    _player->DurabilityLossAll(0.25f, true);

    // get corpse nearest graveyard
    WorldSafeLocsEntry const* corpseGrave = nullptr;
    WorldLocation corpseLocation = _player->GetCorpseLocation();
    if (_player->HasCorpse())
    {
        corpseGrave = sObjectMgr->GetClosestGraveyard(corpseLocation, _player->GetTeam(), _player);
    }

    // now can spawn bones
    _player->SpawnCorpseBones();

    // teleport to nearest from corpse graveyard, if different from nearest to player ghost
    if (corpseGrave)
    {
        WorldSafeLocsEntry const* ghostGrave = sObjectMgr->GetClosestGraveyard(*_player, _player->GetTeam(), _player);

        if (corpseGrave != ghostGrave)
            _player->TeleportTo(corpseGrave->Loc);
    }
}

void WorldSession::HandleBinderActivateOpcode(WorldPackets::NPC::Hello& packet)
{
    if (!GetPlayer()->IsInWorld() || !GetPlayer()->IsAlive())
        return;

    Creature* unit = GetPlayer()->GetNPCIfCanInteractWith(packet.Unit, UNIT_NPC_FLAG_INNKEEPER, UNIT_NPC_FLAG_2_NONE);
    if (!unit)
    {
        TC_LOG_DEBUG("network", "WORLD: HandleBinderActivateOpcode - {} not found or you can not interact with him.", packet.Unit.ToString());
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    SendBindPoint(unit);
}

void WorldSession::SendBindPoint(Creature* npc)
{
    // prevent set homebind to instances in any case
    if (GetPlayer()->GetMap()->Instanceable())
        return;

    uint32 bindspell = 3286;

    // send spell for homebinding (3286)
    npc->CastSpell(_player, bindspell, true);

    _player->PlayerTalkClass->SendCloseGossip();
}

void WorldSession::HandleRequestStabledPets(WorldPackets::NPC::RequestStabledPets& packet)
{
    if (!CheckStableMaster(packet.StableMaster))
        return;

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    // remove mounts this fix bug where getting pet from stable while mounted deletes pet.
    if (GetPlayer()->IsMounted())
        GetPlayer()->RemoveAurasByType(SPELL_AURA_MOUNTED);

    _player->SendStable(packet.StableMaster);
}

void WorldSession::SendPetStableResult(StableResult result)
{
    WorldPackets::Pet::PetStableResult petStableResult;
    petStableResult.Result = AsUnderlyingType(result);
    SendPacket(petStableResult.Write());
}

void WorldSession::HandleStablePet(WorldPackets::NPC::StablePet& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Recv CMSG_STABLE_PET");
    ObjectGuid npcGUID = packet.StableMaster;

    if (!GetPlayer()->IsAlive())
    {
        SendPetStableResult(StableResult::InternalError);
        return;
    }

    if (!CheckStableMaster(npcGUID))
    {
        SendPetStableResult(StableResult::InternalError);
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    PetStable* petStable = GetPlayer()->GetPetStable();
    if (!petStable)
        return;

    Pet* pet = _player->GetPet();

    // can't place in stable dead pet
    if ((pet && (!pet->IsAlive() || pet->getPetType() != HUNTER_PET))
        || (!pet && (petStable->UnslottedPets.size() != 1 || !petStable->UnslottedPets[0].Health || petStable->UnslottedPets[0].Type != HUNTER_PET)))
    {
        SendPetStableResult(StableResult::InternalError);
        return;
    }

    for (uint32 freeSlot = 0; freeSlot < petStable->MaxStabledPets; ++freeSlot)
    {
        if (!petStable->StabledPets[freeSlot])
        {
            if (pet)
            {
                // stable summoned pet
                _player->RemovePet(pet, PetSaveMode(PET_SAVE_FIRST_STABLE_SLOT + freeSlot));
                std::swap(petStable->StabledPets[freeSlot], petStable->CurrentPet);
                SendPetStableResult(StableResult::StableSuccess);
                return;
            }

            CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHAR_PET_SLOT_BY_ID);
            stmt->setUInt8(0, PetSaveMode(PET_SAVE_FIRST_STABLE_SLOT + freeSlot));
            stmt->setUInt32(1, _player->GetGUID().GetCounter());
            stmt->setUInt32(2, petStable->UnslottedPets[0].PetNumber);
            CharacterDatabase.Execute(stmt);

            // stable unsummoned pet
            petStable->StabledPets[freeSlot] = std::move(petStable->UnslottedPets.back());
            petStable->UnslottedPets.pop_back();
            SendPetStableResult(StableResult::StableSuccess);
            return;
        }
    }

    // not free stable slot
    SendPetStableResult(StableResult::InvalidSlot);
}

void WorldSession::HandleStableSwapPet(WorldPackets::NPC::StableSwapPet& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Recv CMSG_UNSTABLE_PET.");
    ObjectGuid npcGUID = packet.StableMaster;
    uint32 petnumber = packet.PetNumber;

    if (!CheckStableMaster(npcGUID))
    {
        SendPetStableResult(StableResult::InternalError);
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    PetStable* petStable = GetPlayer()->GetPetStable();
    if (!petStable)
    {
        SendPetStableResult(StableResult::InternalError);
        return;
    }

    auto stabledPet = std::find_if(petStable->StabledPets.begin(), petStable->StabledPets.end(), [petnumber](Optional<PetStable::PetInfo> const& pet)
    {
        return pet && pet->PetNumber == petnumber;
    });

    if (stabledPet == petStable->StabledPets.end())
    {
        SendPetStableResult(StableResult::InvalidSlot);
        return;
    }

    CreatureTemplate const* creatureInfo = sObjectMgr->GetCreatureTemplate((*stabledPet)->CreatureId);
    if (!creatureInfo || !creatureInfo->IsTameable(_player->CanTameExoticPets(), creatureInfo->GetDifficulty(DIFFICULTY_NONE)))
    {
        // if problem in exotic pet
        if (creatureInfo && creatureInfo->IsTameable(true, creatureInfo->GetDifficulty(DIFFICULTY_NONE)))
            SendPetStableResult(StableResult::CantControlExotic);
        else
            SendPetStableResult(StableResult::InternalError);
        return;
    }

    Pet* oldPet = _player->GetPet();
    if (oldPet)
    {
        // try performing a swap, client sends this packet instead of swap when starting from stabled slot
        if (!oldPet->IsAlive() || !oldPet->IsHunterPet())
        {
            SendPetStableResult(StableResult::InternalError);
            return;
        }

        _player->RemovePet(oldPet, PetSaveMode(PET_SAVE_FIRST_STABLE_SLOT + std::distance(petStable->StabledPets.begin(), stabledPet)));
    }
    else if (petStable->UnslottedPets.size() == 1)
    {
        if (petStable->CurrentPet || !petStable->UnslottedPets[0].Health || petStable->UnslottedPets[0].Type != HUNTER_PET)
        {
            SendPetStableResult(StableResult::InternalError);
            return;
        }

        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHAR_PET_SLOT_BY_ID);
        stmt->setUInt8(0, PetSaveMode(PET_SAVE_FIRST_STABLE_SLOT + std::distance(petStable->StabledPets.begin(), stabledPet)));
        stmt->setUInt32(1, _player->GetGUID().GetCounter());
        stmt->setUInt32(2, petStable->UnslottedPets[0].PetNumber);
        CharacterDatabase.Execute(stmt);

        // move unsummoned pet into CurrentPet slot so that it gets moved into stable slot later
        petStable->CurrentPet = std::move(petStable->UnslottedPets.back());
        petStable->UnslottedPets.pop_back();
    }
    else if (petStable->CurrentPet || !petStable->UnslottedPets.empty())
    {
        SendPetStableResult(StableResult::InternalError);
        return;
    }

    Pet* newPet = new Pet(_player, HUNTER_PET);
    if (!newPet->LoadPetFromDB(_player, 0, petnumber, false))
    {
        delete newPet;

        petStable->UnslottedPets.push_back(std::move(*petStable->CurrentPet));
        petStable->CurrentPet.reset();

        // update current pet slot in db immediately to maintain slot consistency, dismissed pet was already saved
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHAR_PET_SLOT_BY_ID);
        stmt->setUInt8(0, PET_SAVE_NOT_IN_SLOT);
        stmt->setUInt32(1, _player->GetGUID().GetCounter());
        stmt->setUInt32(2, petnumber);
        CharacterDatabase.Execute(stmt);

        SendPetStableResult(StableResult::InternalError);
    }
    else
    {
        // update current pet slot in db immediately to maintain slot consistency, dismissed pet was already saved
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHAR_PET_SLOT_BY_ID);
        stmt->setUInt8(0, PET_SAVE_AS_CURRENT);
        stmt->setUInt32(1, _player->GetGUID().GetCounter());
        stmt->setUInt32(2, petnumber);
        CharacterDatabase.Execute(stmt);

        SendPetStableResult(StableResult::UnstableSuccess);
    }
}

void WorldSession::HandleUnstablePet(WorldPackets::NPC::UnStablePet& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Recv CMSG_UNSTABLE_PET.");
    ObjectGuid npcGUID = packet.StableMaster;
    uint32 petnumber = packet.PetNumber;

    if (!CheckStableMaster(npcGUID))
    {
        SendPetStableResult(StableResult::InternalError);
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    PetStable* petStable = GetPlayer()->GetPetStable();
    if (!petStable)
    {
        SendPetStableResult(StableResult::InternalError);
        return;
    }

    auto stabledPet = std::find_if(petStable->StabledPets.begin(), petStable->StabledPets.end(), [petnumber](Optional<PetStable::PetInfo> const& pet)
    {
        return pet && pet->PetNumber == petnumber;
    });

    if (stabledPet == petStable->StabledPets.end())
    {
        SendPetStableResult(StableResult::InternalError);
        return;
    }

    CreatureTemplate const* creatureInfo = sObjectMgr->GetCreatureTemplate((*stabledPet)->CreatureId);
    if (!creatureInfo || !creatureInfo->IsTameable(_player->CanTameExoticPets(), creatureInfo->GetDifficulty(DIFFICULTY_NONE)))
    {
        // if problem in exotic pet
        if (creatureInfo && creatureInfo->IsTameable(true, creatureInfo->GetDifficulty(DIFFICULTY_NONE)))
            SendPetStableResult(StableResult::CantControlExotic);
        else
            SendPetStableResult(StableResult::InternalError);
        return;
    }

    Pet* oldPet = _player->GetPet();
    if (oldPet)
    {
        // try performing a swap, client sends this packet instead of swap when starting from stabled slot
        if (!oldPet->IsAlive() || !oldPet->IsHunterPet())
        {
            SendPetStableResult(StableResult::InternalError);
            return;
        }

        _player->RemovePet(oldPet, PetSaveMode(PET_SAVE_FIRST_STABLE_SLOT + std::distance(petStable->StabledPets.begin(), stabledPet)));
    }
    else if (petStable->UnslottedPets.size() == 1)
    {
        if (petStable->CurrentPet || !petStable->UnslottedPets[0].Health || petStable->UnslottedPets[0].Type != HUNTER_PET)
        {
            SendPetStableResult(StableResult::InternalError);
            return;
        }

        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHAR_PET_SLOT_BY_ID);
        stmt->setUInt8(0, PetSaveMode(PET_SAVE_FIRST_STABLE_SLOT + std::distance(petStable->StabledPets.begin(), stabledPet)));
        stmt->setUInt32(1, _player->GetGUID().GetCounter());
        stmt->setUInt32(2, petStable->UnslottedPets[0].PetNumber);
        CharacterDatabase.Execute(stmt);

        // move unsummoned pet into CurrentPet slot so that it gets moved into stable slot later
        petStable->CurrentPet = std::move(petStable->UnslottedPets.back());
        petStable->UnslottedPets.pop_back();
    }
    else if (petStable->CurrentPet || !petStable->UnslottedPets.empty())
    {
        SendPetStableResult(StableResult::InternalError);
        return;
    }

    Pet* newPet = new Pet(_player, HUNTER_PET);
    if (!newPet->LoadPetFromDB(_player, 0, petnumber, false))
    {
        delete newPet;

        petStable->UnslottedPets.push_back(std::move(*petStable->CurrentPet));
        petStable->CurrentPet.reset();

        // update current pet slot in db immediately to maintain slot consistency, dismissed pet was already saved
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHAR_PET_SLOT_BY_ID);
        stmt->setUInt8(0, PET_SAVE_NOT_IN_SLOT);
        stmt->setUInt32(1, _player->GetGUID().GetCounter());
        stmt->setUInt32(2, petnumber);
        CharacterDatabase.Execute(stmt);

        SendPetStableResult(StableResult::InternalError);
    }
    else
    {
        // update current pet slot in db immediately to maintain slot consistency, dismissed pet was already saved
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHAR_PET_SLOT_BY_ID);
        stmt->setUInt8(0, PET_SAVE_AS_CURRENT);
        stmt->setUInt32(1, _player->GetGUID().GetCounter());
        stmt->setUInt32(2, petnumber);
        CharacterDatabase.Execute(stmt);

        // we gotta re-arange the stabled pets.. because it breaks client side
        std::array<Optional<PetStable::PetInfo>, MAX_PET_STABLES> StabledPets;
        uint8 index = 0;

        for (const auto& pet : petStable->StabledPets)
        {
            if (!pet.has_value() || pet->PetNumber == petnumber)
                continue;

            StabledPets[index] = pet;
            ++index;
        }

        petStable->StabledPets = StabledPets;

        SendPetStableResult(StableResult::UnstableSuccess);
    }
}

void WorldSession::HandleBuyStableSlot(WorldPackets::NPC::BuyStableSlot& packet)
{
    TC_LOG_DEBUG("network", "WORLD: Recv CMSG_BUY_STABLE_SLOT.");
    ObjectGuid npcGUID = packet.StableMaster;

    if (!CheckStableMaster(npcGUID))
    {
        SendPetStableResult(StableResult::InternalError);
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    PetStable& petStable = GetPlayer()->GetOrInitPetStable();
    if (petStable.MaxStabledPets < MAX_PET_STABLES)
    {
        StableSlotPricesEntry const* SlotPrice = sStableSlotPricesStore.LookupEntry(petStable.MaxStabledPets + 1);
        if (_player->HasEnoughMoney(uint64(SlotPrice->Cost)))
        {
            ++petStable.MaxStabledPets;
            _player->ModifyMoney(-int32(SlotPrice->Cost));
            SendPetStableResult(StableResult::BuySlotSuccess);

            _player->SetUpdateFieldValue(_player->m_values.ModifyValue(&Player::m_activePlayerData).ModifyValue(&UF::ActivePlayerData::NumStableSlots), petStable.MaxStabledPets);
            _player->SendStable(packet.StableMaster);
        }
        else
            SendPetStableResult(StableResult::NotEnoughMoney);
    }
    else
        SendPetStableResult(StableResult::InvalidSlot);
}

void WorldSession::HandleRepairItemOpcode(WorldPackets::Item::RepairItem& packet)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_REPAIR_ITEM: Npc {}, Item {}, UseGuildBank: {}",
        packet.NpcGUID.ToString(), packet.ItemGUID.ToString(), packet.UseGuildBank);

    Creature* unit = GetPlayer()->GetNPCIfCanInteractWith(packet.NpcGUID, UNIT_NPC_FLAG_REPAIR, UNIT_NPC_FLAG_2_NONE);
    if (!unit)
    {
        TC_LOG_DEBUG("network", "WORLD: HandleRepairItemOpcode - {} not found or you can not interact with him.", packet.NpcGUID.ToString());
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    // reputation discount
    float discountMod = _player->GetReputationPriceDiscount(unit);

    if (!packet.ItemGUID.IsEmpty())
    {
        TC_LOG_DEBUG("network", "ITEM: Repair {}, at {}", packet.ItemGUID.ToString(), packet.NpcGUID.ToString());

        Item* item = _player->GetItemByGuid(packet.ItemGUID);
        if (item)
            _player->DurabilityRepair(item->GetPos(), true, discountMod);
    }
    else
    {
        TC_LOG_DEBUG("network", "ITEM: Repair all items at {}", packet.NpcGUID.ToString());
        _player->DurabilityRepairAll(true, discountMod, packet.UseGuildBank);
    }
}
