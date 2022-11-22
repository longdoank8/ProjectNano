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

#ifndef CLASS_NANOBOT_
#define CLASS_NANOBOT_

#include "ns3/object.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/gateway.h"
#include "ns3/packet.h"
#include "ns3/gateway-to-nanobot-header.h"
#include "ns3/header.h"
#include "ns3/health-care-header.h"


//#include <list>
#include <iostream>
#include <unistd.h>
using namespace std;
namespace ns3 {
    class Nanobot;

    /**
     * \brief Nanobot is a mobile Object.
     *
     * Each Nanobot has a dimension [length and width] and can be positioned by a Vector.
     * The position of the Nanobot is the position of its node which it owns.
     * A Nanobot belongs to a particular stream of a specific Bloodvessel.
     * Its current velocity depends on the stream it is in. Nanobots are managed by bloodvessels.
     */
    //enum class task
    //		 {
    //                                random_task_0 = 0,
    //		 		random_task_1 = 1,
    //		 		random_task_2 = 2
    //		 };

    class Nanobot : public ns3::Object {
    private:
        uint32_t m_nanobotID; // nanobot's id
        double m_length; // nanobot's length.
        double m_width; // nanobot's width.
        int m_stream_nb; // nanobot's stream.
        uint16_t m_function; // nanobot's function
        uint16_t m_body_area; // nanobot's need to know where they currently are
        uint32_t m_hop_count; // hop count when packet arrives at nanobots

        bool m_received_new_packet;
        bool m_sent_packet; // set to true if packet was sent
        bool m_received_a_packet; //marker for csv
        uint32_t m_visited_gateway;//round counter for nanobot 


        Ptr<Node> m_node; // nanobot has a node - a node has a position

        bool m_shouldChange; // nanobots change setting - if true change streams
        ns3::Time m_timeStep; // simulatortime of the last change of the nanobot

        Ptr<Packet> m_last_received_packet; // last packet that was received by gateway
        Ptr<Packet> m_new_packet; //newest received packet




        static void RandomTask0(Ptr<Nanobot> bot); /// random task 0 to accomplish
        static void RandomTask1(Ptr<Nanobot> bot); /// random task 1 to accomplish
        static void RandomTask2(Ptr<Nanobot> bot); /// random task 2 to accomplish



    public:
        //static TypeId GetTypeId (void);

        /// Constructor to initialize values of all variables.
        /// Length an width are set to 100nm and the Nanobot does not change streams by default.
        /// The NanobotID is set in bloodcircuit, were the Nanobots are initialised.
        Nanobot(u_int32_t nano_id,Vector initialPosition);

        /// Second constructor where also function and body area are set. 
        Nanobot(uint16_t function, uint16_t m_body_area);
        /// Destructor [does nothing].
        ~Nanobot();

        /// Compare is used for the purpose of sorting nanobots based on their ID in a list or a queue.
        /// returns true if the ID of v1 is smaller than v2's ID.
        static bool Compare(Ptr<Nanobot> v1, Ptr<Nanobot> v2);


        /// check if packet is new, send to all other nanobots and if packet is addressed to this nanobot, if so call one of the three random tasks  
        void DisassemblePacket(); //list<Ptr<Nanobot>> nanobots);

        void CopyPacketToNanobot(Ptr<Nanobot> bot);

        ///CopyPacketFromGateway is to Copy a Packet that a gateway received to the nanobots
        void CopyPacketFromGateway();

        ///Getter and setter methods

        /**
         *\returns the vessel id the nanobot currently is in
         */
        uint16_t GetNanobotBodyArea();

        /**
         *\param value the vessel id the nanobot currently is in
         */
        void SetNanobotBodyArea(uint16_t value);

        /**
         * \returns the function id of the nanobot
         */
        uint16_t GetNanobotFunction();

        /**
         * sets the nanobots function
         */
        void SetNanobotFunction();

        /**
         * \returns the Nanobot Id.
         */
        int GetNanobotID();

        /**
         * \param value a Nanobot Id.
         *
         * A Nanobot has an unique Id.
         */
        void SetNanobotID(int value);

        /**
         * \returns the length of the Nanobot.
         */
        double GetLength();

        /**
         * \param value the length of the Nanobot.
         */
        void SetLength(double value);

        /**
         * \returns the width of the Nanobot.
         */
        double GetWidth();

        /**
         * \param value the stream of the Nanobot.
         */
        void SetStream(int value);

        /**
         * \returns the stream of the Nanobot.
         */
        int GetStream();

        /**
         * \param value the width of the Nanobot.
         */
        void SetWidth(double value);

        /**
         * \returns true if the Nanobot should change
         */
        bool GetShouldChange();

        /**
         * \param bool if nanobot should change.
         */
        void SetShouldChange(bool value);

        /**
         * \returns the time of the last change of Nanobots position
         */
        ns3::Time GetTimeStep();

        /**
         * \sets the time of the last change in position.
         */
        void SetTimeStep();

        /**
         * \returns the position of Nanobot's Node which is located at the center back of the Nanobot.
         */
        Vector GetPosition();

        /**
         * \param value a position Vector.
         *
         * This function sets the position of Nanobot's Node. Nanobot's position is its Node's position.
         * The position Vector must point to the center back of the Nanobot.
         */
        void SetPosition(Vector value);

        /**
         * returns last received packet
         * @return 
         */
        Ptr<Packet> GetLastReceivedPacket();

        /**
         * sets last received packet
         * @param packet
         */
        void SetLastReceivedPacket(Ptr<Packet> packet);

        /**
         * returns new packet
         * @return 
         */
        Ptr<Packet> GetNewPacket();

        /**
         * sets new packet
         * @param packet
         */
        void SetNewPacket(Ptr<Packet> packet);

        /**
         * gets the seq number of a packet
         * @param packet
         * @return seq number attached to the packet
         */
        uint32_t GetSeqNr(Ptr<Packet> packet);

        /**
         * gets the value of the ToNanobot Tag of the header
         * @param packet
         * @return 
         */
        bool ToNanobot(Ptr<Packet> packet);

        /**
         * compares new packet and last received packet
         */
        void ComparePackets();

        /**
         * checks if there is a newly received packet
         * @return 
         */
        bool CheckForNewPacket();

        /**
         * checks if packet was already sent
         * @return 
         */
        bool CheckIfPacketWasSent();

        /**
         * set that packet was sent before
         */
        void SetPacketAsSent();

        /**
         * set the received packet
         */
        void SetReceivedPacket();

        /**
         * return received packet
         * @return 
         */
        bool GetReceivedPacket();

        /**
         * reset received packet
         */
        void ResetReceivedPacket();

        /**
         * set the hop count of the packet
         * @param hop_count
         */
        void SetHopCount(uint32_t hop_count);

        /**
         * returns hop count
         * @return 
         */
        uint32_t GetHopCount();
        
        /**
         * set the round number of nanobot
         */
       void SetVisitedGateway(uint32_t round_number);

        /**
         * return round number of nanobot
         * @return 
         */
       uint32_t GetVisitedGateway();

    };
}; // namespace ns3
#endif
