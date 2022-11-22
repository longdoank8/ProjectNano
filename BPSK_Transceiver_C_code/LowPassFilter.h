/**
 * @file LowPassFilter.h
 * @author Jimmy van den Berg (https://github.com/jimmyberg)
 * @brief Low Pass Filter
 * @version 0.1
 * @date 2022-07-27
 * 
 * @copyright https://github.com/jimmyberg/LowPassFilter
 * 
 */
#ifndef _LowPassFilter_h_
#define _LowPassFilter_h_

#include "ns3/object.h"
#include <cmath>

using namespace std;
namespace ns3 {
class LowPassFilter : public ns3::Object
{
public:
	//constructors
	LowPassFilter();
	LowPassFilter(double iCutOffFrequency, double iDeltaTime);
    //destructor
    ~LowPassFilter();
	//functions
	double update(double input);
	double update(double input, double deltaTime, double cutoffFrequency);
	//get and configure funtions
	double getOutput() const{return output;}
	void reconfigureFilter(double deltaTime, double cutoffFrequency);
private:
	double output;
	double ePow;
};
};
#endif //_LowPassFilter_h_