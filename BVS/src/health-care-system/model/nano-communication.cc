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
 * Author: Anke Küstner <anke.kuestner@ccs-labs.org>
 */
#include "ns3/simulator.h"
#include "nano-communication.h"

using namespace ns3;
    TypeId
    NanoCommunication::GetTypeId(void) {
        static TypeId tid = TypeId("ns3::NanoCommunication")
                .SetParent<Object> ()
                .SetGroupName("Health-Care-System")
                ;
        return tid;
    }

    NanoCommunication::NanoCommunication(Bloodcircuit circuit){
        m_bloodcircuit = circuit;
        m_delta = 1;
    }

    void
    NanoCommunication::ScheduleMessageRoutines(){
        this->UpdateNanobotPositionInfo();
        this->MessageRoutine();
    }

    void
    NanoCommunication::UpdateNanobotPositionInfo(){
        if (m_nanobotPositionInfo.empty()) {
            // add an entry for every vessel id in the map with the list of Nanobots
            for(const auto& indexVesselPair : m_bloodcircuit.GetBloodcircuit())
            {
                m_nanobotPositionInfo.insert(std::pair<int, list<Ptr < Nanobot>>>(indexVesselPair.first, indexVesselPair.second->GetNanobotList()));
            }
        } else {
            //erase old entry and insert new entry for list of Nanobots
            for(const auto& indexVesselPair : m_bloodcircuit.GetBloodcircuit())
            {
                m_nanobotPositionInfo.erase(indexVesselPair.first);
                m_nanobotPositionInfo.insert(std::pair<int, list<Ptr < Nanobot>>>(indexVesselPair.first, indexVesselPair.second->GetNanobotList()));
            }
        }
    }

    void
    NanoCommunication::MessageRoutine(){
        // if information on nanobot position is not initialized just return
        if (m_nanobotPositionInfo.empty()) {
            return;
        } else {
            //for every vessel in this circuit
            for(const auto& indexVesselPair : m_bloodcircuit.GetBloodcircuit())
            {
                //for every bot in this vessel
                for (const Ptr<Nanobot> &nb : m_nanobotPositionInfo[indexVesselPair.first]) {
                    //check if vessel has ID=2 (left half of heart), vessel id 29 for left forearm
                    if (indexVesselPair.first == 2) {
                        LeftHeart(nb);
                    }
                    nb->ComparePackets();
                    //check which packet is newer
                    if (nb->CheckForNewPacket()) {
                        NS_LOG_UNCOND("I received a new packet");
                        NewPacket(nb);
                    }
                    CopyMessage(nb);
                }
            }
            //only reset flags if gateway got feedback that packet was received by at least one nanobot
            if (Gateway::NbsReceivedMessage()) {
                Gateway::ResetPacketFlag();
                Gateway::ResetNbsReceivedMessage();
            }
        }
    }

    void
    NanoCommunication::CopyMessage(Ptr<Nanobot> sendingBot) {
        //check if nanobot has already received a packet and checked if it has a valid seq
        if (sendingBot->GetLastReceivedPacket() != NULL) {
            //check if nanobot has already send the message
            if (!(sendingBot->CheckIfPacketWasSent())) {
                //test if there are bots in the range of 1 cm around the nanobot who has a message
                for (const auto& indexVesselPair : m_bloodcircuit.GetBloodcircuit())
                {
                    for (const Ptr<Nanobot> &receivingBot : m_nanobotPositionInfo[indexVesselPair.first]) {
                        if (InRangeToEachOther(sendingBot, receivingBot)) {
                            //don't let nanobots send to themselves 
                            if (SameBot(sendingBot, receivingBot)) { 
                                
                                if (AllowedToSend(sendingBot, receivingBot)) {
                                    sendingBot->CopyPacketToNanobot(receivingBot);
                                    NS_LOG_UNCOND("Bot to copy to:" << receivingBot->GetPosition());
                                    NS_LOG_UNCOND("Bot to copy from:" << sendingBot->GetPosition());
                                    sendingBot->SetPacketAsSent();
                                }
                            }
                        }
                    }
                }//if we want nanobots to be able to just send their received packet once, than nb->SetPacketAsSent(); has to be here
            }
        }
    }

    void
    NanoCommunication::LeftHeart(Ptr<Nanobot> nb) {      
        if (Gateway::CheckForPacket()) {
            nb->CopyPacketFromGateway();
            NS_LOG_UNCOND("My Nanobot-ID is " << nb->GetNanobotID());
            NS_LOG_UNCOND("I'm in vessel " << nb->GetNanobotBodyArea());

            //Feedback for Gateway, there is a nanobot which received the message from the gateway
            Gateway::SetNbsReceivedMessage();
        }
    }

    void
    NanoCommunication::NewPacket(Ptr<Nanobot> nb) {
        //  
        if (nb->ToNanobot(nb->GetLastReceivedPacket()) == true) {
            nb->DisassemblePacket();
        } else {
            NS_LOG_UNCOND("Packet destination is not Nanobot");
        }
    }

    bool 
    NanoCommunication::InRangeToEachOther(Ptr<Nanobot> sendingBot, Ptr<Nanobot> receivingBot) {
        return (receivingBot->GetPosition().x <= sendingBot->GetPosition().x + 0.01 
                && receivingBot->GetPosition().x >= sendingBot->GetPosition().x - 0.01 
                && receivingBot->GetPosition().y <= sendingBot->GetPosition().y + 0.01 
                && receivingBot->GetPosition().y >= sendingBot->GetPosition().y - 0.01
                && receivingBot->GetPosition().z <= sendingBot->GetPosition().z + 0.01 
                && receivingBot->GetPosition().z >= sendingBot->GetPosition().z - 0.01);
    }

    bool
    NanoCommunication::SameBot(Ptr<Nanobot> sendingBot, Ptr<Nanobot> receivingBot){
        return (sendingBot->GetNanobotID() != receivingBot->GetNanobotID()); 
    }

    bool
    NanoCommunication::AllowedToSend(Ptr<Nanobot> sendingBot, Ptr<Nanobot> receivingBot){
        //check if bot has already received a packet and if so check if the packet is newer than my own    
        return (receivingBot->GetLastReceivedPacket() == NULL 
                || sendingBot->GetSeqNr(sendingBot->GetLastReceivedPacket()) > receivingBot->GetSeqNr(sendingBot->GetLastReceivedPacket()));
    }

