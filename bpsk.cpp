#include "bpsk.hpp"

BPSK::BPSK(double carrier_freq, double sampling_freq)
{
    this->set_parameters(carrier_freq, sampling_freq);
}

BPSK::~BPSK()
{
    delete this->modulator;
    delete this->fec;
    delete this->bandpass;
}

void BPSK::set_parameters(double &carrier_freq, double &sampling_freq)
{
    this->carrier_freq = carrier_freq;
    this->sampling_freq = sampling_freq;
    this->time_per_sample = 1 / sampling_freq;
    this->bandpass->setBiquad(bq_type_bandpass,(this->carrier_freq)/ this->sampling_freq,0.05,1.0);
}

itpp::vec BPSK::send(itpp::bvec &message)
{
    auto signal = this->fec->encode(message);
    auto modulated = this->modulator->modulate_bits(signal);
    for (double i = 0; i < modulated.size(); ++i)
    {
        modulated[i] *= std::sin(i * this->time_per_sample * 2 * M_PI * this->carrier_freq);
    }

    return modulated;
}

itpp::bvec BPSK::receive(itpp::vec &signal)
{
    for (double i = 0; i < signal.size(); ++i)
    {   
        auto temp = signal[i] * std::sin(i * this->time_per_sample * 2 * M_PI * this->carrier_freq);
        if (temp < 0)
        {
            signal[i] = -1;
        } else {
            signal[i] = 1;
        }
    }
    auto received_bits = this->modulator->demodulate_bits(signal);
    auto output = this->fec->decode(received_bits);
    return output;
}