/**
 * @file ultrasound-channel.cc
 * @author your name (you@domain.com)
 * @brief 
 * @version 1.0
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <iostream>
#include <math.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <cassert>
#include <cmath>

#include "ultrasound-channel.h"

using namespace std;
namespace ns3 {

/**
* @brief Construct a new Ultrasound::Ultrasound object
* Read the csv files containing interpolated gain, delay and the angle per timestep from the matlab simulations
* Create output files for writing results
*/
Ultrasound::Ultrasound(int samples, int periods, int upsampledBitSequence)
{
	// read matlab files
	gain = readVectorFromFile("gain_int_ber.csv");
	delay = readVectorFromFile("delay_int.csv");
 	cos_angle = readVectorFromFile("cos_angle.csv");

 
    outputGain.open("../Plots/data/ultrasound_output_gain.csv");
	outputGain << "Time t;Signal Gain" << endl;
    outputPathLoss.open("../Plots/data/ultrasound_output_loss.csv");
	outputPathLoss << "Time t;Signal Path Loss" << endl;
    outputSendSignal.open("../Plots/data/ultrasound_output_send_signal.csv");
	outputSendSignal << "Time t;Signal Amplitude" << endl;
	outputDemodRecvSignal.open("../Plots/data/ultrasound_output_demod_recv_signal.csv");
	outputDemodRecvSignal << "NanobotID;Bit" << endl;
	outputDemodSignal.open("../Plots/data/ultrasound_output_demod_signal.csv");
	outputDemodSignal << "NanobotID;Bit" << endl;
	outputRecvSignal.open("../Plots/data/ultrasound_output_recv_signal.csv");
	outputRecvSignal << "Time t;Amplitude" << endl;
	outputDelay.open("../Plots/data/ultrasound_output_delay.csv");
	outputDelay << "Time t;Delay" << endl;

    
	numSamples = samples;
	numPeriods = periods;
	numBits = upsampledBitSequence;
}

/**
* @brief Destroy the Ultrasound::Ultrasound object
* Close the output files safely upon end of simulation
*/
Ultrasound::~Ultrasound(void)
{
	outputGain.flush();
    outputGain.close();
    outputPathLoss.flush();
    outputPathLoss.close();
    outputSendSignal.flush();
    outputSendSignal.close();
	outputDemodRecvSignal.flush();
	outputDemodRecvSignal.close();
	outputDemodSignal.flush();
	outputDemodSignal.close();
	outputRecvSignal.flush();
	outputRecvSignal.close();
	outputDelay.flush();
	outputDelay.close();
}


/**
 * @brief Read csv files and save the values in a vector
 * 
 * @param filename  the file to read from
 * @return std::vector<double> containing the values from the file
 */
std::vector<double> Ultrasound::readVectorFromFile(std::string filename)
{
	filename = "../Ultrasonic_channel_C_code/" + filename;

	std::ifstream fin;
	fin.open(filename);

	std::cout<<"opened the file correctly"<< std::endl;

	std::vector<double> helper;

	std::string line, word;

	while(fin.good()){
		fin>>line;
		std::stringstream s(line);
		while(s.good()){
			getline(s,word,',');
			double value = std::stod(word);
			helper.push_back(value);
		} 
	}

	fin.close();

	return helper;

}

/**
 * @brief convert the delay value from the matlab simulations to micro seconds
 * according to the baseband sampling frequency
 * 
 * @param delay  the value to be converted
 * @return double delay converted to micro seconds
 */
double Ultrasound::convertDelayVectorToMicroSeconds(double delay)
{
	return delay * pow(10,6) / this->fir_fs;
}

/**
 * @brief convert the linear gain value to power loss 
 * 
 * @param value linear gain
 * @return double power loss in dB
 */
double Ultrasound::convertGainToPowerLoss(double value)
{
	return -10 * log10(value);
}


/**
 * @brief compute the doppler term
 * 
 * @param velocity nanobot's velocity
 * @param phi angle of nanobot to gateway according to direction of movement
 * @return double the computed value for the doppler term
 */
double
Ultrasound::dopplerTerm(double velocity, double phi)
{
	return velocity * phi * center_frequency/c_0;
}


/**
 * @brief ultrasound_module to be called from the BVS simulation if in the Left Heart
 * looking up the gain, delay and angle per time step
 * computing the doppler term per time step
 * writing delay, gain, sent signal, received signal and demodulated signal to csv files for plotting
 * 
 * @param NanoID the nanobots ID
 * @param velocity  nanobots velocity
 */
void Ultrasound::ultrasound_module(double NanoID, double velocity) {
	cout << "Ultrasound" << endl;
	vector <double> receivedSignal;
	double delta_t = 1 / (center_frequency * numSamples);
	
    transceiver = new Modulation(numBits, numSamples, numPeriods, center_frequency, delta_t); 
	vector<int> upsampledBitSequence = transceiver->generateBitSequence();
	
	double timeWindow = 2 * sqrt(pow(d_max,2)-pow(d_min,2)) / velocity;
	int maxNumSamples = 5000;
	for(int i = 0; i <= maxNumSamples; i++){
		double gain_m = gain[i];
		double delay_m = delay[i];
		double cos_angle_m = cos_angle[i];
		
		double doppler_term = dopplerTerm(velocity,cos_angle_m);
		
		double t = (double)i * timeWindow / maxNumSamples;
		// transmitter
		double sendSignal = upsampledBitSequence[i] * transceiver->transmitter(t);
		
		//receiver
		double recvSignal = upsampledBitSequence[i] * transceiver->receiver(t, doppler_term, delay_m) * gain_m;
		receivedSignal.push_back(recvSignal);

		outputDelay << t << ";" << delay[i] << endl;
		outputGain << t << ";"  << convertGainToPowerLoss(gain_m) << endl;
        outputPathLoss << t << ";"  << convertGainToPowerLoss(pow(gain_m, 2)) << endl;
        outputSendSignal << t << ";" << sendSignal << endl;
		outputRecvSignal << t << ";" << recvSignal << endl;
		
	}
	
    // demodulating
	vector<double> filteredRecv = transceiver->filter(receivedSignal, numPeriods, numSamples);
	for (int i = 0; i < (int)filteredRecv.size(); i++) {
		outputDemodRecvSignal << NanoID << ";" << filteredRecv[i] << endl;
	}

	vector<int> receivedBits = transceiver->demodulation(filteredRecv);
	for (int i = 0; i < (int)receivedBits.size(); i++) {
		outputDemodSignal << NanoID << ";" << receivedBits[i] << endl;
	}
}
};
