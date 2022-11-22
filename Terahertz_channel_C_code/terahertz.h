/**
 * @file terahertz.h
 * @author Philip Nys (p.nys@campus.tu-berlin.de)
 * @brief External Terahertz module
 * @version 1.0
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef CLASS_TERAHERTZ_
#define CLASS_TERAHERTZ_

#include "ns3/object.h"
#include "ns3/vector.h"
#include "../BPSK_Transceiver_C_code/modulation.h"
#include <complex>
#include <fstream>

class Modulation;

using namespace std;
namespace ns3 {

class Terahertz : public ns3::Object
{
private:
	const double m_skin_thickness = 1e-4;
	const double m_fat_thickness = 1.25e-3;
	const double m_blood_thickness = 2.7e-3;

	const double full_thickness = 4.05e-3; 
	
	const double left_hand_gateway_x = 0.2;
	const double right_hand_gateway_x = -0.3;
	const double gateway_y = -0.4273;
	const double gateway_z = 0;
	
	const double d_min = m_skin_thickness + m_fat_thickness + m_blood_thickness;
	//const double d_max = 6.83e-3;
	const double d_max = 6.915e-3;
	const double frequency = 1e10;
	
	const double ALPHA_1_DERMIS = 0.92;
	const double ALPHA_1_EPIDERMIS = 0.95;

	const double ALPHA_2_DERMIS = 0.97;

	const double BETA_1_DERMIS = 0.8;
	const double BETA_1_EPIDERMIS = 0.96;

	const double BETA_2_DERMIS = 0.99;

	const double EPSILON_0 = 8.85e-12;

	const double EPSILON_INF_BLOOD = 2.1;
	const double EPSILON_INF_DERMIS = 4.0;
	const double EPSILON_INF_EPIDERMIS = 3.0;

	const double EPSILON_1_BLOOD = 130.0;
	const double EPSILON_1_DERMIS = 5.96;
	const double EPSILON_1_EPIDERMIS = 89.61;

	const double EPSILON_2_BLOOD = 3.8;
	const double EPSILON_2_DERMIS = 380.4;

	const double TAU_1_BLOOD = 14.4e-12;
	const double TAU_1_DERMIS = 1.6e-12;
	const double TAU_1_EPIDERMIS = 15.9e-12;

	const double TAU_2_BLOOD = 0.1e-12;
	const double TAU_2_DERMIS = 159.0e-9;

	const double SIGMA_DERMIS = 0.1;
	const double SIGMA_EPIDERMIS = 0.01;

	const double c_0 = 299792458.0;
	
	int numSamples;
	int numPeriods;
	int numBits;

	ofstream outputGain;
	ofstream outputPathLoss;
	ofstream outputTest;
	
    ofstream outputBitSequence;
    ofstream outputSendSignal;
	ofstream outputDemodRecvSignal;
	ofstream outputDemodSignal;
	ofstream outputRecvSignal;

	Ptr<Modulation> transceiver;

public:

	Terahertz(int samples, int periods, int bits);
	
	~Terahertz(void);

	double convertTodB(double x);
	double angularFrequency(double f);
	double effectiveWavelength(double lambda, double n_1);
	double molecularAbsorptionCoefficient(double lambda_g, double n_2);
	double spreadingLoss(double lambda_g, double d);
	double absorptionLoss(double mu_abs, double d);
	double totalPathLoss(double lambda, double n_1, double n_2, double d);
	double pathLossBlood(double d, double omega, double lambda);
	double pathLossFat(double d, double omega, double lambda);
	double pathLossSkin(double d, double omega, double lambda);
	double totalPathLossTotal(double blood, double fat, double skin);

	double signalGain(double z_t, double d_t);
	double dopplerTerm(double velocity, double z_t, double d_t);
	void terahertz_module(int NanoID, double velocity);
};
};
#endif //TERAHERTZ_H
