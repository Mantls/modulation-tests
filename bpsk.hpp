#pragma once
#include <itpp/comm/modulator.h>
#include <itpp/comm/channel.h>
#include <itpp/base/math/trig_hyp.h>
#include <cmath>
#include <itpp/comm/hammcode.h>
#include <itpp/stat/misc_stat.h>
#include "libdsp/Biquad.h"

class BPSK
{
private:
double carrier_freq;
double sampling_freq;
double time_per_sample;
double windowing_time;
int windowing_time_samples;
itpp::BPSK* modulator = new itpp::BPSK();
itpp::Hamming_Code* fec = new itpp::Hamming_Code(2);
Biquad* bandpass = new Biquad();

public:
    BPSK();
    BPSK(double carrier_freq, double sampling_freq, int windowing_time);
    void set_parameters(double &carrier_freq, double &sampling_freq, int &windowing_samples);
    itpp::vec send(itpp::bvec &signal);
    itpp::bvec receive(itpp::vec &signal);

    ~BPSK();
};


