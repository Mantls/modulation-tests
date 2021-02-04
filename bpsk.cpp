#include <itpp/comm/modulator.h>
#include <itpp/comm/channel.h>
#include <itpp/base/math/trig_hyp.h>
#include <cmath>
#include <complex>
#include <iostream>
#include <bitset>
#include <string>
#include <vector>
#include <fstream>
#include "utils.h"
#include <math.h>
#include <itpp/comm/channel.h>
#include <sciplot/sciplot.hpp>
#include <itpp/comm/reedsolomon.h>
#include <itpp/comm/hammcode.h>
#include <itpp/comm/turbo.h>
#include <itpp/comm/error_counters.h>
#include <itpp/stat/misc_stat.h>

int main()
{   
    int F_SAMPLING = 44100;
    double T_SAMPLE = 1.F / F_SAMPLING;
    double carrier_freq = 19000;
    double omega = 2 * M_PI * carrier_freq; // 2*pi*f

    std::string message_string = "Hello, World! This has been encoded using BPSK Modulation!";
    auto message_bitstring = string_to_binary(message_string);
    auto message_bitvec = string_to_binary_vec(message_string);

    itpp::bvec bitvec(message_bitvec.size());
    for (int i = 0; i<bitvec.size();++i)
    {
        bitvec[i] = message_bitvec[i];
    }

    itpp::BERC berc; // Bit error counter 
    itpp::RNG_randomize();
    itpp::Stat statistics;
    itpp::BPSK bpsk;
    // itpp::Reed_Solomon fec(5,3);
    itpp::Hamming_Code fec(2); // Binary Hamming-Code for Forward Error Correction

    auto encoded = fec.encode(bitvec);
    auto modulated = bpsk.modulate_bits(encoded); // Modulated Signal

    itpp::vec input_vec(modulated.size());
    for (double i = 0; i<input_vec.size();++i)
    {
        input_vec[i] = i  * T_SAMPLE * omega;
    }
    itpp::vec cosine_vec = itpp::cos(input_vec);


    std::cout << "Performing Tests. 1000 iterations for Each SNR Value." << std::endl;
    std::cout << "-----------------------------------------------------" << std::endl;
    std::cout << std::endl;

    for (int SNR=0;SNR<40;SNR++)
    {   
        itpp::vec bit_error_rate(1000);
        for (int k=0; k<bit_error_rate.size();++k)
        {
            auto signal = cosine_vec;
            
            for (int i =0; i < modulated.size(); ++i)
            {
                signal[i] *= modulated[i]; // A(t)*cos(2*PI*f*t)
            }
            
            auto signal_rcvd = signal; // Copy signal so that we may compare the original to the transmitted and received signal
            itpp::vec noise = itpp::randn(signal_rcvd.size());
            signal_rcvd = signal_rcvd + noise/itpp::inv_dB(SNR);
            for (int i = 0; i<signal.size(); ++i)
            {
                signal_rcvd[i] /= cosine_vec[i];
            }
            
            auto demodulated = bpsk.demodulate_bits(signal_rcvd);
            auto decoded = fec.decode(demodulated);
            bit_error_rate[k] = berc.count_errors(bitvec,decoded);     

            std::vector<int> result(decoded.size());
            for (int i=0; i<result.size();++i)
            {
                result[i] = decoded[i];
            }

            
        }
        std::cout << "SNR: " << SNR << "dB. BER: " << get_average(bit_error_rate) << std::endl;
    }
    return 1;
}