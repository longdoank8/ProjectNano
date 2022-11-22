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

#ifndef HEALTH_CARE_H
#define HEALTH_CARE_H

	#include <stdint.h>
	#include <string>
	#include "ns3/header.h"

	namespace ns3 {
		/**
		 * \ingroup healthcare
		 * \brief Packet header for healthcare packets
		 *
		 */

//		 enum class addressingtype
//		 {
//		 		unicast = 0,
//		 		multicast = 1,
//		 		broadcast = 2
//		 };
//
//		 enum class function
//		 {
//		 		measure = 0,
//		 		work_on_problem = 1,
//		 		both = 2
//		 };
//
//		 //TODO: insert all the vessels
//		 enum class bodyarea
//		 {
//		 		head = 0,
//		 		chest = 1,
//		 		left_arm = 2,
//		 		right_arm = 3,
//		 		belly = 4,
//		 		left_leg = 5,
//		 		right_leg = 6
//
//		 };

//		 enum class task
//		 {
//		  	random_task_0 = 0,
//		 		random_task_1 = 1,
//		 		random_task_2 = 2
//		 };
//


		class HealthCareHeader : public Header
		{
			public:
                                /**
                                * \brief Constructor
                                *
                                * Creates a null header
                                */

                                HealthCareHeader ();
                                HealthCareHeader(uint32_t start_nanobot_id, uint32_t end_nanobot_id, uint16_t function_id, uint16_t addressing_type, uint16_t body_area, uint16_t task);
                                ~HealthCareHeader () = default;

                                /**
                                * \param start the start for the range of nanobot ids
                                * \param end the end for the range of nanobot ids
                                */
                                void SetRangeOfNanobotIds (uint32_t start, uint32_t end);

                                /**
                                * \param functionId the function a nanobot has
                                */
                                void SetNanobotFunction (uint16_t functionId);

                                /**
                                * \param addressingType the type which describes how to address the nanobots
                                */
                                void SetAddressingType (uint16_t addressingType);

                                /**
                                * \param bodyarea the location where the nanobot should be to receive the message
                                */
                                void SetBodyArea (uint16_t bodyarea);

                                /**
                                * \param task the task that should be done by the nanobots
                                */
                                void SetTask (uint16_t task);

                                uint32_t GetStartNanobotId (void);

                                uint32_t GetEndNanobotId (void);

                                uint16_t GetFunction (void);

                                uint16_t GetBodyArea (void);

                                uint16_t GetTask (void);

                                /**
                                * \brief Get the type ID.
                                * \return the object TypeId
                                */


                                static TypeId GetTypeId (void);
                                virtual TypeId GetInstanceTypeId (void) const;

                                //------------------------------------------------------------------------------
                                //methods which are inherited by class 'header' and have to be reimplemented
                                //------------------------------------------------------------------------------

                                /**
                                * this method is used to print the content of a header as ascii data to a c++ output stream
                                */
                                void Print (std::ostream &os) const override;
                                /**
                                * the expected size of the header
                                */
                                uint32_t GetSerializedSize (void) const override;
                                /**
                                 * serializes the header content
                                 * @param start start of header in buffer
                                 */
                                void Serialize (Buffer::Iterator start) const override;
                                /**
                                 * deserializes packet content
                                 * @param start start of header in buffer
                                 * @return the header size
                                 */
                                uint32_t Deserialize (Buffer::Iterator start) override;

			private:
                                uint32_t m_start_nanobot_id; // !< Start for nanobot id range
                                uint32_t m_end_nanobot_id; // !< End for nanobot id range
                                uint16_t m_function_id; // !< Function a nanobot should have
                                uint16_t m_addressing_type; // !< How to address nanobots
                                uint16_t m_body_area; // !< the body are in which the nanobot currently is
                                uint16_t m_task; // !< the task that has to be accomplished
		};

	}//namespace ns3
#endif //HEALTH_CARE_H
