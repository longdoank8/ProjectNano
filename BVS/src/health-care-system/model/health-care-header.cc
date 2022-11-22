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

 #include "health-care-header.h"


namespace ns3{

  NS_OBJECT_ENSURE_REGISTERED (HealthCareHeader);

  //default constructor
  HealthCareHeader::HealthCareHeader()
  {
    m_start_nanobot_id = 0;
    m_end_nanobot_id = 0;
    m_function_id = 0;
    m_addressing_type = 0;
    m_body_area = 0;
    m_task = 0;
  }

  HealthCareHeader::HealthCareHeader(uint32_t start_nanobot_id, uint32_t end_nanobot_id, uint16_t function_id, uint16_t addressing_type, uint16_t body_area, uint16_t task)
  {
    m_start_nanobot_id = start_nanobot_id;
    m_end_nanobot_id = end_nanobot_id;
    m_function_id = function_id;
    m_addressing_type = addressing_type;
    m_body_area = body_area;
    m_task = task;
  }



  void
  HealthCareHeader::SetRangeOfNanobotIds (uint32_t start, uint32_t end)
  {
  	m_start_nanobot_id = start;
  	m_end_nanobot_id = end;
  }
  
  uint32_t 
  HealthCareHeader::GetStartNanobotId ()
  {
      return m_start_nanobot_id;
  }
  
  uint32_t
  HealthCareHeader::GetEndNanobotId ()
  {
      return m_end_nanobot_id;
  }
  
  uint16_t
  HealthCareHeader::GetFunction ()
  {
      return m_function_id;
  }
  
  uint16_t
  HealthCareHeader::GetBodyArea ()
  {
      return m_body_area;
  }
  
  uint16_t
  HealthCareHeader::GetTask ()
  {
      return m_task;
  }
  void
  HealthCareHeader::SetNanobotFunction (uint16_t functionId)
  {
  	m_function_id = functionId;
  }

  void
  HealthCareHeader::SetAddressingType (uint16_t addressingType)
  {
  	m_addressing_type = addressingType;
  }

  void
  HealthCareHeader::SetBodyArea (uint16_t bodyarea)
  {
  	m_body_area = bodyarea;
  }

  void
  HealthCareHeader::SetTask (uint16_t task)
  {
  	m_task = task;
  }

  TypeId
  HealthCareHeader::GetTypeId (void)
  {
    static TypeId tid = TypeId ("ns3::HealthCareHeader")
      .SetParent<Header> ()
      .SetGroupName ("health-care-system")
      .AddConstructor<HealthCareHeader> ()
    ;
    return tid;
  }

  TypeId
  HealthCareHeader::GetInstanceTypeId (void) const
  {
    return GetTypeId ();
  }

  void
  HealthCareHeader::Print (std::ostream &os) const
  {
  	os << "Nanobot ID range = " << m_start_nanobot_id << " - " << m_end_nanobot_id
  		 << " "
  		 << "Function nanobots should have = " << m_function_id
  		 << " "
  		 << "How to address nanobots = " << m_addressing_type
  		 << " "
  		 << "Location of nanobots = " << m_body_area
  		 << " "
  		 << "Task for nanobots = " << m_task
  	;
  }

  uint32_t
  HealthCareHeader::GetSerializedSize (void) const
  {
  	return 16;
  }

  
  void
  HealthCareHeader::Serialize (Buffer::Iterator start) const
  {
   	Buffer::Iterator i = start;
  	i.WriteHtonU32 (m_start_nanobot_id);
  	i.WriteHtonU32 (m_end_nanobot_id);
  	i.WriteHtonU16 (m_function_id);
  	i.WriteHtonU16 (m_addressing_type);
  	i.WriteHtonU16 (m_body_area);
  	i.WriteHtonU16 (m_task);
  }

  
  uint32_t
  HealthCareHeader::Deserialize (Buffer::Iterator start)
  {

  	Buffer::Iterator i = start;
  	m_start_nanobot_id = i.ReadNtohU32 ();
  	m_end_nanobot_id = i.ReadNtohU32 ();
  	m_function_id = i.ReadNtohU16 ();
  	m_addressing_type = i.ReadNtohU16 ();
  	m_body_area = i.ReadNtohU16 ();
  	m_task = i.ReadNtohU16 ();

  	return GetSerializedSize ();
  }

}
