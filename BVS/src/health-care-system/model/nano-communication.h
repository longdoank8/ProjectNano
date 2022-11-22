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

#ifndef CLASS_NANO_COMMUNICATION_
#define CLASS_NANO_COMMUNICATION_

#include "ns3/core-module.h"
#include "ns3/blood-voyager-s-module.h"
#include "ns3/simulator.h"
#include "ns3/nstime.h"

namespace ns3 {
    class NanoCommunication;

    class NanoCommunication: public ns3::Object {
    public:
        static TypeId GetTypeId(void);
        /**
         * @brief Constructor 
         * 
         */
        NanoCommunication(Bloodcircuit circuit);
        ~NanoCommunication() = default; 

        void ScheduleMessageRoutines();

    private:
        map<int, list<Ptr<Nanobot>>> m_nanobotPositionInfo;
        Bloodcircuit m_bloodcircuit;
        double m_delta;

        /**
         * update the map containing information on position of every bot
         */
        void UpdateNanobotPositionInfo();
        /**
         * Call for message routine of nanobots
         */
        void MessageRoutine();
        /**
         * @brief Copy Message among nanobots 
         * 
         * @param nb bot to copy from
         * @param circuitMap circuitMap containing all bloodvessels
         * @param m_nanobotPositionInfo map containing the positions of all nanobots
         */
        void CopyMessage(Ptr<Nanobot> nb);
        /**
         * trigger disassembling of new packet
         * @param nb
         */
        void NewPacket(Ptr<Nanobot> nb);

        /**
         * Routines for nanobots being in the left half of the heart
         * @param nb
         */
        void LeftHeart(Ptr<Nanobot> nb);

        bool InRangeToEachOther(Ptr<Nanobot> sendingBot, Ptr<Nanobot> receivingBot);

        bool SameBot(Ptr<Nanobot> sendingBot, Ptr<Nanobot> receivingBot);

        bool AllowedToSend(Ptr<Nanobot> sendingBot, Ptr<Nanobot> receivingBot);
    };

    
}
#endif
