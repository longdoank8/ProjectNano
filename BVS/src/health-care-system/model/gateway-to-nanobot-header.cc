/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2005 INRIA
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Anke Küstner <anke.kuestner@ccs-labs.org>
 */

#include "gateway-to-nanobot-header.h"

namespace ns3 {
    NS_OBJECT_ENSURE_REGISTERED(GatewayToNanobotHeader);

    uint32_t GatewayToNanobotHeader::m_counter = 1;
    //default Constructor

    uint32_t
    GatewayToNanobotHeader::GetSeqNr(void) {
        return m_seq_nr;
    }

    void
    GatewayToNanobotHeader::SetSeqNr(void) {
        m_seq_nr = m_counter;
        m_counter++;
    }

    bool
    GatewayToNanobotHeader::GetToNanobot(void) {
        return m_to_nanobot;
    }

    void
    GatewayToNanobotHeader::SetToNanobot(bool ToNanobot) {
        m_to_nanobot = ToNanobot;
    }

    void
    GatewayToNanobotHeader::AddHopCount(void) {
        m_hop_count = m_hop_count + 1;
    }

    void
    GatewayToNanobotHeader::SetHopCount(uint32_t hop_count) {
        m_hop_count = hop_count;
    }

    uint32_t
    GatewayToNanobotHeader::GetHopCount() {
        return m_hop_count;
    }

    TypeId
    GatewayToNanobotHeader::GetTypeId(void) {
        static TypeId tid = TypeId("ns3::GatewayToNanobotHeader")
                .SetParent<Header> ()
                .SetGroupName("health-care-system")
                .AddConstructor<GatewayToNanobotHeader> ()
                ;
        return tid;
    }

    TypeId
    GatewayToNanobotHeader::GetInstanceTypeId(void) const {
        return GetTypeId();
    }

    void
    GatewayToNanobotHeader::Print(std::ostream &os) const {
        os << "Sequence number = " << m_seq_nr
                << " "
                << "To Nanobot = " << m_to_nanobot
                << " "
                << "Hop Count = " << m_hop_count
                ;
    }

    uint32_t
    GatewayToNanobotHeader::GetSerializedSize(void) const {
        return 9;
    }

    void
    GatewayToNanobotHeader::Serialize(Buffer::Iterator start) const {
        Buffer::Iterator i = start;
        i.WriteHtonU32(m_seq_nr);
        i.WriteU8(m_to_nanobot);
        i.WriteHtonU32(m_hop_count);
    }

    uint32_t
    GatewayToNanobotHeader::Deserialize(Buffer::Iterator start) {
        Buffer::Iterator i = start;
        m_seq_nr = i.ReadNtohU32();
        m_to_nanobot = i.ReadU8();
        m_hop_count = i.ReadNtohU32();

        return GetSerializedSize();
    }
}
