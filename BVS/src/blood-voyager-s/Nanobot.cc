/* -*-  Mode: C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2017 Universität zu Lübeck [GEYER]
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * Author: Regine Geyer <geyer@itm.uni-luebeck.de>
 */

#include "Nanobot.h"

using namespace std;
namespace ns3 {
    // TypeId Nanobot::GetTypeId (void)
    // {
    //   static TypeId tid = TypeId ("ns3::Nanobot")
    //   .SetParent<Object> ()
    //   .AddConstructor<Nanobot> ()
    //   ;
    //   return tid;
    // }   
    //    

    Nanobot::Nanobot(u_int32_t nano_id, Vector initialPosition) {
        m_node = CreateObject<Node> ();
        //enables mobility
        MobilityHelper mobility;
        mobility.Install(m_node);
        SetPosition(initialPosition);

        m_nanobotID = nano_id;
        m_length = 0.00001; //100nm
        m_width = 0.00001; //100nm
        m_stream_nb = 0;
        //nanobot is created upon injection -> should change marks if nanobot should change the stream -> upon injection it should not change
        m_shouldChange = false;
        m_timeStep = Seconds(0.0);
        m_hop_count = 0;

        m_function = 0;
        m_body_area = 0;
        m_received_a_packet = false;
        m_visited_gateway = 0;

        m_last_received_packet = NULL;

    }

    // is second constructor necessary?
    Nanobot::Nanobot(uint16_t function, uint16_t body_area) {
        m_node = CreateObject<Node> ();
        //enables mobility
        MobilityHelper mobility;
        mobility.Install(m_node);

        m_length = 0.00001; //100nm
        m_width = 0.00001; //100nm
        m_stream_nb = 0;
        m_shouldChange = false;
        m_timeStep = Seconds(0.0);
        m_hop_count = 0;
        m_visited_gateway = 0;
        m_function = function;
        m_body_area = body_area;
    }

    Nanobot::~Nanobot() {
    }

    // move to stream class?
    bool
    Nanobot::Compare(Ptr<Nanobot> v1, Ptr<Nanobot> v2) {
        if (v1->GetNanobotID() < v2->GetNanobotID()) {
            return true;
        } else {
            return false;
        }
    }

    // maybe move to nano communication class
    void
    Nanobot::CopyPacketFromGateway() {
        Ptr<Packet> packet = Gateway::GetLatestPacket() ->Copy();
        GatewayToNanobotHeader g;
        packet->RemoveHeader(g);
        this->SetHopCount(g.GetHopCount());
        NS_LOG_UNCOND("HopCount before:" << g.GetHopCount());
        g.AddHopCount();
        NS_LOG_UNCOND("HopCount after:" << g.GetHopCount());
        packet->AddHeader(g);
        this->SetNewPacket(packet);

        NS_LOG_UNCOND("Received packet of size " << packet->GetSize());
        m_received_a_packet = true;
    }
    //maybe move to nano communication class
    void
    Nanobot::DisassemblePacket() {
        Ptr<Packet> packet = m_last_received_packet->Copy();
        GatewayToNanobotHeader g;
        packet->RemoveHeader(g);

        HealthCareHeader h;
        packet->PeekHeader(h);
        if (h.GetStartNanobotId() <= m_nanobotID && h.GetEndNanobotId() >= m_nanobotID) {
            NS_LOG_UNCOND("I am in the ID range: " << this->GetNanobotID());
            if (h.GetFunction() == m_function) {
                NS_LOG_UNCOND("I don't have the right function: " << this->GetNanobotFunction());

                if (h.GetBodyArea() == m_body_area) {
                    NS_LOG_UNCOND("I am in the right body area: " << this->GetNanobotBodyArea());

                    switch (h.GetTask()) {
                        case 0: RandomTask0(this);
                            break;
                        case 1: RandomTask1(this);
                            break;
                        case 2: RandomTask2(this);
                            break;
                        default:
                            NS_LOG_UNCOND("Invalid Task");
                    }

                } else {
                    NS_LOG_UNCOND("I am not in the right body area: " << this->GetNanobotBodyArea());
                }
            } else {
                NS_LOG_UNCOND("I don't have the right function: " << this->GetNanobotFunction());
            }
        } else {
            NS_LOG_UNCOND("I am not in the ID range: " << this->GetNanobotID());
        }

    }

