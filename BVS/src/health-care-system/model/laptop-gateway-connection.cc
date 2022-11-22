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
#include "laptop-gateway-connection.h"

namespace ns3 {

    TypeId
    LaptopGatewayConnection::GetTypeId(void) {
        static TypeId tid = TypeId("ns3::LaptopGatewayConnection")
                .SetParent<Object> ()
                .SetGroupName("Health-Care-System")
                ;
        return tid;
    }

    LaptopGatewayConnection::LaptopGatewayConnection() {
        //create a channel
        Ptr<SingleModelSpectrumChannel> communicationchannel = CreateObject<SingleModelSpectrumChannel> ();
        SetChannelProperties(communicationchannel);
        //create a laptop that gets attached to the channel
        m_laptop = CreateObject<Laptop> (Vector(0.0, 0.0, 8.0), communicationchannel, Mac16Address("00:01"));
        //create a gateway that gets attached to the channel 
        m_gateway = CreateObject<Gateway> (Vector(0.0, 0.0, 2.0), communicationchannel, Mac16Address("00:02"));
    }

    void
    LaptopGatewayConnection::SetChannelProperties(Ptr<SingleModelSpectrumChannel> channel) {
        //simulate propagation loss model and add it to channel
        Ptr<LogDistancePropagationLossModel> propModel = CreateObject<LogDistancePropagationLossModel> ();
        channel -> AddPropagationLossModel(propModel);

        //simulate a constant propagation speed and add it to channel
        Ptr<ConstantSpeedPropagationDelayModel> delayModel = CreateObject<ConstantSpeedPropagationDelayModel> ();
        channel -> SetPropagationDelayModel(delayModel);
    }

    Ptr<Laptop>
    LaptopGatewayConnection::GetLaptop() {
        return m_laptop;
    }

    Ptr<Gateway>
    LaptopGatewayConnection::GetGateway() {
        return m_gateway;
    }

}
