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
 * Author: Regine Geyer <geyer@itm.uni-luebeck.de>, Chris Deter <chris.deter@student.uni-luebeck.de>
 */

#include "Bloodvessel.h"
#include "ns3/Nanobot.h"
#include <random>
#include <functional>

using namespace std;
namespace ns3 {

//upon call it returns 0 or 1 
auto randomIntegerBetweenZeroAndOne =
    std::bind (std::uniform_int_distribution<> (0, 1), std::default_random_engine ());

TypeId
Bloodvessel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::Bloodvessel")
      .SetParent<Object> ()
     // .AddConstructor<Bloodvessel> ()
      ;
  return tid;
}

Bloodvessel::Bloodvessel (int id, BloodvesselType type, Vector start, Vector end, double width, Ptr<PrintNanobots> nanoprinter)
{
  m_bloodvesselID = id;
  m_bloodvesselType = type;
  m_startPositionBloodvessel = start;
  m_stopPositionBloodvessel = end;
  m_vesselWidth = width/100;
  m_printer = nanoprinter;
  m_deltaT = 1;

  m_changeStreamSet = true;
  m_basevelocity = 0;
  m_currentStream = 0;
  m_numberOfStreams = stream_definition_size;
  if (this->m_bloodvesselID == 27 || this->m_bloodvesselID == 33) 
  {
    this->th_channel = new Terahertz(10, 10, 100);
  }
  if (this->m_bloodvesselID == 2)
  {
    this->us_channel = new Ultrasound(10, 10, 100);
  }
  //init bloodvessel: calculate length and angle & velocity
  InitBloodvesselVelocity();
  double length = CalcLength ();
  m_bloodvesselLength = length < 0 ? 10000 : length; // move if statement to CalcLength?
  m_angle = CalcAngle ();

  initStreams();
}

Bloodvessel::~Bloodvessel (){}

void
Bloodvessel::SetPrinter (Ptr<PrintNanobots> printer)
{
  m_printer = printer;
}

void
Bloodvessel::Start ()
{
  m_start = true;
  Simulator::Schedule (Seconds (0.0), &Step, Ptr<Bloodvessel> (this));
}

void
Bloodvessel::Step (Ptr<Bloodvessel> bloodvessel)
{
  if (bloodvessel->GetbloodvesselID () == 1)
    {
      static clock_t now, old = clock ();
      old = now;
      now = clock ();
      std::cout << (now - old) / 1000000 << " -> " << Simulator::Now () << std::endl;
    }
  bloodvessel->TranslateNanobots ();
}

Ptr<UniformRandomVariable>
Bloodvessel::getRandomObjectBetween (double min, double max)
{
  Ptr<UniformRandomVariable> random = CreateObject<UniformRandomVariable> ();
  random->SetAttribute ("Min", DoubleValue (min));
  random->SetAttribute ("Max", DoubleValue (max));
  return random;
}



Vector
Bloodvessel::CalcPosition (Vector nbv, double distance)
{
  //Check vessel direction and move according to distance.
  //right
  if (m_angle == 0.00 && m_bloodvesselType != ORGAN)
    {
      nbv.x += distance;
    }
  //left
  else if (m_angle == -180.00 || m_angle == 180.00)
    {
      nbv.x -= distance;
    }
  //down
  else if (m_angle == -90.00)
    {
      nbv.y -= distance;
    }
  //up
  else if (m_angle == 90.00)
    {
      nbv.y += distance;
    }
  //back
  // scale down startPosZ by dividing by 100
  else if (m_angle == 0.00 && m_bloodvesselType == ORGAN && this->GetStartPositionBloodvessel().z == 0.02)
    {
      nbv.z -= distance;
    }
  //front
  // scale down startPosZ by dividing by 100
  else if (m_angle == 0.00 && m_bloodvesselType == ORGAN && this->GetStartPositionBloodvessel().z == -0.02)
    {
      nbv.z += distance;
    }
  //right up, right down, left up, left down
  else if ((0.00 < m_angle && m_angle < 90.00) || (-90.00 < m_angle && m_angle < 0.00) ||
           (90.00 < m_angle && m_angle < 180.00) || (-180.00 < m_angle && m_angle < -90.00))
    {
      nbv.x += distance * (cos (fmod ((m_angle), 360) * M_PI / 180));
      nbv.y += distance * (sin (fmod ((m_angle), 360) * M_PI / 180));
    }
  return nbv;
}

