/**
 * @file modulation.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 1.0
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef CLASS_MODULATION_
#define CLASS_MODULATION_

#include "ns3/object.h"

#include <iostream>
#include <random>
#include <ctime>
#include <time.h>
#include <cmath>
#include <math.h>
#include <array>
#include <fstream>
#include <vector>
#include <iterator>
#include <algorithm>

#include "LowPassFilter.h"

class LowPassFilter;

using namespace std;
namespace ns3 {
//class Modulation;
class Modulation : public ns3::Object
{
private:
    int num;
    //const int defaultBits[] = {0, 1};
    int samplesPerPeriod;
    int periodsPerBit;
    double frequency;
    double delta_t;
    int L;                  // number of samples per bit (samplesPerPeriod * periodsPerBit)
    Ptr<LowPassFilter> LPF;

public:

	Modulation(int numBits, int numSamples, int numPeriods, double f, double delta_t);
	
	~Modulation(void);

    vector<int> generateRandomBitSequence(int n);
    //template<typename T> void printVector(std::ofstream& file, int ID, vector<T> vec);

    // BEGIN -- BPSK - Transmitter
    // -----------------------------------------------------------------

    vector<int> MultiplyArrayByTwo(vector<int> arr);
    vector<int> upsample(vector<int> arr);
    vector<double> signal(vector<int> arr, int doppler);

    // END -- BPSK Transmitter
    // -----------------------------------------------------------------


    // BEGIN -- BPSK - Receiver
    // -----------------------------------------------------------------

    vector<double> MultiplyByCos(vector<double> arr, double numPeriods, double numSamples);
    vector<double> MultiplyBySin(vector<double> arr, double numPeriods, double numSamples);
    vector<double> Add(vector<double> arr, vector<double> arra);
    template<typename T> std::vector<T> conv(std::vector<T> const &f, std::vector<T> const &g);
    vector<int> sample(vector<double> arr);
    vector<int> transform(vector<int> arr);

    // END -- BPSK - Receiver
    // -----------------------------------------------------------------

    void main();
    vector<int> generateBitSequence();
    double transmitter(double t);
    double receiver(double t, double doppler, double delay = 0);
    vector<double> filter(vector<double> receivedSignal, double numPeriods, double numSamples);
    vector<int> demodulation(vector<double> signal);


    // generate vector for convolution
    vector<double> vec();

};
};
#endif //CLASS_MODULATION_
