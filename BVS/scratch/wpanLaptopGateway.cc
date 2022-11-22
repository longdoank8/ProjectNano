/*
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
 * Anke Küstner anke.kuestner@ccs-labs.org
 */

#include "ns3/core-module.h"
#include "ns3/lr-wpan-module.h"
#include "ns3/simulator.h"
#include "ns3/laptop-gateway-connection.h"
#include "ns3/health-care-header.h"
#include "ns3/Bloodcircuit.h"
#include <iostream>
#include <fstream>
#include "ns3/system-wall-clock-ms.h"
#include "ns3/nano-communication.h"
 /*
 Implementation of the connection between Laptop and Gateway using the
 implementation of IEEE802.15.4
 */





using namespace ns3;

void
Starter(Ptr<Bloodvessel> vessel) {
	vessel->Start();
}

void
MessageManagement(double delta, NanoCommunication nanocomm) {
	nanocomm.ScheduleMessageRoutines();

	 Simulator::Schedule(Seconds(delta), &MessageManagement, delta, nanocomm);
}



static void StateChangeNotification (std::string context, Time now, LrWpanPhyEnumeration oldState, LrWpanPhyEnumeration newState)
{
  NS_LOG_UNCOND (context << " state change at " << now.GetSeconds ()
                         << " from " << LrWpanHelper::LrWpanPhyEnumerationPrinter (oldState)
                         << " to " << LrWpanHelper::LrWpanPhyEnumerationPrinter (newState));
}

static void CourseChange (Ptr<const MobilityModel> model)
{
	Vector position = model->GetPosition ();
	NS_LOG_UNCOND (" x = " << position.x << ", y = " << position.y << ", z = " << position.z);
}

void EnableOutputToConsole(Ptr<LaptopGatewayConnection> lgconnection)
{
	//--------------------------------------------------------------------
	//for tracing how the connection between laptop and gateway is working
	//--------------------------------------------------------------------
	 lgconnection->GetLaptop ()->GetLrWpanDevice ()->GetPhy ()->TraceConnect ("TrxState", std::string ("PHYlaptop"), MakeCallback (&StateChangeNotification));
	 lgconnection->GetGateway ()->GetLrWpanDevice ()->GetPhy ()->TraceConnect ("TrxState", std::string ("PHYgateway"), MakeCallback (&StateChangeNotification));

	//-----------------------------------------------------
	// for tracing position changes of laptop and gateway
	//-----------------------------------------------------
	 lgconnection->GetLaptop ()->GetLrWpanDevice ()->GetPhy ()->GetMobility ()->TraceConnect ("Position", std::string ("Laptop"), MakeCallback (&CourseChange));
	 lgconnection->GetGateway ()->GetLrWpanDevice ()->GetPhy ()->GetMobility ()->TraceConnect ("Position", std::string ("Gateway"), MakeCallback (&CourseChange));


	//--------------------
	//tracing
	//--------------------

	//needed for tracing
	LrWpanHelper lrWpanHelper;
	lrWpanHelper.EnableLogComponents ();
	//Pcap Trace
	lrWpanHelper.EnablePcapAll (std::string ("laptop-gateway connection"), true);
	//ASCII Trace
	AsciiTraceHelper ascii;
	Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream ("laptop-gateway-connection.tr");
	lrWpanHelper.EnableAsciiAll (stream);
}


