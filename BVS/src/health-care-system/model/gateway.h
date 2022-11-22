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


//TODO Add all the stuff which is needed for communicating with nanobots
#ifndef CLASS_GATEWAY_
#define CLASS_GATEWAY_

#include "ns3/core-module.h"
#include "ns3/lr-wpan-module.h"
#include "ns3/mobility-helper.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/single-model-spectrum-channel.h"
#include "ns3/gateway-to-nanobot-header.h"
#include "ns3/header.h"
#include "ns3/packet.h"
#include <iostream>

namespace ns3 {

    class Gateway : public ns3::Object {
    public:
        static TypeId GetTypeId(void);
        /**
         * \brief Constructor
         */
        Gateway(Vector v, Ptr<SingleModelSpectrumChannel> channel, Mac16Address m);
        ~Gateway() = default;

        /**
         * Get LrWpanNetDevice attached to gateway
         * @return 
         */
        Ptr<LrWpanNetDevice> GetLrWpanDevice(void) const;
        
        /**
         * Get latest received packet
         * @return 
         */
        static Ptr<Packet> GetLatestPacket(void);

        /**
         * Check Flag that signals if gateway received a packet by smartdevice
         * @return true if there is a packet 
         */
        static bool CheckForPacket(void);

        /**
         * Reset Flag that signals if gateway received a packet by smartedevice
         */
        static void ResetPacketFlag(void);

        /**
         * Get if Nanobots have received the message
         * @return true if received
         */
        static bool NbsReceivedMessage();

        /**
         * reset the flag that bots received message
         */
        static void ResetNbsReceivedMessage();

        /**
         * set flag that bots received message
         */
        static void SetNbsReceivedMessage();

        /**
         * Copy Packet from nanobots
         * @param packet packet to copy
         */
        void CopyFromNanobot(Ptr<Packet> packet);

        /**
         * Send packet to laptop
         * @param packet packet to sent to smartdevice 
         */
        void SendPacketToLaptop(Ptr<Packet> packet);



    private:
        Ptr<Node> m_node; // Gateway has a node
        Ptr<LrWpanNetDevice> m_lrwpan_netdevice;

        //TODO make this to none static if there is more than one gateway
        static Ptr<Packet> m_latest_packet;
        static bool m_packet_here;

        uint32_t m_last_seen_sequence_number;

        static bool m_nanobots_received_message;

        /**
         * Set LrWpanNetDevice
         * @param channel channel the device should be attached to 
         * @param m mac address that should be attached to the device
         */
        void SetLrWpanDeviceProperties(Ptr<SingleModelSpectrumChannel> channel, Mac16Address m);

        /**
         * set mobility model and position for gateway
         * @param v position of gateway
         */
        void SetMobilityAndPosition(Vector v);
        /**
         * \brief if mcpsdataindicationcallback is triggered this function is called
         *
         * This method adds another header to the packet containing a sequence number
         * for the nanobots to know if its an old or a new packet
         */
        void SetMacCallback(void);

        //TODO make this to none static
        /**
         * Callback for Data Indication SAP
         * @param params 
         * @param p packet to be traced
         */
        static void DataIndication(McpsDataIndicationParams params, Ptr<Packet> p);

        /**
         * Callback for Data Confirmation
         * @param params
         */
        static void DataConfirm(McpsDataConfirmParams params);

    };
}

#endif
