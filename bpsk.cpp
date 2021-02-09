#include <itpp/comm/modulator.h>
#include <itpp/comm/channel.h>
#include <itpp/base/math/trig_hyp.h>
#include <cmath>
#include <complex>
#include <iostream>
#include <string>
#include <vector>
#include "utils.h"
#include <math.h>
#include <itpp/comm/reedsolomon.h>
#include <itpp/comm/hammcode.h>
#include <itpp/comm/error_counters.h>
#include <itpp/stat/misc_stat.h>
#include "libdsp/Biquad.h"


int main()
{
    int TEST_AMOUNT = 1000;
    int MIN_SNR = 2;
    int MAX_SNR = 12;
    int F_SAMPLING = 44100;
    double T_SAMPLE = 1.F / F_SAMPLING;
    double carrier_freq = 19000;
    double omega = 2 * M_PI * carrier_freq; // 2*pi*f

    std::string message_string = "Hello, World! This has been encoded using BPSK Modulation!";
    auto message_bitvec = string_to_binary_vec(message_string);

    itpp::bvec bitvec(message_bitvec.size());
    for (int i = 0; i < bitvec.size(); ++i)
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
    for (double i = 0; i < input_vec.size(); ++i)
    {
        input_vec[i] = i * T_SAMPLE * omega;
    }
    itpp::vec cosine_vec = itpp::cos(input_vec);

    std::cout << "Performing Tests. " << TEST_AMOUNT << " iterations for Each SNR Value." << std::endl;
    std::cout << "-----------------------------------------------------" << std::endl;
    std::cout << std::endl;

    for (int SNR = MIN_SNR; SNR <= MAX_SNR; SNR++)
    {
        itpp::vec bit_error_rate(TEST_AMOUNT);
        for (int k = 0; k < bit_error_rate.size(); ++k)
        {
            auto signal = cosine_vec;

            for (int i = 0; i < modulated.size(); ++i)
            {
                signal[i] *= modulated[i]; // A(t)*cos(2*PI*f*t)
            }

            itpp::vec noise = itpp::randn(signal.size());
            signal = signal + noise / itpp::inv_dB(SNR);
            Biquad *bp_filter = new Biquad(bq_type_bandpass,
                                            ((carrier_freq)/ F_SAMPLING), 
                                            0.7, 
                                            1.0);
            // bp_filter->setBiquad(bq_type_bandpass,10000.0 / 44100.0,1.0,1.0);
            // for (int i=0; i<signal.size();++i)
            // {
            //     signal[i] = bp_filter->process((float) signal[i]);
            // }

            for (int i = 0; i < signal.size(); ++i)
            {
                signal[i] /= cosine_vec[i];
            }

            auto demodulated = bpsk.demodulate_bits(signal);
            auto decoded = fec.decode(demodulated);
            bit_error_rate[k] = berc.count_errors(bitvec, decoded);

            std::vector<int> result(decoded.size());
            for (int i = 0; i < result.size(); ++i)
            {
                result[i] = decoded[i];
            }
        }
        std::cout << "SNR: " << SNR << "dB. BER: " << get_average(bit_error_rate) << std::endl;
    }
    return 1;
}