void RunTestSimulation(Ptr<LaptopGatewayConnection> lgconnection, uint32_t startRangeNanobotId, uint32_t endRangeNanobotId, uint16_t nanobotFunction,
	uint16_t addressingType, uint16_t bodyArea, uint16_t task, int simulationDuration, int numOfNanobots, int injectionVessel)
{


	McpsDataRequestParams params;
	params.m_dstPanId = 0;
	params.m_srcAddrMode = SHORT_ADDR;
	params.m_dstAddrMode = SHORT_ADDR;
	params.m_dstAddr = Mac16Address ("00:02");
	params.m_msduHandle =  0;
	params.m_txOptions = TX_OPTION_ACK;

	//send packet to destinationaddr at simulation second 0
	/*
	* DESCRIPTION ON HOW A PACKET IS SENT
	* Schedule with context(uint32_t context, Time const & delay, MEM mem_ptr, OBJ obj, T1 a1, T2 a2)
	* context = 1 -> user-specified context parameter
	* delay = 0s -> relative expiration time of the event
	* mem_ptr = &LrWpanMac::McpsDataRequest -> member method pointer to invoke
	* obj = laptopdevice->GetMac() -> the object on which to invoke the member method
	* a1 = the first argument to pass to the invoked method -> in this case the request parameters
	* a2 = the second argument to pass to the invoked method -> in this case the packet to be transmitted
	*/

	//----------------------
	//create a packet
	//----------------------

	Ptr<Packet> packet0 = Create<Packet> ();

	HealthCareHeader healthcareHeader(startRangeNanobotId, endRangeNanobotId, nanobotFunction, addressingType, bodyArea, task);
        //add the header to the packet
 	packet0->AddHeader (healthcareHeader);

	params.m_dstAddr = Mac16Address ("00:02");

//	Simulator::ScheduleWithContext (1, Seconds(10.0),
//                                &LrWpanMac::McpsDataRequest,
//                                lgconnection->GetLaptop ()->GetLrWpanDevice ()->GetMac (), params, packet0);

	Ptr<Packet> packet1 = Create<Packet> ();

	//HealthCareHeader healthcareHeader1 (startRangeNanobotId, endRangeNanobotId, nanobotFunction, addressingType, bodyArea, task);
	//add the header to the packet
	packet1->AddHeader (healthcareHeader);

	params.m_dstAddr = Mac16Address ("00:02");

	Simulator::ScheduleWithContext (2, Seconds(60.0),
							&LrWpanMac::McpsDataRequest,
							lgconnection->GetLaptop ()->GetLrWpanDevice ()->GetMac (), params, packet1);

	
	return;
}

int main(int argc, char *argv[])
{

	RngSeedManager::SetSeed(1);
	RngSeedManager::SetRun(1);

	uint32_t startRangeNanobotId = 10;
	uint32_t endRangeNanobotId = 100;
	uint16_t nanobotFunction = 0;
	uint16_t bodyArea = 2;
	uint16_t addressingType = 0;
	uint16_t task = 2;

	//Add default values for bloodvoyagers:
	int numOfNanobots = 50;//0;
	int simulationDuration = 30;//;300;
	int injectionVessel = 1;

	Ptr<LaptopGatewayConnection> lgconnection = CreateObject<LaptopGatewayConnection> ();

	EnableOutputToConsole (lgconnection);

	//ParseArgs (argc, argv);
	CommandLine cmd;
	cmd.AddValue ("startRangeNanobotId", "startRangeNanobotId", startRangeNanobotId);
	cmd.AddValue ("endRangeNanobotId", "endRangeNanobotId", endRangeNanobotId);
	cmd.AddValue ("nanobotFunction", "nanobotFunction", nanobotFunction);
	cmd.AddValue ("bodyArea", "bodyArea", bodyArea);
	cmd.AddValue ("addressingType", "addressingType", addressingType);
	cmd.AddValue ("task", "task", task);
	cmd.AddValue ("simulationDuration", "simulationDuration", simulationDuration);
	cmd.AddValue ("numOfNanobots", "numOfNanobots", numOfNanobots);
	cmd.AddValue ("injectionVessel", "injectionVessel", injectionVessel);
	cmd.Parse (argc, argv);

	/**
	 * measure execution time
	 */
	SystemWallClockMs realtime;
	realtime.Start();

	/**
	 * Define Simulation Time
	 */
	Simulator::Stop(Seconds(simulationDuration + 1));

	RunTestSimulation (lgconnection, startRangeNanobotId, endRangeNanobotId, nanobotFunction, addressingType, bodyArea, task, simulationDuration, numOfNanobots, injectionVessel);


	//Create Bloodcircuit
	Bloodcircuit circuit(numOfNanobots,injectionVessel);

	NanoCommunication nanocomm(circuit);


	// Schedule and run the Simulation in each bloodvessel
	for(const auto& vessel : circuit.GetBloodcircuit())
	{
		Simulator::Schedule(Seconds(0.0), &Starter, vessel.second);
	}
	// schedule and run the simulation of the messageManagement of the nanobots
	Simulator::Schedule(Seconds(0.0), &MessageManagement, 1, nanocomm);
	/**
	 * Start Simulation and call destroy to avoid false-positive reports by a leak checker
	 */
	Simulator::Run();
	Simulator::Destroy();

	/**
	 *  Print Simulation Run Information
	 
	int64_t elapsed_time = realtime.End();
    cout << "Dauer: " << simulationDuration << "s " << numOfNanobots << "NB -> "
         << elapsed_time * 0.001 << "s ------------------------" << endl;
    cout << "Injected Vessel: " << injectionVessel << endl;
	*/
	
}
