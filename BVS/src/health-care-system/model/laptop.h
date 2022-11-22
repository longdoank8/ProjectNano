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
#ifndef CLASS_LAPTOP_
#define CLASS_LAPTOP_

#include "ns3/core-module.h"
#include "ns3/lr-wpan-module.h"
#include "ns3/mobility-helper.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/single-model-spectrum-channel.h"
namespace ns3 {
    class Laptop; //forward declaration

    class Laptop : public ns3::Object {
    public:
        static TypeId GetTypeId(void);
        /**
         * \brief Constructor
         */
        Laptop(Vector v, Ptr<SingleModelSpectrumChannel> channel, Mac16Address m);
        ~Laptop() = default;
        /**
         * get the LrWpanNetDevice attached to smartdevice
         * @return 
         */
        Ptr<LrWpanNetDevice> GetLrWpanDevice() const;


    private:
        Ptr<Node> m_node; // Gateway has a node
        Ptr<LrWpanNetDevice> m_lrwpan_netdevice;

        /**
         * set properties of lrwpannetdevice
         * @param channel channel the netdevice should bet attached to
         * @param m mac address for the netdevice
         */
        void SetLrWpanDeviceProperties(Ptr<SingleModelSpectrumChannel> channel, Mac16Address m);

        /**
         * set mobility and position of smartdevice
         * @param v position of laptop
         */
        void SetMobilityAndPosition(Vector v);

        /**
         * set mac callback
         */
        void SetMacCallback();

        /**
         * Callback for Data Indication
         * @param params 
         * @param p
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
