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
 * Author: Regine Geyer <geyer@itm.uni-luebeck.de>
 */

#include "Bloodcircuit.h"
#include "Nanobot.h"
#include <fstream>

/**
 * This function sets the Bloodvessel map of the human body of a female: hight 1.72m, weight 69kg, age 29.
 */

namespace ns3 {

    double Bloodcircuit::m_deltaT = 1;

    Bloodcircuit::Bloodcircuit(unsigned int numberOfNanobots, unsigned int injectionVessel) {
        // initialise map with bloodvesselinformation
        m_bloodvessels = map<int, Ptr < Bloodvessel >> ();
        this->printer = new PrintNanobots();


        // loading vasculature via csv
        std::ifstream infile{"vasculature.csv"};
    
        // check if infile is good
        if(!(infile.good())){
            throw std::ios_base::failure("NO VALID CSV FILE FOUND!\n Please provide a valid 'vasculature.csv'");
        }

        // check if file is valid
        std::vector<double> numbers;
        numbers.resize(valuesPerLine);
        std::string buffer;
        buffer.reserve(64);

        int errorflag = 0;

        while (infile.good()) {
            for (auto &&elem : numbers) {
                if (std::getline(infile, buffer, ',')) {
                    elem = std::stoi(buffer);
                } else {
                    elem = 0;
                    errorflag = 1;
                }
            }
            if (errorflag == 0) {
                AddVesselData(numbers[0], (BloodvesselType) numbers[1],
                        Vector(numbers[2]/100, numbers[3]/100, numbers[4]/100),
                        Vector(numbers[5]/100, numbers[6]/100, numbers[7]/100));
            }
        }
        cout << "load CSV - done" << endl;
        // Check if there is more than one vessel in vasculature.csv
        if (!(m_bloodvessels.size() > 1))
        {
            throw std::invalid_argument("Not enough vessels for simulation! Please check 'vasculature.csv'");        
        }

        //connect bloodvessels with each other
        ConnectBloodvessels();
        //Inject Nanobots here!
        //Places x Bots, randomly on Streams at specific vessel injected.
        //If you choose other values, the nanobots all get injected at the same coordinates
        if (m_bloodvessels.size() > 1) {
            InjectNanobots(numberOfNanobots, m_bloodvessels[injectionVessel < m_bloodvessels.size() ? injectionVessel : m_bloodvessels.size() - 1]);
        }
    }

    Bloodcircuit::~Bloodcircuit() {
        m_bloodvessels.clear();
    }

    map<int, ns3::Ptr<ns3::Bloodvessel>>
    Bloodcircuit::GetBloodcircuit() {
        return m_bloodvessels;
    }

    Vector
    Bloodcircuit::CalcDirectionVectorNorm(Ptr<Bloodvessel> m_bloodvessel) {
        double vectorLength = CalculateDistance(m_bloodvessel->GetStartPositionBloodvessel(), m_bloodvessel->GetStopPositionBloodvessel());
        // Normalization of distance vector
        double x = (m_bloodvessel->GetStopPositionBloodvessel().x - m_bloodvessel->GetStartPositionBloodvessel().x) / vectorLength;
        double y = (m_bloodvessel->GetStopPositionBloodvessel().y - m_bloodvessel->GetStartPositionBloodvessel().y) / vectorLength;
        double z = (m_bloodvessel->GetStopPositionBloodvessel().z - m_bloodvessel->GetStartPositionBloodvessel().z)/ vectorLength;

        return Vector(x, y, z);
    }