   void
    Nanobot::CopyPacketToNanobot(Ptr<Nanobot> bot) {
        Ptr<Packet> packet = (this->GetLastReceivedPacket())->Copy();
        GatewayToNanobotHeader g;
        packet->PeekHeader(g);
        //if Hop Count is less or equals....
        if(g.GetHopCount() <= 10){
            NS_LOG_UNCOND("HopCount before:" << g.GetHopCount());
            NS_LOG_UNCOND("sequence number " << g.GetSeqNr());
            packet->RemoveHeader(g);
            bot->SetHopCount(g.GetHopCount());
            g.AddHopCount();
            NS_LOG_UNCOND("HopCount after:" << g.GetHopCount());
            packet->AddHeader(g);
            bot->SetNewPacket(packet);
            bot->SetReceivedPacket();
        }
        else {
            bot->SetNewPacket(NULL);
        }
    }

    void
    Nanobot::RandomTask0(Ptr<Nanobot> bot) {
        NS_LOG_UNCOND("Random Task 0 was executed");

        Ptr<Packet> packet = Create<Packet> ();
        HealthCareHeader healthcareHeader(bot->GetNanobotID(), bot->GetNanobotID(), bot->GetNanobotFunction(), 0, bot->GetNanobotBodyArea(), 0);

        packet->AddHeader(healthcareHeader);

        GatewayToNanobotHeader g;
        g.SetSeqNr();
        g.SetToNanobot(false);

        packet->AddHeader(g);
    }

    void
    Nanobot::RandomTask1(Ptr<Nanobot> bot) {
        NS_LOG_UNCOND("Random Task 1 was executed");

        Ptr<Packet> packet = Create<Packet> ();
        HealthCareHeader healthcareHeader(bot->GetNanobotID(), bot->GetNanobotID(), bot->GetNanobotFunction(), 0, bot->GetNanobotBodyArea(), 1);

        packet->AddHeader(healthcareHeader);

        GatewayToNanobotHeader g;
        g.SetSeqNr();
        g.SetToNanobot(false);

        packet->AddHeader(g);
    }

    void
    Nanobot::RandomTask2(Ptr<Nanobot> bot) {
        NS_LOG_UNCOND("Random Task 2 was executed");

        Ptr<Packet> packet = Create<Packet> ();
        HealthCareHeader healthcareHeader(bot->GetNanobotID(), bot->GetNanobotID(), bot->GetNanobotFunction(), 0, bot->GetNanobotBodyArea(), 2);

        packet->AddHeader(healthcareHeader);

        GatewayToNanobotHeader g;
        g.SetSeqNr();
        g.SetToNanobot(false);

        packet->AddHeader(g);
    }

    uint16_t
    Nanobot::GetNanobotBodyArea() {
        return m_body_area;
    }

    void
    Nanobot::SetNanobotBodyArea(uint16_t value) {
        m_body_area = value;
    }

    uint16_t
    Nanobot::GetNanobotFunction() {
        return m_function;
    }

    void
    Nanobot::SetNanobotFunction() {
        m_function = rand() % 3;
    }

    int
    Nanobot::GetNanobotID() {
        return m_nanobotID;
    }

    void
    Nanobot::SetNanobotID(int value) {
        m_nanobotID = value;

    }

    double
    Nanobot::GetLength() {
        return m_length;
    }

    void
    Nanobot::SetLength(double value) {
        if (value < 0) {
            value = 0;
        }
        m_length = value;
    }

    double
    Nanobot::GetWidth() {
        return m_width;
    }

    void
    Nanobot::SetWidth(double value) {
        if (value < 0) {
            value = 0;
        }
        m_width = value;
    }

    void
    Nanobot::SetStream(int value) {
        m_stream_nb = value;
    }

    int
    Nanobot::GetStream() {
        return m_stream_nb;
    }

