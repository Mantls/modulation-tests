#include "bpsk.hpp"

BPSK::BPSK(double carrier_freq, double sampling_freq, int windowing_samples)
{
    this->set_parameters(carrier_freq, sampling_freq, windowing_samples);
}

BPSK::~BPSK()
{
    delete this->modulator;
    delete this->fec;
    delete this->bandpass;
}

void BPSK::set_parameters(double &carrier_freq, double &sampling_freq, int &windowing_samples)
{
    this->carrier_freq = carrier_freq;
    this->sampling_freq = sampling_freq;
    this->time_per_sample = 1 / sampling_freq;
    this->bandpass->setBiquad(bq_type_bandpass,(this->carrier_freq)/ this->sampling_freq,0.05,1.0);
    this->windowing_time_samples =  windowing_samples;
}

itpp::vec BPSK::send(itpp::bvec &message)
{
    auto signal = this->fec->encode(message);
    auto modulated = this->modulator->modulate_bits(signal);
    itpp::vec output(modulated.size()*this->windowing_time_samples);
    for (double i = 0; i < modulated.size(); ++i)
    {   
        for (int k=0; k<this->windowing_time_samples;++k)
            output[(i * this->windowing_time_samples)+k] = modulated[i] * std::sin(((i * this->windowing_time_samples)+k) * this->time_per_sample * 2 * M_PI * this->carrier_freq);
    }

    return output;
}

itpp::bvec BPSK::receive(itpp::vec &signal)
{   
    itpp::vec sampled_signal(signal.size() / this->windowing_time_samples);
    double moving_avg = 0;
    auto count = 0;
    for (auto i=0; i<signal.size();++i)
    {
        moving_avg += signal[i] * std::sin(i * this->time_per_sample * 2 * M_PI * this->carrier_freq);
        if ((int) i % this->windowing_time_samples == 0 && i!=0)
        {
            if (moving_avg < 0)
            {   
                sampled_signal[count] = -1;
            } else {
                sampled_signal[count] = 1;
            }
            ++count;
            moving_avg = 0;
        }
    } 
    
    auto received_bits = this->modulator->demodulate_bits(sampled_signal);
    auto output = this->fec->decode(received_bits);
    return output;
}


