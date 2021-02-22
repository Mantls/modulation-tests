#include "qam16.hpp"

QAM16::QAM16(double carrier_freq, double sampling_freq, int windowing_samples)
{
    this->set_parameters(carrier_freq, sampling_freq, windowing_samples);
}

QAM16::~QAM16()
{
    delete this->modulator;
    delete this->fec;
    delete this->lowpass1;
    delete this->lowpass2;
}

void QAM16::set_parameters(double &carrier_freq, double &sampling_freq, int &windowing_samples)
{
    this->carrier_freq = carrier_freq;
    this->sampling_freq = sampling_freq;
    this->time_per_sample = 1 / sampling_freq;
    this->lowpass1->setBiquad(bq_type_lowpass, (this->carrier_freq) + 2000 / this->sampling_freq, 2, 1.0);
    this->lowpass2->setBiquad(bq_type_lowpass, (this->carrier_freq) + 2000 / this->sampling_freq, 2, 1.0);

    this->windowing_time_samples = windowing_samples;
}

itpp::vec QAM16::send(itpp::bvec &message)
{
    auto signal = this->fec->encode(message);
    auto modulated = this->modulator->modulate_bits(signal);
    this->copy = modulated;
    itpp::vec output(modulated.size() * this->windowing_time_samples);
    for (double i = 0; i < modulated.size(); ++i)
    {
        for (int k = 0; k < this->windowing_time_samples; ++k)
        {
            output[(i * this->windowing_time_samples) + k] = modulated[i].imag() * -1 * std::sin(((i * this->windowing_time_samples) + k) * this->time_per_sample * 2 * M_PI * this->carrier_freq);
            output[(i * this->windowing_time_samples) + k] += modulated[i].real() * std::cos(((i * this->windowing_time_samples) + k) * this->time_per_sample * 2 * M_PI * this->carrier_freq);
        }
    }

    return output;
}

itpp::bvec QAM16::receive(itpp::vec &signal)
{
    itpp::cvec sampled_signal(signal.size() / this->windowing_time_samples);
    double estimated_window_imag = 0;
    double estimated_window_real = 0;
    std::complex<double> real;
    std::complex<double> imag;
    auto count = 0;
    for (auto k = 0; k < signal.size(); ++k)
    {
        estimated_window_imag += this->lowpass1->process(signal[k] * -1 * std::sin(k * this->time_per_sample * 2 * M_PI * this->carrier_freq));
        estimated_window_real += this->lowpass2->process(signal[k] * std::cos(k * this->time_per_sample * 2 * M_PI * this->carrier_freq));

        if ((int)k % this->windowing_time_samples == 0 && (k != 0 || this->windowing_time_samples == 1) || (k == signal.size() - 1))
        {   
            using namespace std::complex_literals;
            estimated_window_imag /= windowing_time_samples;
            estimated_window_real /= windowing_time_samples;
            real = estimated_window_real + 0i;
            real *= 2;
            imag = estimated_window_imag*1i;
            imag *= 2;
            estimated_window_real = estimated_window_imag = 0;
            sampled_signal[count] = real + imag;
            ++count;
        }
    }
    auto received_bits = this->modulator->demodulate_bits(sampled_signal);
    auto output = this->fec->decode(received_bits);
    return output;
}
