/**
 * @file ultrasound-channel.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 1.0
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef CLASS_ULTRASOUND_
#define CLASS_ULTRASOUND_

#include "ns3/object.h"
#include "ns3/vector.h"
#include "../BPSK_Transceiver_C_code/modulation.h"

class Modulation;

using namespace std;
namespace ns3 {

class Ultrasound : public ns3::Object
{
private:
	const double c_0 = 1480; //propagation speed in m/s
	const double center_frequency = 1e6; // in Hz
	const double fir_fs = 5.0000e+06; // in Hz

	const double d_min = 20e-4;
	const double d_max = 80e-4;
	
	const long unsigned int total_steps = pow(10,5); //time
	std::vector<double> gain;
	std::vector<double> delay;
	std::vector<double> cos_angle;
		
	
	int numSamples;
	int numPeriods;
	int numBits;
	
	ofstream outputGain;
	ofstream outputPathLoss;
	ofstream outputSendSignal;
	ofstream outputDemodRecvSignal;
	ofstream outputDemodSignal;
	ofstream outputRecvSignal;
	ofstream outputDelay;
	
	Ptr<Modulation> transceiver;
	
	
public:

	Ultrasound(int samples, int periods, int bits);
	
	~Ultrasound(void);

	void ultrasound_module(double NanoID, double velocity);
	
	double dopplerTerm(double velocity, double phi);
	

	std::vector<double> readVectorFromFile(std::string filename);
	double convertDelayVectorToMicroSeconds(double delay);
	double convertGainToPowerLoss(double value);

};
};
#endif //CLASS_ULTRASOUND_H
