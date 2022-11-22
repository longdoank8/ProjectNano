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
#ifndef CLASS_LAPTOP_GATEWAY_CONNECTION_
#define CLASS_LAPTOP_GATEWAY_CONNECTION_

#include "ns3/core-module.h"
#include "ns3/gateway.h"
#include "ns3/laptop.h"
#include "ns3/single-model-spectrum-channel.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/propagation-delay-model.h"

namespace ns3 {
    //forward declaration
    class LaptopGatewayConnection;

    class LaptopGatewayConnection : public ns3::Object {
    public:
        static TypeId GetTypeId(void);

        /**
         * \brief Constructor
         */
        LaptopGatewayConnection();
        ~LaptopGatewayConnection() = default;

        /**
         * get gatway attached to this connection
         * @return 
         */
        Ptr<Gateway> GetGateway();
        /**
         * get laptop attached to this connection
         * @return 
         */
        Ptr<Laptop> GetLaptop();


    private:

        Ptr<Laptop> m_laptop;
        Ptr<Gateway> m_gateway;

        /**
         * Set channel properties
         * @param channel properties should be added to this channel
         */
        void SetChannelProperties(Ptr<SingleModelSpectrumChannel> channel);

    };


}


#endif
