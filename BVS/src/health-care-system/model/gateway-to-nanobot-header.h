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
 * Author: Anke Küstner <anke.kuestner@ccs-labs.org>
 */

#ifndef GATEWAY_TO_NANOBOT_H
#define GATEWAY_TO_NANOBOT_H

#include <iostream>
#include <stdint.h>
#include <string>
#include "ns3/header.h"

namespace ns3 {

    /**
     * \ingroup healthcare
     * \brief Packet header for being sure its a new packet
     *
     */
    class GatewayToNanobotHeader : public Header {
    public:

        /**
         *\brief Constructor
         * Creates a Header with a new SeqNr
         */

        GatewayToNanobotHeader() {
        };
        virtual ~GatewayToNanobotHeader() = default;

        /**
         * @return sequence number 
         */

        uint32_t GetSeqNr(void);


        bool GetToNanobot(void);

        /**
         * \brief Get the type ID.
         * \return the object TypeId
         */

        static TypeId GetTypeId(void);
        virtual TypeId GetInstanceTypeId(void) const;

        //------------------------------------------------------------------------------
        //methods which are inherited by class 'header' and have to be reimplemented
        //------------------------------------------------------------------------------

        /**
         * this method is used to print the content of a header as ascii data to a c++ output stream
         */
        void Print(std::ostream &os) const override;
        /**
         * the expected size of the header
         */
        uint32_t GetSerializedSize(void) const override;

        /**
         * serialize header content 
         * @param start start of buffer
         */
        void Serialize(Buffer::Iterator start) const override;

        /**
         * deserialize header content
         * @param start start of buffer
         * @return the size of the header in bytes
         */
        uint32_t Deserialize(Buffer::Iterator start) override;

        /**
         * set sequence number of the header object
         */
        void SetSeqNr(void);

        /**
         * Tag for addressing nanobots or gateway
         * @param ToNanobot true if packet is addressed to bots
         */
        void SetToNanobot(bool ToNanobot);

        /**
         * Increase hop count by one
         */
        void AddHopCount(void);
        
        /**
         * Set hop count of the header object
         * @param hop_count
         */
        void SetHopCount(uint32_t hop_count);
        
        /**
         * Get hop count of the header object
         * @return hop count
         */
        uint32_t GetHopCount(void);

    private:

        static uint32_t m_counter; // !< The counter for the seqNr
        uint32_t m_seq_nr; // !< The seqNr which should be added to the packet
        bool m_to_nanobot; // !< To differentiate if packet should be send to nanobots or gateway
        uint32_t m_hop_count; // !< The counter for the hops a packet does

    };


}//namespace ns3

#endif //GATEWAY_TO_NANOBOT_H