    void
    Bloodcircuit::InjectNanobots(int numberOfNanobots, Ptr<Bloodvessel> bloodvessel) {
        int number_of_Nanobot_groups = 10;
        Ptr<UniformRandomVariable> distribute_randomly =
                bloodvessel->getRandomObjectBetween(0, bloodvessel->GetNumberOfStreams());       
        Vector m_coordinateStart = bloodvessel->GetStartPositionBloodvessel();

        //calculate amount of nanobots per group
        int intervall;
        //IF if equal or more than 10 Nanobots are injected,
        if(numberOfNanobots >= number_of_Nanobot_groups) {
            // THEN Divide number of Nanobots into 10 equaly sized groups 
            //div(int, int) returns quotient and remainder - save the quotient in intervall
            intervall = div(numberOfNanobots, number_of_Nanobot_groups)
                .quot; 
        }else{
            intervall = numberOfNanobots; // ELSE put them on beginning of the bloodvessel in one point -> intervall is therefore as big as the number of Nanobots itself
        }

        //Calculate the normalized vector from direction vector
        Vector m_direction = CalcDirectionVectorNorm(bloodvessel);
        // normalized vector scaled down to meter
        m_direction = Vector(m_direction.x/100, m_direction.y/100, m_direction.z/100);
        
        //Set direction intervall as 1/10 of the normalized direction vector.
        Vector directionIntervall =
                Vector(m_direction.x/ number_of_Nanobot_groups, m_direction.y/ number_of_Nanobot_groups,
                m_direction.z / number_of_Nanobot_groups);        
        // cout << "directionIntervall: " << directionIntervall.x <<"," <<directionIntervall.y << "," <<directionIntervall.z <<endl;

        unsigned int groupID = 0; // Group 0 to 9

        //Distribute Nanobots in 10 groups over the beginning of the start vessel.
        for (int i = 1; i <= numberOfNanobots; ++i) {
            // groupID calculated by id of nanobot
            groupID = (i - 1) / intervall;
            // cout << "groupID " << groupID<<endl;
            // cout << "directionIntervall multiplied with groupID " << directionIntervall.x * groupID <<"," <<directionIntervall.y *groupID << "," <<directionIntervall.z *groupID <<endl;
            Vector initialPosition = Vector(m_coordinateStart.x + (directionIntervall.x * groupID),
                                        m_coordinateStart.y + (directionIntervall.y * groupID),
                                        m_coordinateStart.z + (directionIntervall.z * groupID));
            Ptr<Nanobot> temp_nb = CreateObject<Nanobot> (i, initialPosition);
            
            int dr = floor(distribute_randomly->GetValue());
            bloodvessel->AddNanobotToStream(dr, temp_nb);
        }
        bloodvessel->PrintNanobotsOfVessel();
    }

    void
    Bloodcircuit::AddVesselData(int id, BloodvesselType type, Vector start, Vector stop) {
        Ptr<Bloodvessel> vessel = CreateObject<Bloodvessel> (id, type, start, stop, 0.25, printer);
        m_bloodvessels[id] = vessel;

        cout << "New Vessel(" + to_string(id) + "," + to_string(type) + "," + to_string(start.x* 100) +
                "," + to_string(start.y * 100) + "," + to_string(start.z* 100) + "," + to_string(stop.x* 100) +
                "," + to_string(stop.y * 100) + "," + to_string(stop.z* 100) + ")"
                << endl;
    }

    //TODO:  throw exception if a vessel does not have another one connected to start and end position?
    void
    Bloodcircuit::ConnectBloodvessels() {
        //Set Connections between bloodvessels if they have the same start/end coordinates
        for (const auto& currentVessel: m_bloodvessels)
        {
            Vector end = currentVessel.second->GetStopPositionBloodvessel();

            for (const auto& vesseltoConnect : m_bloodvessels)
            {   //search through all other vessels to get the vessel with fitting start position
                Vector start = vesseltoConnect.second->GetStartPositionBloodvessel();
                if (end.x == start.x && end.y == start.y && end.z == start.z) {
                    currentVessel.second->SetSuccessorVessel(vesseltoConnect.second);
                    // cout << "The successor vessel of vessel " << currentVessel.first << " is " <<  vesseltoConnect.first << endl; 
                }
            }
        }
    }

} // namespace ns3
