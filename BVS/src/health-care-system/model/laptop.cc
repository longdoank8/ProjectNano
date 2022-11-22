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


#include "laptop.h"

namespace ns3 {

    TypeId
    Laptop::GetTypeId(void) {
        static TypeId tid = TypeId("ns3::Laptop")
                .SetParent<Object> ()
                .SetGroupName("Health-Care-System")
                ;
        return tid;
    }

    Laptop::Laptop(Vector v, Ptr<SingleModelSpectrumChannel> channel, Mac16Address m) {
        m_node = CreateObject<Node> ();
        m_lrwpan_netdevice = CreateObject<LrWpanNetDevice> ();
        SetMobilityAndPosition(v);
        SetLrWpanDeviceProperties(channel, m);
        m_node -> AddDevice(m_lrwpan_netdevice);
    }

    void
    Laptop::SetMobilityAndPosition(Vector v) {
        MobilityHelper mobility;
        mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
        mobility.Install(m_node);
        m_node -> GetObject<MobilityModel> ()-> SetPosition(v);
        SetMacCallback();
    }

    void
    Laptop::SetLrWpanDeviceProperties(Ptr<SingleModelSpectrumChannel> channel, Mac16Address m) {
        m_lrwpan_netdevice->SetChannel(channel);
        m_lrwpan_netdevice->SetAddress(m);
    }

    Ptr<LrWpanNetDevice>
    Laptop::GetLrWpanDevice() const {
        return m_lrwpan_netdevice;
    }

    /**
     * For tracing packet exchange
     */

    void
    Laptop::SetMacCallback() {
        McpsDataIndicationCallback dataIndication_cb;
        dataIndication_cb = MakeCallback(&DataIndication);
        m_lrwpan_netdevice->GetMac()->SetMcpsDataIndicationCallback(dataIndication_cb);

        McpsDataConfirmCallback dataConfirm_cb;
        dataConfirm_cb = MakeCallback(&DataConfirm);
        m_lrwpan_netdevice->GetMac()->SetMcpsDataConfirmCallback(dataConfirm_cb);
    }

    void
    Laptop::DataIndication(McpsDataIndicationParams params, Ptr<Packet> p) {
        NS_LOG_UNCOND("Received packet of size " << p->GetSize());
    }

    void
    Laptop::DataConfirm(McpsDataConfirmParams params) {
        NS_LOG_UNCOND("LrWpanMcpsDataConfirmStatus = " << params.m_status);
    }

}