void
Bloodvessel::TranslateNanobots ()
{
  Simulator::Schedule (Seconds (m_deltaT), &Bloodvessel::Step, Ptr<Bloodvessel> (this));
  //if vessel is empty or vessels were just created, just reschedule method, and make sure that m_start is set to false
  if (this->IsEmpty () || m_start)
    {
      m_start = false;
      return;
    }
  static int loop = 1;
  if (loop == 2)
    {
      loop = 0;
    }
  //Change streams only in organs
  if (loop == 0 && m_changeStreamSet == true && this->GetBloodvesselType () == ORGAN)
    {
      ChangeStream ();
    }
  //Translate their position every timestep
  TranslatePosition ();
  loop++;
}

void
Bloodvessel::moveNanobot (Ptr<Nanobot> nb, Ptr<Bloodstream> stream, int randVelocityOffset, bool direction)
{
  double distance;
  if (direction)  //Check vessel direction and move accordingly
    {
      distance = (stream->GetVelocity() - ((stream->GetVelocity() / 100) * randVelocityOffset)) * m_deltaT;
    }
  else
    {
      distance = (stream->GetVelocity() + ((stream->GetVelocity() / 100) * randVelocityOffset)) * m_deltaT;
    }
  nb->SetPosition(CalcPosition (nb->GetPosition (), distance));
  nb->SetTimeStep ();
}

void
Bloodvessel::TranslatePosition ()
{
  list<Ptr<Nanobot>> print;
  list<Ptr<Nanobot>> reachedEnd;
  list<Ptr<Nanobot>> nanobotsInThisVessel;
  auto randomVelocityOffset =
      std::bind (std::uniform_int_distribution<> (0, 11), std::default_random_engine ());
  for (const Ptr<Bloodstream> stream : m_bloodstreams)
    {
      for (const Ptr<Nanobot> &nb : stream->GetNanobotList())
        {
          nb->SetNanobotBodyArea(this->GetbloodvesselID());
          nanobotsInThisVessel.push_back (nb);
          //move only nanobots that have not already been translated by another vessel
          if (nb->GetTimeStep () < Simulator::Now ())
            {
              moveNanobot (nb, stream, randomVelocityOffset (), randomIntegerBetweenZeroAndOne ());
              if (ExceedsVessel(nb, false))
                {
                  CheckRoundCompleted(nb);
                  reachedEnd.push_back (nb);
                }
              else
                {
                  print.push_back (nb);
                }
            } 
        } 
      if (reachedEnd.size () > 0)
        {
          // ns3::BreakpointFallback();
          TransposeNanobots (reachedEnd);
        }
      reachedEnd.clear ();
    } 
  m_printer->PrintSomeNanobots (print, this->GetbloodvesselID ());
  /**
   * @brief PJ KT Code: In-body Ultrasound and Terahertz Communication
   * Loop over all nanobots \p print and check if \p BvID Bloodvessel ID of current nanobot
   * is either Left Hand (33), Right Hand (27) or Left Heart (2).
   * Then collect \p x, \p y, \p z coordianate and \p velocity of nanobot and call external modules.
   * On Left and Right Hand call Terahertz module.
   * On Left Heart call Ultrasound module.
   * 
   * @param print
   */
  int BvID = this->GetbloodvesselID ();
  for (const Ptr<Nanobot> &bot : print)
  {
    if (BvID == 27 || BvID == 33){
      int NanoID = bot->GetNanobotID ();
      double velocity = this->m_basevelocity * 10;
      th_channel->terahertz_module(NanoID, velocity);
    }
    else if (BvID == 2){
      int NanoID = bot->GetNanobotID ();
      double velocity = this->m_basevelocity * 10;
      us_channel->ultrasound_module(NanoID, velocity);
    }
      
  }
  nanobotsCurrentlyInThisVessel = nanobotsInThisVessel;
} //Function

