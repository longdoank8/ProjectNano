#include <iostream>
#include <math.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <cassert>


std::vector <double> readVectorFromFile(std::string filename)
{
	std::ifstream fin;
	fin.open(filename);

	std::cout<<"opened the file correctly"<< std::endl;


	std::vector<double> helper;

	std::string line, word;

	while(!fin.eof()){
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

std::vector<double> convertDelayVectorToMicroSeconds(std::vector<double> delay, double sampling_frequency)
{
	std::vector<double> convertedVector;
	double helper;
	for(int i=0; i < delay.size(); i++)
       	{

		helper = delay[i] *(1/sampling_frequency) * pow(10,6);
		convertedVector.push_back(helper);
	}
	return convertedVector;	
}

std::vector<double> convertGainToPowerLoss(std::vector<double> gain)
{
	std::vector<double> convertedVector;
	double helper;
	for(int i=0; i<gain.size();i++)
	{
		helper= -20 * log10(gain[i]);
		convertedVector.push_back(helper);
	}
	return convertedVector;
}

double getMatchingPowerLoss(std::map<double,double> distance_powerLoss, double distance)
{
	//TODO if distance is not in map, print next higher or lower value
	if(distance_powerLoss.count(distance)>0){
		return distance_powerLoss.find(distance)->second;
	}
	else
	{
	        return 0; 
	}
	
}

double getMatchingDelay(std::map<double,double> distance_delay, double distance)
{
	if(distance_delay.count(distance)>0){
		return distance_delay.find(distance)->second;
	}
	else
	{
		return 0;
	}
}

std::map<double,double> createMap(std::vector<double> key, std::vector<double> value)
{
	std::map<double, double> keyValueMap;
	assert(key.size() == value.size());
	for(size_t i = 0; i < key.size(); ++i)
	{
		keyValueMap[key[i]] = value[i];
	}

	return keyValueMap;

}

int main() {

	double propagation_speed_us_waves = 1480; //meter per second
	double center_frequency = 1e6; // in Hz
	double fir_fs = 5.0000e+06; // in Hz
	double velocity =  0.1; // nanosensor speed in meter per seconds (aorta, arteries and veins) 
	//		     * <- gateway
	//		    /|
	//	    80mm-> / | 20mm
	//		  /  |
	//		 *   * <- nanobot
	//		 |_______|		c^2 = a^2 +b^2 -> c = opposite from 90 degree angle
	//		 Communication distance: 2 * sqrt(80^2 - 20^2)
	double distance_min_measure = 0.0020; //minimum distance in millimeter between nanobot and gateway in m
	double distance_max_measure = 0.0080; //maximum distance in millimeter between nanobot and gateway in m
	double distance_communication = 2 * sqrt(pow(distance_max_measure,2)-pow(distance_min_measure,2)); // distance where nanobot is in communication range to the gateway in m 
	long unsigned int total_steps = pow(10,5); //time
	double gateway_x = (distance_communication/2);
	double gateway_y = distance_min_measure;

	double vector_nanosensor[2] = {1, 0}; // direction of movement is x-axis
	double vector_nano_gtw [2] = {}; // vector gateway to nanobot
	double cos_angle [total_steps] = {};
	double nu [total_steps] = {};
	double dist_nanosensor_gtw [total_steps] = {};
	std::cout << "Computing the doppler effect of mobile nanosensors" << std::endl;
	double doppler_factor_per_velocity [total_steps] = {}; 
	//TODO consider 3D coordinates for calculation
	for(int j=0; j<total_steps; j++){
		vector_nano_gtw[0] = gateway_x - nano_sensor_x[j];
		vector_nano_gtw[1] = gateway_y - nano_sensor_y[j]; 
		dist_nanosensor_gtw[j] = sqrt(pow(gateway_x - nano_sensor_x[j], 2) + pow(gateway_y - nano_sensor_y[j],2));
		cos_angle[j] = (vector_nanosensor[0]*vector_nano_gtw[0] + vector_nanosensor[1]*vector_nano_gtw[1])/dist_nanosensor_gtw[j];
		doppler_factor_per_velocity[j] = velocity * cos_angle[j] * ( center_frequency / propagation_speed_us_waves);
		std::cout << "Computed doppler effect of mobile nanosensor is: " << doppler_factor_per_velocity[j] << std::endl;
	}


	double time[total_steps] = {};
	double nano_sensor_x [total_steps] = {};
	double nano_sensor_y [total_steps] = {};
	for (int i = 0; i<total_steps; i++){
		nano_sensor_x [i] = i * (distance_communication / total_steps);
		nano_sensor_y [i] = 0;
		time[i] = distance_communication/velocity/total_steps * (i);
	}	
	
	std::vector<double> gain = readVectorFromFile("gain_vector.csv");
	std::vector<double> delay = readVectorFromFile("delay_vector.csv");
 	std::vector<double> distance = readVectorFromFile("distance_vector.csv");

	std::vector<double> powerLoss = convertGainToPowerLoss(gain);
	std::vector<double> delayConverted = convertDelayVectorToMicroSeconds(delay, fir_fs);

	std::map<double, double> distance_delay = createMap(distance,delayConverted);
	std::map<double, double> distance_powerLoss = createMap(distance, powerLoss);
		
	double distanceToFind = 0;
	double matchedPowerLoss = getMatchingPowerLoss(distance_powerLoss, distanceToFind);
	double matchedDelay = getMatchingDelay(distance_delay, distanceToFind);

	std::cout << "The value for matched power loss is:" << matchedPowerLoss << std::endl;
	std::cout << "The value for match delay is:" << matchedDelay << std::endl;


}