    bool
    Nanobot::GetShouldChange() {
        return m_shouldChange;
    }

    void
    Nanobot::SetShouldChange(bool value) {
        m_shouldChange = value;
    }

    ns3::Time
    Nanobot::GetTimeStep() {
        return m_timeStep;
    }

    void
    Nanobot::SetTimeStep() {
        m_timeStep = Simulator::Now();
    }

    Vector
    Nanobot::GetPosition() {
        return m_node->GetObject<MobilityModel> ()->GetPosition();
    }

    void
    Nanobot::SetPosition(Vector value) {
        // std::cout << "SetPosition() " << (value.x * 100) << "," << (value.y*100) << "," << (value.z*100) << std::endl;
        m_node->GetObject<MobilityModel> ()->SetPosition(value);
    }
//--------------------------- Maybe move to nano communication class
    Ptr<Packet>
    Nanobot::GetLastReceivedPacket() {
        return m_last_received_packet;
    }

    void
    Nanobot::SetLastReceivedPacket(Ptr<Packet> packet) {
        m_last_received_packet = packet;
    }

    void
    Nanobot::SetNewPacket(Ptr<Packet> packet) {
        m_new_packet = packet;
    }

    Ptr<Packet>
    Nanobot::GetNewPacket() {
        return m_new_packet;
    }

    uint32_t
    Nanobot::GetSeqNr(Ptr<Packet> packet) {
        Ptr<Packet> packetcopy = packet->Copy();
        GatewayToNanobotHeader g;
        packetcopy->PeekHeader(g);
        return g.GetSeqNr();
    }

    bool
    Nanobot::ToNanobot(Ptr<Packet> packet) {
        GatewayToNanobotHeader g;
        packet->PeekHeader(g);
        return g.GetToNanobot();
    }
//------------------------------------------
// maybe move to nano communication class
    void
    Nanobot::ComparePackets() {
        //is new packet set?
        if (this->GetNewPacket() != NULL) {
            //if true copy
            Ptr<Packet> newpacket = this->GetNewPacket()->Copy();
            //does a last received packet exist?
            if (this->GetLastReceivedPacket() != NULL) {
                //if so copy
                Ptr<Packet> latestpacket = this->GetLastReceivedPacket()->Copy();
                //compare the seqNr of both
                if (this->GetSeqNr(newpacket) > this->GetSeqNr(latestpacket)) {
                    //if newpacket has higher seqNr, set newpacket as last received packet and set flag that new packet is available
                    m_last_received_packet = newpacket->Copy();
                    m_received_new_packet = true;
                    m_sent_packet = false;

                } else {
                    m_received_new_packet = false;

                }

            }//if there is no last received packet, the newpacket is "newer"
            else {
                // set new packet as last received packet and set flag that new packet is available
                m_last_received_packet = newpacket->Copy();
                m_received_new_packet = true;
                m_sent_packet = false;

            }


        }// set flag for new packet on false
        else {
            m_received_new_packet = false;

        }
    }
//-------------------- maybe move to nano communication class
    bool
    Nanobot::CheckForNewPacket() {
        return m_received_new_packet;
    }

    bool
    Nanobot::CheckIfPacketWasSent() {
        return m_sent_packet;
    }

    void
    Nanobot::SetPacketAsSent() {
        m_sent_packet = true;
    }

    void
    Nanobot::SetReceivedPacket() {
        m_received_a_packet = true;
    }

    bool
    Nanobot::GetReceivedPacket() {
        return m_received_a_packet;
    }

    void
    Nanobot::ResetReceivedPacket() {
        m_received_a_packet = false;
    }

    void
    Nanobot::SetHopCount(uint32_t hop_count) {
        m_hop_count = hop_count;
    }

    uint32_t
    Nanobot::GetHopCount() {
        return m_hop_count;
    }
 //---------------------------------------------------------   
     void
    Nanobot::SetVisitedGateway(uint32_t round_number) {
        m_visited_gateway = round_number ;
    }

   uint32_t
    Nanobot::GetVisitedGateway() {
        return m_visited_gateway;
    }
   
    
} // namespace ns3
