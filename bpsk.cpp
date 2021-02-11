#include "bpsk.hpp"

BPSK::BPSK(double carrier_freq, double sampling_freq)
{
    this->set_parameters(carrier_freq, sampling_freq);
}

BPSK::~BPSK()
{
}

void BPSK::set_parameters(double &carrier_freq, double &sampling_freq)
{
    this->carrier_freq = carrier_freq;
    this->sampling_freq = sampling_freq;
    this->time_per_sample = 1 / sampling_freq;
    this->bandpass->setBiquad(bq_type_bandpass,this->carrier_freq/ this->sampling_freq,4.5,1.0);
}

itpp::vec BPSK::send(itpp::bvec &message)
{
    auto signal = fec->encode(message);
    auto modulated = modulator->modulate_bits(signal);
    // now we have BPSK modulated signal. Now we mulitply it  onto our carrier
    for (double i = 0; i < modulated.size(); ++i)
    {
        modulated[i] *= std::cos(i * this->time_per_sample * 2 * M_PI * this->carrier_freq);
    }

    return modulated;
}

itpp::bvec BPSK::receive(itpp::vec &signal)
{
    for (int i=0;i<signal.size();++i)
    {
        signal[i]=bandpass->process(signal[i]); // "hopefully" bandpass-filter the signal to imrpove SNR...
    }
    


}