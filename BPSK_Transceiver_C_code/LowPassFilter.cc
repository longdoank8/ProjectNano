/**
 * @file LowPassFilter.cc
 * @author Jimmy van den Berg (https://github.com/jimmyberg)
 * @brief Low Pass Filter
 * @version 0.1
 * @date 2022-07-27
 * 
 * @copyright https://github.com/jimmyberg/LowPassFilter
 * 
 */
#include "LowPassFilter.h"

#define ERROR_CHECK (true)

#if ERROR_CHECK
#include <iostream>
#endif

using namespace std;
namespace ns3 {
LowPassFilter::LowPassFilter():
	output(0),
	ePow(0){}

LowPassFilter::LowPassFilter(double iCutOffFrequency, double iDeltaTime):
	output(0),
	ePow(1-exp(-iDeltaTime * 2 * M_PI * iCutOffFrequency))
{
	#if ERROR_CHECK
	if (iDeltaTime <= 0){
		std::cout << "Warning: A LowPassFilter instance has been configured with 0 s as delta time.";
		ePow = 0;
	}
	if(iCutOffFrequency <= 0){
		std::cout << "Warning: A LowPassFilter instance has been configured with 0 Hz as cut-off frequency.";
		ePow = 0;
	}
	#endif
}

LowPassFilter::~LowPassFilter(){}

double LowPassFilter::update(double input){
	return output += (input - output) * ePow;
}

double LowPassFilter::update(double input, double deltaTime, double cutoffFrequency){
	reconfigureFilter(deltaTime, cutoffFrequency); //Changes ePow accordingly.
	return output += (input - output) * ePow;
}

void LowPassFilter::reconfigureFilter(double deltaTime, double cutoffFrequency){
	#if ERROR_CHECK
	if (deltaTime <= 0){
		std::cout << "Warning: A LowPassFilter instance has been configured with 0 s as delta time.";
		ePow = 0;
	}
	if(cutoffFrequency <= 0){
		std::cout << "Warning: A LowPassFilter instance has been configured with 0 Hz as cut-off frequency.";
		ePow = 0;
	}
	#endif
	ePow = 1-exp(-deltaTime * 2 * M_PI * cutoffFrequency);
}
}