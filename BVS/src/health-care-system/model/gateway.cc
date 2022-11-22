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
 * Author:
 */
//TODO Add all the stuff which is needed for communicating with nanobots


#include "gateway.h"

namespace ns3
{
	TypeId
	Gateway::GetTypeId (void)
	{
		static TypeId tid = TypeId ("ns3::Gateway")
			.SetParent<Object> ()
			.SetGroupName ("Health-Care-System")
		;
		return tid;
	}

	Ptr<Packet> Gateway::m_latest_packet = NULL;
        bool Gateway::m_packet_here = false;
        bool Gateway::m_nanobots_received_message = false;

        /**
         * TODO:
         * EITHER create two positions for the same node 
         *      -> one representing the value for bvs, one representing the value in ns3
         * OR create new class called bvs-vector to define specific vectors
         * OR scale down BloodVoyager Values 
         * 
         */
	Gateway::Gateway (Vector v, Ptr<SingleModelSpectrumChannel> channel, Mac16Address m)
	{
		m_node = CreateObject<Node> ();
		m_lrwpan_netdevice = CreateObject<LrWpanNetDevice> ();
		SetMobilityAndPosition (v);
		SetLrWpanDeviceProperties (channel, m);
                m_last_seen_sequence_number = 0;
                //add device to node
		m_node -> AddDevice (m_lrwpan_netdevice);
                //set callbacks for tracing the packet
		SetMacCallback ();
	}

	void
	Gateway::SetMobilityAndPosition(Vector v)
	{
		MobilityHelper mobility;
                //position should not change
		mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
		mobility.Install (m_node);
                // set position of node to v
		m_node -> GetObject<MobilityModel> ()-> SetPosition (v);
	}

	void
	Gateway::SetLrWpanDeviceProperties (Ptr<SingleModelSpectrumChannel> channel, Mac16Address m)
	{
		m_lrwpan_netdevice->SetChannel (channel);
		m_lrwpan_netdevice->SetAddress (m);
	}

	Ptr<LrWpanNetDevice>
	Gateway::GetLrWpanDevice (void) const
	{
		return m_lrwpan_netdevice;
	}

	Ptr<Packet>
	Gateway::GetLatestPacket (void)
	{
		return m_latest_packet;
	}

        bool
        Gateway::CheckForPacket(void)
        {
                return m_packet_here;
        }
	/**
	*For tracing packet exchange
	*/
	void
	Gateway::SetMacCallback (void)
	{
                //callback if packet is received by receiver
		McpsDataIndicationCallback dataIndication_cb;
		dataIndication_cb = MakeCallback(&DataIndication);
                //if packet gets received by MAC and everthing is okay DataIndication Callback is called
		m_lrwpan_netdevice->GetMac ()->SetMcpsDataIndicationCallback (dataIndication_cb);

                //callback if packet gets confirmes
		McpsDataConfirmCallback dataConfirm_cb;
		dataConfirm_cb = MakeCallback(&DataConfirm);
		m_lrwpan_netdevice->GetMac ()->SetMcpsDataConfirmCallback (dataConfirm_cb);
	}

        void
        Gateway::ResetPacketFlag()
        {
            m_packet_here = false;
        }

	void
	Gateway::DataIndication (McpsDataIndicationParams params, Ptr<Packet> p)
	{

		NS_LOG_UNCOND ("Received packet of size " << p->GetSize ());
                //create new header for sequence number
		GatewayToNanobotHeader gatewayToNanobotHeader;
                //set sequence number of the new created header object
                gatewayToNanobotHeader.SetSeqNr();
                //set m_to_nanobot to define if packet is for nanobots
                gatewayToNanobotHeader.SetToNanobot(true);
                //set m_hop_count to 0
//                uint32_t hops = 0;
                gatewayToNanobotHeader.SetHopCount(1);
                //add header to packet
		p->AddHeader (gatewayToNanobotHeader);

                //print some tracing context
		NS_LOG_UNCOND ("Added seqNr to packet, size is now " << p->GetSize ());
		NS_LOG_UNCOND ("Is this message for nanobots? " << gatewayToNanobotHeader.GetToNanobot());

                NS_LOG_UNCOND ("Sequence Number for Nanobots is: " << gatewayToNanobotHeader.GetSeqNr ());
                NS_LOG_UNCOND ("Hop Count is now: " << gatewayToNanobotHeader.GetHopCount());

                //set latest packet on the just received packet
		m_latest_packet = p->Copy();

                //set flag for new packet to true

                m_packet_here = true;

                NS_LOG_UNCOND ("There is a packet at the gateway:" << m_packet_here);


	}


	/**
	* \brief if mcpsdataconfirmcallback is triggered this function is called
	*/
	void
	Gateway::DataConfirm (McpsDataConfirmParams params)
	{
	  NS_LOG_UNCOND ("LrWpanMcpsDataConfirmStatus = " << params.m_status);
	}

        bool
        Gateway::NbsReceivedMessage ()
        {
            return m_nanobots_received_message;
        }

        void
        Gateway::ResetNbsReceivedMessage ()
        {
            m_nanobots_received_message = false;
        }

        void
        Gateway::SetNbsReceivedMessage ()
        {
            m_nanobots_received_message = true;
        }

        /*
         *
         * not tested for now
         */
        void
        Gateway::CopyFromNanobot(Ptr<Packet> packet)
        {
            Ptr<Packet> received_packet = packet->Copy();
            GatewayToNanobotHeader g;
            received_packet->RemoveHeader (g);

            //TODO here is a thinking twist, messages can get lost, only the newest packet will be sent to the laptop
            if(m_last_seen_sequence_number < g.GetSeqNr ())
            {
                SendPacketToLaptop(received_packet);
                m_last_seen_sequence_number = g.GetSeqNr();
            }

        }

        void
        Gateway::SendPacketToLaptop(Ptr<Packet> packet)
        {
            McpsDataRequestParams params;
            params.m_dstPanId = 0;
            params.m_srcAddrMode = SHORT_ADDR;
            params.m_dstAddrMode = SHORT_ADDR;
            params.m_dstAddr = Mac16Address ("00:01");
            params.m_msduHandle =  0;
            params.m_txOptions = TX_OPTION_ACK;

            Simulator::ScheduleNow (&LrWpanMac::McpsDataRequest,
                                    this->GetLrWpanDevice ()->GetMac (), params, packet);

        }
}