void
Bloodvessel::ChangeStream ()
{
  if (m_numberOfStreams > 1)
    {
      Ptr<UniformRandomVariable> nb_randomize = getRandomObjectBetween (0, m_numberOfStreams);
      for (const Ptr<Bloodstream> stream : m_bloodstreams)
        {
          for (const Ptr<Nanobot> nanobot : stream->GetNanobotList())
            {
              if (randomIntegerBetweenZeroAndOne ())
                { // 50 / 50 Chance for every nanobot to change
                  nanobot->SetShouldChange (true);
                }
            }
        }
      // after all nanobots that should change are flagged, do change
      for (Ptr<Bloodstream> stream : m_bloodstreams)
        {
          int direction = CalcStreamDir(stream);
          DoChangeStreamIfPossible (stream, stream->GetStreamId() + direction);
        }
    }
}

void
Bloodvessel::DoChangeStreamIfPossible (Ptr<Bloodstream> curStream, int desStream)
{
  for (uint j = 0; j < curStream->CountNanobots (); j++)
    {
      if (curStream->GetNanobot (j)->GetShouldChange ())
        {
          curStream->GetNanobot (j)->SetShouldChange (false);
          m_bloodstreams[desStream]->AddNanobot (curStream->RemoveNanobot (j));
        }
    }
  m_bloodstreams[desStream]->SortStream ();
}

void
Bloodvessel::transposeNanobot (Ptr<Nanobot> botToTranspose, Ptr<Bloodvessel> nextBloodvessel)
{
  double distance = sqrt (pow (botToTranspose->GetPosition().x - this->GetStopPositionBloodvessel ().x, 2) +
                          pow (botToTranspose->GetPosition().y - this->GetStopPositionBloodvessel ().y, 2) +
                          pow (botToTranspose->GetPosition().z - this->GetStopPositionBloodvessel ().z, 2));
  distance = distance / m_bloodstreams[botToTranspose->GetStream()]->GetVelocity () *
             nextBloodvessel->m_bloodstreams[botToTranspose->GetStream()]->GetVelocity ();           

  botToTranspose->SetPosition (nextBloodvessel->GetStartPositionBloodvessel());
  const auto nextPos = nextBloodvessel->CalcPosition (botToTranspose->GetPosition(), distance);
  botToTranspose->SetPosition (nextPos);
}

void
Bloodvessel::TransposeNanobots (list<Ptr<Nanobot>> reachedEnd)
{
  list<Ptr<Nanobot>> print1;
  list<Ptr<Nanobot>> print2;
  list<Ptr<Nanobot>> reachedEndAgain;
  Ptr<UniformRandomVariable> rv = getRandomObjectBetween (0, 4);
  for (const Ptr<Nanobot> &botToTranspose : reachedEnd)
    {
      int onetwo = floor (rv->GetValue ());
      m_bloodstreams[botToTranspose->GetStream()]->RemoveNanobot (botToTranspose);
      //Bias for Vessels that deliver blood to organs - 75% go directly forward in the main vessel - hard coded
      if (SuccessorVesselIsBloodDeliverVessel())
        {
          if (onetwo == 1)
            {
              onetwo = 2;
            }
        }
      if (SuccessorVesselIsOrgan())
        {
          if (onetwo == 2)
            {
              onetwo = 1;
            }
        }
      if (m_successor_vessels[1] != NULL && (onetwo == 0 || onetwo == 1))
        {
          transposeNanobot (botToTranspose, m_successor_vessels[1]);
          if (m_successor_vessels[1]->ExceedsVessel(botToTranspose, true))
            { 
              reachedEndAgain.push_back (botToTranspose);
              m_successor_vessels[1]->TransposeNanobots (reachedEndAgain);
              reachedEndAgain.clear ();
            }
          else
            {
              m_successor_vessels[1]->m_bloodstreams[botToTranspose->GetStream()]->AddNanobot (botToTranspose);
              print2.push_back (botToTranspose);
            }
        }
      else if ((onetwo == 2 || onetwo == 3) || m_successor_vessels[1] == NULL)
        {
          transposeNanobot (botToTranspose, m_successor_vessels[0]);
          if (m_successor_vessels[0]->ExceedsVessel(botToTranspose, true))
            {
              reachedEndAgain.push_back (botToTranspose);
              m_successor_vessels[0]->TransposeNanobots (reachedEndAgain);
              reachedEndAgain.clear ();
            }
          else
            {
              m_successor_vessels[0]->m_bloodstreams[botToTranspose->GetStream()]->AddNanobot (botToTranspose);
              print1.push_back (botToTranspose);
            }
        }
    } // for
  m_printer->PrintSomeNanobots (print1, m_successor_vessels[0]->GetbloodvesselID ());
  if (m_successor_vessels[1]!= NULL)
    {
      m_printer->PrintSomeNanobots (print2, m_successor_vessels[1]->GetbloodvesselID ());
    }
  
}

