/**
 * @file terahertz.cc
 * @author Philip Nys (p.nys@campus.tu-berlin.de)
 * @brief External Terahertz module
 * @version 1.0
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "terahertz.h"
#include <iostream>
#include <cmath>
#include <complex>

using namespace std;
namespace ns3 {

    /**
     * @brief Construct a new Terahertz:: Terahertz object
     * 
     * @param samples Number of samples per period
     * @param periods Number of periods per bit
     * @param bits Number of bits per signal
     */
	Terahertz::Terahertz(int samples, int periods, int bits)
	{
        outputBitSequence.open("../Plots/data/output_bit_sequence.csv");
        outputBitSequence << "NanobotID;Bit" << endl;
        outputPathLoss.open("../Plots/data/terahertz_output_path_loss.csv");
        outputPathLoss << "Time t;Z Position z_t;Signal Path Loss" << endl;
        outputGain.open("../Plots/data/terahertz_output_gain.csv");
        outputGain << "Time t;Z Position z_t;Signal Gain" << endl;
        outputSendSignal.open("../Plots/data/terahertz_output_send_signal.csv");
        outputSendSignal << "Time t;Z Position z_t;Signal Amplitude" << endl;
        outputRecvSignal.open("../Plots/data/terahertz_output_recv_signal.csv");
        outputRecvSignal << "Time t;Amplitude" << endl;
        outputDemodRecvSignal.open("../Plots/data/terahertz_output_demod_recv_signal.csv");
        outputDemodRecvSignal << "NanobotID;Bit" << endl;
        outputDemodSignal.open("../Plots/data/terahertz_output_demod_signal.csv");
        outputDemodSignal << "NanobotID;Bit" << endl;
        outputTest.open("../Plots/data/terahertz_output_test.csv");
        outputTest << "Placeholder" << endl;

        numSamples = samples;
	    numPeriods = periods;
	    numBits = bits;
    }
	
    /**
     * @brief Destroy the Terahertz:: Terahertz object
     * 
     */
	Terahertz::~Terahertz(void){
        outputBitSequence.flush();
        outputBitSequence.close();
        outputPathLoss.flush();
        outputPathLoss.close();
        outputGain.flush();
        outputGain.close();
        outputSendSignal.flush();
        outputSendSignal.close();
        outputRecvSignal.flush();
        outputRecvSignal.close();
        outputDemodRecvSignal.flush();
        outputDemodRecvSignal.close();
        outputDemodSignal.flush();
        outputDemodSignal.close();
        outputTest.flush();
        outputTest.close();
    }

    /**
     * @brief Convert a given number to dB.
     * 
     * @param x Number
     * @return Number \p x converted to dB 
     */
    double 
    Terahertz::convertTodB(double x){
        return 10 * log10(x);
    }

    /**
     * @brief Converts a given frequency to its angualar frequency.
     * 
     * @param f Frequency
     * @return Angular frequency of \p f
     */
    double 
    Terahertz::angularFrequency(double f){
        return 2 * M_PI * f;
    }

    /**
     * @brief Computes the effective wavelength.
     * 
     * @param lambda Wavelength
     * @param n_1 Real part of refractive index
     * @return effective Wavelength 
     */
    double 
    Terahertz::effectiveWavelength(double lambda, double n_1){
        return lambda / n_1;
    }

    /**
     * @brief Computes the molecular absorption coefficient.
     * 
     * @param lambda_g Effective wavelength
     * @param n_2 Imaginary part of refractive index
     * @return Molecular absorption coefficient 
     */
    double 
    Terahertz::molecularAbsorptionCoefficient(double lambda_g, double n_2){
        return (4 * M_PI * n_2) / lambda_g;
    }

    /**
     * @brief Computes the spreading loss.
     * 
     * @param lambda_g Effective wavelength
     * @param d Travel distance of signal
     * @return Spreading loss 
     */
    double 
    Terahertz::spreadingLoss(double lambda_g, double d){
        return pow((4 * M_PI * d) / lambda_g, 2);
    }

    /**
     * @brief Computes the absoption loss.
     * 
     * @param mu_abs Molecular absorption coefficient
     * @param d Travel distance of signal
     * @return Absorption loss 
     */
    double 
    Terahertz::absorptionLoss(double mu_abs, double d){
        return exp(mu_abs * d);
    }

    /**
     * @brief Computes the total path loss.
     * 
     * @param lambda Wavelength
     * @param n_1 Real part of refractive index
     * @param n_2 Imaginary part of refractive index
     * @param d Travel distance of signal
     * @return Total path loss of signal with given parameters 
     */
    double 
    Terahertz::totalPathLoss(double lambda, double n_1, double n_2, double d){
        double lambda_g = effectiveWavelength(lambda, n_1);

        double mu_abs = molecularAbsorptionCoefficient(lambda_g, n_2);

        double L_spr = spreadingLoss(lambda_g, d);

        double L_abs = absorptionLoss(mu_abs, d);
        
        return L_abs * L_spr;
    }
    
    /**
     * @brief Computes the total path loss for the blood layer.
     * 
     * @param d Travel distance of signal
     * @param omega Angular frequency of signal
     * @param lambda Wavelength
     * @return Total path loss in blood medium 
     */
    double 
    Terahertz::pathLossBlood(double d, double omega, double lambda){
        const complex<double> I(0.0, 1.0);
        // refractive index
        complex<double> sum1 = (EPSILON_1_BLOOD - EPSILON_2_BLOOD) / (1.0 + I * omega * TAU_1_BLOOD);
        complex<double> sum2 = (EPSILON_2_BLOOD - EPSILON_INF_BLOOD) / (1.0 + I * omega * TAU_2_BLOOD);
        complex<double> epsilon_r = EPSILON_INF_BLOOD + sum1 + sum2;

        complex<double> n = sqrt(epsilon_r);
        double n_1 = real(n);
        double n_2 = -imag(n);

        return totalPathLoss(lambda, n_1, n_2, d);
    }
    
    /**
     * @brief Computes the total path loss for the fat layer.
     * 
     * @param d Travel distance of signal
     * @param omega Angular frequency of signal
     * @param lambda Wavelength
     * @return double Total path loss in fat medium
     */
    double 
    Terahertz::pathLossFat(double d, double omega, double lambda){
        const complex<double> I(0.0, 1.0);
        // refractive index dermis
        complex<double>sum1 = EPSILON_1_DERMIS / pow(1.0 + pow(I * omega * TAU_1_DERMIS, ALPHA_1_DERMIS), BETA_1_DERMIS);
        complex<double>sum2 = EPSILON_2_DERMIS / pow(1.0 + pow(I * omega * TAU_2_DERMIS, ALPHA_2_DERMIS), BETA_2_DERMIS);
        complex<double> epsilon_r = EPSILON_INF_DERMIS + sum1 + sum2 - I * SIGMA_DERMIS / (omega * EPSILON_0);

        complex<double> n = sqrt(epsilon_r);
        double n_1 = real(n);
        double n_2 = -imag(n);

        return totalPathLoss(lambda, n_1, n_2, d);
    }
    
    /**
     * @brief Computes the total path loss for the skin layer.
     * 
     * @param d Travel distance of signal
     * @param omega Angular frequency of signal
     * @param lambda Wavelength
     * @return double Total path loss in skin medium
     */
    double 
    Terahertz::pathLossSkin(double d, double omega, double lambda){
        const complex<double> I(0.0, 1.0);
        // refractive index epidermis
        complex<double>sum1 = EPSILON_1_EPIDERMIS / pow(1.0 + pow(I * omega * TAU_1_EPIDERMIS, ALPHA_1_EPIDERMIS), BETA_1_EPIDERMIS);
        complex<double>sum2 = I * SIGMA_EPIDERMIS / (omega * EPSILON_0);
        complex<double> epsilon_r = EPSILON_INF_EPIDERMIS + sum1 - sum2;

        complex<double> n = sqrt(epsilon_r);
        double n_1 = real(n);
        double n_2 = -imag(n);

        return totalPathLoss(lambda, n_1, n_2, d);
    }

    /**
     * @brief Adds up all Path Losses.
     * 
     * @param blood Path loss of blood
     * @param fat Path loss of fat
     * @param skin Path loss of skin
     * @return Total path loss of the full distance between nanobot and gateway 
     */
    double 
    Terahertz::totalPathLossTotal(double blood, double fat, double skin){
        return blood * fat * skin;
    }

    /**
     * @brief Computes the signal gain.
     * 
     * @param z_t Current position along the z axis
     * @param d_t Current distance from nanobot to gateway
     * @return double Signal gain
     */
    double
    Terahertz::signalGain(double z_t, double d_t) {
        double sin_phi = sin(M_PI - M_PI / 2 - acos(d_min / d_t));
        double skin_distance = m_skin_thickness / sin_phi;
        double fat_distance = m_fat_thickness / sin_phi;
        double blood_distance = m_blood_thickness / sin_phi;
        double omega = angularFrequency(frequency); 

        double lambda = c_0 / frequency;

        double blood = pathLossBlood(blood_distance, omega, lambda);
        double fat = pathLossFat(fat_distance, omega, lambda);
        double skin = pathLossSkin(skin_distance, omega, lambda);

        double L_tot = totalPathLossTotal(blood, fat, skin);

        return sqrt(L_tot);
    }

    /**
     * @brief Computes the doppler effect term.
     * 
     * @param velocity Velocity of nanobot in current blood vessel
     * @param z_t Current position along the z axis
     * @param d_t Current distance from nanobot to gateway
     * @return double Doppler effect term
     */
    double
    Terahertz::dopplerTerm(double velocity, double z_t, double d_t) {
        double cos_phi = abs(z_t) / d_t;
        return velocity * cos_phi * frequency / c_0;
    }

    /**
     * @brief Simulate a signal with Terahertz frequncy
     * 
     * @param NanoID Current ID of nanobot
     * @param velocity Velocity of nanobot in current blood vessel
     */
    void 
    Terahertz::terahertz_module(int NanoID, double velocity) {
	    cout << "Terahertz" << endl;
        vector<double> receivedSignal;
        double delta_t = 1 / (frequency * numSamples);
        
        // starting and end point of cone
        double z_0 = sqrt(pow(d_max, 2) - pow(d_min, 2));
            
        // Transceiver
        transceiver = new Modulation(numBits, numSamples, numPeriods, frequency, delta_t);

        // generate bits
        vector<int> upsampledBitSequence = transceiver->generateBitSequence();
        // save to file
        for (int i = 0; i < (int)upsampledBitSequence.size(); i++) {
            outputBitSequence << NanoID << ";" << upsampledBitSequence[i] << endl;
        }
        
        // maximum possible number of samples
        int maxNumSamples = numSamples * numPeriods * numBits;
        for (int i = 0; i <= maxNumSamples; i++) {
            // time steps
            double t = (double)i * delta_t;
            // current position along the blood vessel
            double z_t = -z_0 + velocity * t;
            // current distance between gateway and nanobot
            double d_t = sqrt(pow(z_t, 2) + pow(d_min, 2));

            double gain = signalGain(z_t, d_t);
            double doppler = dopplerTerm(velocity, z_t, d_t);

            // transmitter
            double sendSignal = upsampledBitSequence[i] * transceiver->transmitter(t);
            
            // receiver
            double recvSignal = upsampledBitSequence[i] * transceiver->receiver(t, doppler) / gain;
            receivedSignal.push_back(recvSignal);
            
            outputGain << t << ";" << z_t << ";" << convertTodB(gain) << endl;
            outputPathLoss << t << ";" << z_t << ";" << convertTodB(pow(gain, 2)) << endl;
            outputSendSignal << t << ";" << z_t << ";" << sendSignal << endl;
            outputRecvSignal << t << ";" << z_t << ";" << recvSignal << endl;
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
}
