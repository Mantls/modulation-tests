#pragma once
#include <itpp/comm/modulator.h>
#include <itpp/comm/channel.h>
#include <itpp/base/math/trig_hyp.h>
#include <cmath>
#include <complex>
#include <itpp/comm/hammcode.h>
#include <itpp/stat/misc_stat.h>
#include "libdsp/Biquad.h"

class QPSK
{
private:
double carrier_freq;
double sampling_freq;
double time_per_sample;
double windowing_time;
int windowing_time_samples;
itpp::QPSK* modulator = new itpp::QPSK();
itpp::Hamming_Code* fec = new itpp::Hamming_Code(2);
Biquad* bandpass = new Biquad();

public:
    QPSK();
    QPSK(double carrier_freq, double sampling_freq, int windowing_time);
    void set_parameters(double &carrier_freq, double &sampling_freq, int &windowing_samples);
    itpp::vec send(itpp::bvec &signal);
    itpp::bvec receive(itpp::vec &signal);

    ~QPSK();
};


