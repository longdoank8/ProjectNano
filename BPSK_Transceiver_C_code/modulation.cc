/**
 * @file modulation.cc
 * @author Long Kaestner (you@domain.com)
 * @brief 
 * @version 1.0
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "modulation.h"
#include <iostream>
#include <cmath>
#include <complex>

using namespace std;
namespace ns3 {

    /**
     * @brief Construct a new Modulation:: Modulation object
     * 
     * @param numBits Number of bits per signal
     * @param numSamples number of data points per "cos/sin" wave
     * @param numPeriods number of "cos/sin" waves per bit
     */
	Modulation::Modulation(int numBits, int numSamples, int numPeriods, double f, double Delta_t)
	{
        num = numBits;
        samplesPerPeriod = numSamples;
        periodsPerBit = numPeriods;
        frequency = f;
        L = samplesPerPeriod * periodsPerBit;   // number of samples per bit
        delta_t = Delta_t;
        LPF = new LowPassFilter(f, Delta_t);
    }
	
    /**
     * @brief Destroy the Modulation:: Modulation object
     * 
     */
	Modulation::~Modulation(void){
    }

    // BEGIN -- BPSK - Transmitter
    // -----------------------------------------------------------------

    /**
     * @brief Convert Bits. 1 => 1 and 0 => -1
     * 
     * @param arr Array of bits
     * @return Converted array
     */
    vector<int> 
    Modulation::MultiplyArrayByTwo(vector<int> arr){ // level converter
        for(int i = 0; i < (int)arr.size(); i++) {
            arr[i] = arr[i]==1? 1: -1; // if == 1 true then 1 else -1
        }
        return arr; 
    }

    /**
     * @brief Upsample bits sequence
     * 
     * @param arr Array of bits
     * @return Upsampled array
     */
    vector<int> 
    Modulation::upsample(vector<int> arr){ // upsampling
        vector<int> temp;
        for(int i = 0; i < (int)arr.size(); i++){
            for(int j = 0; j<L ; j++){
                temp.push_back(arr[i]);
            }
        }
        return temp;
    }
    
    /**
     * @brief Converted bits to signal as cosine wave
     * 
     * @param arr Array of bits
     * @return Bits converted to signal 
     */
    vector<double> 
    Modulation::signal(vector<int> arr, int doppler){
        vector<double> s;
        for(int i = 0; i< (int)arr.size(); i++){
            s.push_back(arr[i] * cos(20 * M_PI * i / L + 2 * M_PI * doppler));
        }
        return s;
    }

    // END -- BPSK Transmitter
    // -----------------------------------------------------------------



    // BEGIN -- BPSK - Receiver
    // -----------------------------------------------------------------

    /**
     * @brief send signal multiplied by cos
     * 
     * @param arr Received signal
     * @return out1 
     */
    vector<double> 
    Modulation::MultiplyByCos(vector<double> arr, double numPeriods, double numSamples){
    vector<double> a;
    for(int i = 0; i < (int)arr.size(); i++) {
        double t = (double)i / ((frequency / numPeriods) * numSamples);
        a.push_back(arr[i]*cos(2*M_PI* t * frequency));
    }
    return a;
}

    /**
     * @brief send signal multiplied by sin
     * 
     * @param arr Received signal
     * @return out2 
     */
    vector<double> 
    Modulation::MultiplyBySin(vector<double> arr, double numPeriods, double numSamples){
    vector<double> a;
    for(int i = 0; i < (int)arr.size(); i++) {
        double t = (double)i / ((frequency / numPeriods) * numSamples);
        a.push_back(arr[i]*sin(2*M_PI* t * frequency));
    }
    return a;
}

    /**
     * @brief adding out1 and out2 = output
     * 
     * @param arr Array 1
     * @param arra Array 2
     * @return combined array out = out1 + out2
     */
    vector<double> 
    Modulation::Add(vector<double> arr, vector<double> arra){
        vector<double> sum;
        for(int i = 0; i< (int)arr.size(); i++){
            sum.push_back(arr[i] + arra[i]);
        }
        return sum;
    }

    /**
     * @brief convolution to retrieve bits 
     * 
     * Source: https://stackoverflow.com/questions/24518989/how-to-perform-1-dimensional-valid-convolution
     * 
     * @tparam T extra vector declaration 
     * @param f 1D vector of output signal 
     * @param g 1D vector of time duration of bits
     * @return convoluted vector
     */
    template<typename T>
    std::vector<T>
    Modulation::conv(std::vector<T> const &f, std::vector<T> const &g) {
        int const nf = f.size();
        int const ng = g.size();
        int const n  = nf + ng - 1;
        std::vector<T> out(n, T());
        for(auto i(0); i < n; ++i) {
            int const jmn = (i >= ng - 1)? i - (ng - 1) : 0;
            int const jmx = (i <  nf - 1)? i            : nf - 1;
            for(auto j(jmn); j <= jmx; ++j) {
                out[i] += (f[j] * g[i - j]);
            }
        }
        return out; 
    }

    /**
     * @brief remove upsampling to get original bit sequence
     * 
     * @param arr of convoluted vector
     * @return vector without upsampling 
     */
    vector<int> 
    Modulation::sample(vector<double> arr){
        vector<int> tmp;
        for(int i = L-1; i< (int)arr.size(); i = i+L){
            tmp.push_back(arr[i]);
        }
        return tmp;
    }  

    /**
     * @brief Convert bits. >0 => 1, else 0
     * 
     * @param arr 
     * @return original bit sequence 
     */
    vector<int> 
    Modulation::transform(vector<int> arr){ // transform back
        for(int i = 0; i < (int)arr.size(); i++) {
            arr[i] = arr[i]>0? 1: 0; // if > 0 true then 1 else 0
        }
        return arr; 
    }

    // END -- BPSK - Receiver
    // -----------------------------------------------------------------

    /**
     * @brief Generate vector for convolution
     * 
     * @return time vector of duration of bits
     */
    vector<double> 
    Modulation::vec(){
        vector<double> v;
        int o = delta_t;
        for(int i = 0; i<L; i++){
            v.push_back(o);
        }
        return v;
    }

    /**
     * @brief Generate random bit sequence
     * 
     * @param n Number of bits
     * @return Array of randomized bits 
     */
    vector<int> 
    Modulation::generateRandomBitSequence(int n) {
        srand(420);
        vector<int> arr;
        for (int i = 0; i < num; i++){
            arr.push_back(rand() % 2);
        }
        return arr;
    }

    /**
     * @brief Print vector
     * 
     * @param vec Vector<double>
     */
    /*template<typename T> 
    void 
    Modulation::printVector(std::ofstream& file, int ID, vector<T> vec) {
        for (int i = 0; i < (int)vec.size(); i++) {
            std::cout << ID << ";" << vec[i] << endl;
        }
    }*/

    /**
     * @brief Generate bit sequence and upsample
     * 
     * @param randomBits true: random, false: provide predefined bits (WIP)
     * @return Array of bits
     * 
     */
    vector<int> 
    Modulation::generateBitSequence() {
        vector<int> vect = generateRandomBitSequence(num);
        vector<int> sbb = MultiplyArrayByTwo(vect);
        vector<int> upsampled = upsample(sbb);

        return upsampled;
    }

    /**
     * @brief Transmitter procedure
     * 
     * @param bitSequence Signal as array of integers
     * @return outgoing signal before attenuation and doppler adjustment 
     */
    double 
    Modulation::transmitter(double t) {
        return cos(2 * M_PI * t * frequency);
    }

    /**
     * @brief Transmitter procedure
     * 
     * @param bitSequence Signal as array of integers
     * @return outgoing signal before attenuation and doppler adjustment 
     */
    double 
    Modulation::receiver(double t, double doppler, double delay) {
        return cos(2 * M_PI * t * frequency + 2 * M_PI * doppler + delay);
    }

    /**
     * @brief Receiver + Demodulation procedure
     * 
     * @param receivedSignal Signal received
     * @return Demodulated received signal 
     */
    vector<double>
    Modulation::filter(vector<double> receivedSignal, double numPeriods, double numSamples) {
        vector<double> d = MultiplyByCos(receivedSignal, numPeriods, numSamples);
        vector<double> h = MultiplyBySin(receivedSignal, numPeriods, numSamples);
        // Low pass filter
        for (int i = 0; i < (int)d.size(); i++) {
            d[i] = LPF->update(d[i]);
        }
        for (int i = 0; i < (int)h.size(); i++) {
            h[i] = LPF->update(h[i]);
        }
        vector<double> sum = Add(d, h);
        return sum;
    }

    vector<int>
    Modulation::demodulation(vector<double> signal) {
        vector<double> t = vec();
        vector<double> b = conv(signal,t); 
        vector<int> e = sample(signal);
        vector<int> dsignal = transform(e);
        return dsignal;
    }
}
