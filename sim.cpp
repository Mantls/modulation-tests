#include <itpp/comm/modulator.h>
#include <itpp/comm/channel.h>
#include <itpp/base/math/trig_hyp.h>
#include <cmath>
#include <complex>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include "utils.h"
#include <math.h>
#include <itpp/comm/reedsolomon.h>
#include <itpp/comm/hammcode.h>
#include <itpp/comm/error_counters.h>
#include <itpp/stat/misc_stat.h>
#include <itpp/itcomm.h>
#include "libdsp/Biquad.h"
#include "bpsk.hpp"
#include "qpsk.hpp"
#include "AudioFile.h"

int main()
{
    int TEST_AMOUNT = 1000;
    int MIN_SNR = 2;
    int MAX_SNR = 12;
    double F_SAMPLING = 44100;
    double T_SAMPLE = 1.F / F_SAMPLING;
    double carrier_freq = 19000;
    double omega = 2 * M_PI * carrier_freq; // 2*pi*f



    std::string message_string = "Hello, World! This has been encoded using BPSK Modulation!";
    auto message_bitvec = string_to_binary_vec(message_string);
    itpp::bvec bitvec(message_bitvec.size());
    for (int i = 0; i < bitvec.size(); ++i)
        bitvec[i] = message_bitvec[i];
    
    QPSK bpsk(carrier_freq, F_SAMPLING, 12);
    auto transmitted_signal = bpsk.send(bitvec);
    //std::cout << transmitted_signal << std::endl;

    itpp::AWGN_Channel channel;
    itpp::BERC berc; // Bit error counter
    itpp::RNG_randomize();
    itpp::Stat statistics;
    AudioFile<double> audioFile;


    audioFile.setSampleRate(F_SAMPLING);
    audioFile.setBitDepth(16);
    audioFile.setNumChannels(1);
    audioFile.setNumSamplesPerChannel(transmitted_signal.size());

    for (int i = 0; i < audioFile.getNumSamplesPerChannel(); i++)
    {
        for (int channel = 0; channel < audioFile.getNumChannels(); channel++)
        {
            audioFile.samples[channel][i] = transmitted_signal[i];
        }
    }


    audioFile.save("test-wav.wav",AudioFileFormat::Wave);


    for (double SNR=0; SNR<20;++SNR)
    {
        channel.set_noise( 1.0/pow(10,(SNR / 10.0)) );
        std::vector<double> erc_vec(500);
        for (int i=0;i<erc_vec.size();++i)
        {
            auto noisy = channel(transmitted_signal);
            auto received = bpsk.receive(noisy);
            erc_vec[i] = berc.count_errors(received, bitvec);
        }
        std::cout << "SNR in dB: " << SNR << " BER: " << std::setprecision(15) << get_average(erc_vec) << std::endl;
    }
    return 1;
}