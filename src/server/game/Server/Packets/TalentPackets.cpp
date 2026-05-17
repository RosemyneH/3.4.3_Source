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

#include "TalentPackets.h"

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Talent::TalentInfo const& talentInfo)
{
    data << uint32(talentInfo.TalentID);
    data << uint8(talentInfo.Rank);
    return data;
}

ByteBuffer& operator>>(ByteBuffer& data, WorldPackets::Talent::TalentInfo& talentInfo)
{
    data >> talentInfo.TalentID;
    data >> talentInfo.Rank;
    return data;
}

WorldPacket const* WorldPackets::Talent::UpdateTalentData::Write()
{
    _worldPacket << uint32(UnspentTalentPoints);
    _worldPacket << uint8(ActiveGroup);
    _worldPacket << uint32(TalentGroupInfos.size());

    for (auto const& talentGroupInfo : TalentGroupInfos)
    {
        _worldPacket << uint8(talentGroupInfo.Talents.size());
        _worldPacket << uint32(talentGroupInfo.Talents.size());

        if (IsPetTalents)
        {
            _worldPacket << uint8(0);
            _worldPacket << uint32(0);
        }
        else
        {
            _worldPacket << uint8(MAX_GLYPH_SLOT_INDEX);
            _worldPacket << uint32(MAX_GLYPH_SLOT_INDEX);
        }

        _worldPacket << uint8(talentGroupInfo.SpecID);

        for (TalentInfo talent : talentGroupInfo.Talents)
            _worldPacket << talent;

        if (!IsPetTalents)
        {
            for (uint16 glyph : talentGroupInfo.GlyphIDs)
                _worldPacket << glyph;
        }
    }

    _worldPacket.WriteBit(IsPetTalents);
    _worldPacket.FlushBits();

    return &_worldPacket;
}

void WorldPackets::Talent::LearnTalents::Read()
{
    Talents.resize(_worldPacket.ReadBits(6));
    for (uint32 i = 0; i < Talents.size(); ++i)
        _worldPacket >> Talents[i];
}

void WorldPackets::Talent::LearnTalent::Read()
{
    _worldPacket >> TalentID;
    _worldPacket >> RequestedRank;
}

void WorldPackets::Talent::LearnPetTalent::Read()
{
    _worldPacket >> PetGUID;
    _worldPacket >> TalentID;
    _worldPacket >> Rank;
}

void WorldPackets::Talent::LearnPetPreviewTalents::Read()
{
    _worldPacket >> PetGUID;
    Talents.resize(_worldPacket.read<uint32>());
    for (TalentInfo& talent : Talents)
        _worldPacket >> talent;
}

void WorldPackets::Talent::LearnPreviewTalents::Read()
{
    Talents.resize(_worldPacket.read<uint32>());
    for (TalentInfo& talent : Talents)
        _worldPacket >> talent;
}

WorldPacket const* WorldPackets::Talent::RespecWipeConfirm::Write()
{
    _worldPacket << int8(RespecType);
    _worldPacket << uint32(Cost);
    _worldPacket << RespecMaster;
    return &_worldPacket;
}

void WorldPackets::Talent::ConfirmRespecWipe::Read()
{
    _worldPacket >> RespecMaster;
    _worldPacket >> RespecType;
}

WorldPacket const* WorldPackets::Talent::LearnTalentFailed::Write()
{
    _worldPacket.WriteBits(Reason, 4);
    _worldPacket << int32(SpellID);
    _worldPacket << uint32(Talents.size());
    if (!Talents.empty())
        _worldPacket.append(Talents.data(), Talents.size());

    return &_worldPacket;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Talent::GlyphBinding const& glyphBinding)
{
    data << uint32(glyphBinding.SpellID);
    data << uint16(glyphBinding.GlyphID);
    return data;
}

WorldPacket const* WorldPackets::Talent::ActiveGlyphs::Write()
{
    _worldPacket << uint32(Glyphs.size());
    for (GlyphBinding const& glyph : Glyphs)
        _worldPacket << glyph;

    _worldPacket.WriteBit(IsFullUpdate);
    _worldPacket.FlushBits();

    return &_worldPacket;
}

void WorldPackets::Talent::RemoveGlyph::Read()
{
    _worldPacket >> GlyphSlot;
}