//HELPER
void
Bloodvessel::PrintNanobotsOfVessel ()
{
  for (Ptr<Bloodstream> stream : m_bloodstreams)
    {
      for (Ptr<Nanobot> nanobot : stream->GetNanobotList())
        {
          m_printer->PrintNanobot (nanobot, GetbloodvesselID ());
        }
    }
}

void
Bloodvessel::initStreams ()
{
  double streamWidth = CalcStreamWidth();
  Ptr<Bloodstream> stream;
  for (int i = 0; i < stream_definition_size; i++)
    {
      stream = CreateObject<Bloodstream> ();
      stream->initBloodstream (m_bloodvesselID, i, stream_definition[i][0],
                               stream_definition[i][1] / 10.0, stream_definition[i][2] / 10.0);
      stream->SetVelocity (m_basevelocity);
      //TODO this could be something that goes wrong
      stream->SetAngle (m_angle, stream_definition[i][1] * streamWidth,
                                    stream_definition[i][2] * streamWidth);
      m_bloodstreams.push_back (stream);
    }
}

double
Bloodvessel::CalcLength ()
{
  if (GetBloodvesselType () == ORGAN)
    {
      //scale down by divding by 100
      return 0.04; 
    }
  else
    {
      return sqrt (pow (m_stopPositionBloodvessel.x - m_startPositionBloodvessel.x, 2) + pow (m_stopPositionBloodvessel.y - m_startPositionBloodvessel.y, 2));
    }
}

double
Bloodvessel::CalcAngle ()
{
  Vector m_start = GetStartPositionBloodvessel ();
  Vector m_end = GetStopPositionBloodvessel ();
  double x = m_end.x - m_start.x;
  double y = m_end.y - m_start.y;
  return atan2 (y, x) * 180 / M_PI;
}

double
Bloodvessel::CalcStreamWidth()
{
  int maxOffsetFactor = 0;
  for (int i = 0; i < m_numberOfStreams; i++)
  {
    // take the biggest offset factor from x or y axis
    if (maxOffsetFactor < stream_definition[i][1])
          {
            maxOffsetFactor = stream_definition[i][1];
          }
    if (maxOffsetFactor < stream_definition[i][2])
          {
            maxOffsetFactor = stream_definition[i][2];
          }
  }
  return m_vesselWidth / 2.0 / maxOffsetFactor;
}

int
Bloodvessel::CalcStreamDir(Ptr<Bloodstream> stream)
{
  int direction = randomIntegerBetweenZeroAndOne () == true ? -1 : 1;
  if (stream->GetStreamId() == 0)
    { //Special Case 1: outer lane left -> go to middle
      return 1;
    }
  else if (stream->GetStreamId()+1 >= m_numberOfStreams)
    { //Special Case 2: outer lane right -> go to middle
      return -1;
    }
  return direction;
}

void
Bloodvessel::CheckRoundCompleted(Ptr<Nanobot> nb)
{
  if(m_bloodvesselID == 2)
  {
     nb->SetVisitedGateway(nb->GetVisitedGateway() + 1);
  }
}

bool
Bloodvessel::ExceedsVessel(Ptr<Nanobot> nb, bool fromTranspose)
{
  double nbx = nb->GetPosition ().x - m_startPositionBloodvessel.x;
  double nby = nb->GetPosition ().y - m_startPositionBloodvessel.y;
  double nbMoveSize = sqrt (nbx * nbx + nby * nby);
  double upperBound = 0.02;
  double lowerBound = -0.02;
  if(fromTranspose){
    return (nbMoveSize > m_bloodvesselLength) || (nb->GetPosition().z < lowerBound) ||
                  (nb->GetPosition().z > upperBound);
  }else{
    return (nbMoveSize > m_bloodvesselLength) || (nb->GetPosition().z < lowerBound && m_angle == 0) ||
                  (nb->GetPosition().z > upperBound && m_angle == 0);
  }
}

