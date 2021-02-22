#pragma once
#include <itpp/comm/modulator.h>
#include <itpp/comm/channel.h>
#include <itpp/base/math/trig_hyp.h>
#include <cmath>
#include <complex>
#include <itpp/comm/hammcode.h>
#include <itpp/stat/misc_stat.h>
#include "libdsp/Biquad.h"

class QAM16
{
private:
double carrier_freq;
double sampling_freq;
double time_per_sample;
double windowing_time;
int windowing_time_samples;
itpp::QAM* modulator = new itpp::QAM(16);
itpp::Hamming_Code* fec = new itpp::Hamming_Code(2);
Biquad* lowpass1 = new Biquad();
Biquad* lowpass2 = new Biquad();


itpp::cvec copy;



public:
    QAM16();
    QAM16(double carrier_freq, double sampling_freq, int windowing_time);
    void set_parameters(double &carrier_freq, double &sampling_freq, int &windowing_samples);
    itpp::vec send(itpp::bvec &signal);
    itpp::bvec receive(itpp::vec &signal);

    ~QAM16();
};