void
Bloodvessel::InitBloodvesselVelocity()
{
  //Scale down by dividing by 100
  if (m_bloodvesselType == ARTERY) {
      m_basevelocity = 10.0/100;
  } else if (m_bloodvesselType == VEIN) {
      m_basevelocity = 3.7/100;
  } else {// if (type == ORGAN)
      m_basevelocity = 1.0/100;
  }
}

bool
Bloodvessel::SuccessorVesselIsBloodDeliverVessel()
{
  return (m_successor_vessels[1] != NULL && (m_successor_vessels[1]->GetbloodvesselID () == 6||
                                            m_successor_vessels[1]->GetbloodvesselID () == 32||
                                            m_successor_vessels[1]->GetbloodvesselID () ==35));
}

bool
Bloodvessel::SuccessorVesselIsOrgan()
{
  // TODO CD:  Umbauen auf m_nextBloodVessel->getNextBloodvessel->isOrgan()?
  return (m_successor_vessels[0]->GetbloodvesselID () == 3 ||
          m_successor_vessels[0]->GetbloodvesselID () == 7 ||
          m_successor_vessels[0]->GetbloodvesselID () == 12 ||
          m_successor_vessels[0]->GetbloodvesselID () == 24 ||
          m_successor_vessels[0]->GetbloodvesselID () == 25 ||
          m_successor_vessels[0]->GetbloodvesselID () == 37 ||
          m_successor_vessels[0]->GetbloodvesselID () == 43 ||
          m_successor_vessels[0]->GetbloodvesselID () == 45);
}

//GETTER AND SETTER

bool
Bloodvessel::IsEmpty ()
{
  bool empty = true;
  for(Ptr<Bloodstream> stream : m_bloodstreams)
    {
      empty = empty && stream->IsEmpty ();
    }
  return empty;
}

int
Bloodvessel::GetbloodvesselID ()
{
  return m_bloodvesselID;
}

void
Bloodvessel::SetBloodvesselID (int b_id)
{
  m_bloodvesselID = b_id;
}

double
Bloodvessel::GetBloodvesselAngle ()
{
  return m_angle;
}

int
Bloodvessel::GetNumberOfStreams ()
{
  return m_numberOfStreams;
}

Ptr<Bloodstream>
Bloodvessel::GetStream (int id)
{
  return m_bloodstreams[id];
}

double
Bloodvessel::GetbloodvesselLength ()
{
  return m_bloodvesselLength;
}

void
Bloodvessel::SetVesselWidth (double value)
{
  m_vesselWidth = value;
}

void
Bloodvessel::AddNanobotToStream (unsigned int streamID, Ptr<Nanobot> bot)
{
  m_bloodstreams[streamID]->AddNanobot (bot);
}

BloodvesselType
Bloodvessel::GetBloodvesselType ()
{
  return m_bloodvesselType;
}

void
Bloodvessel::SetBloodvesselType (BloodvesselType value)
{
  m_bloodvesselType = value;
}

Vector
Bloodvessel::GetStartPositionBloodvessel ()
{
  return m_startPositionBloodvessel;
}

void
Bloodvessel::SetStartPositionBloodvessel (Vector value)
{
  m_startPositionBloodvessel = value;
}

Vector
Bloodvessel::GetStopPositionBloodvessel ()
{
  return m_stopPositionBloodvessel;
}

void
Bloodvessel::SetStopPositionBloodvessel (Vector value)
{
  m_stopPositionBloodvessel = value;
}

list<Ptr<Nanobot>> 
Bloodvessel::GetNanobotList ()
{
    return nanobotsCurrentlyInThisVessel;
}

void 
Bloodvessel::SetSuccessorVessel(Ptr<Bloodvessel> successor_vessel)
{
   m_successor_vessels.push_back(successor_vessel);
}
} // namespace ns3
