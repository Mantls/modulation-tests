#include "qpsk.hpp"

QPSK::QPSK(double carrier_freq, double sampling_freq, int windowing_samples)
{
    this->set_parameters(carrier_freq, sampling_freq, windowing_samples);
}

QPSK::~QPSK()
{
    delete this->modulator;
    delete this->fec;
    delete this->bandpass;
}

void QPSK::set_parameters(double &carrier_freq, double &sampling_freq, int &windowing_samples)
{
    this->carrier_freq = carrier_freq;
    this->sampling_freq = sampling_freq;
    this->time_per_sample = 1 / sampling_freq;
    this->bandpass->setBiquad(bq_type_bandpass, (this->carrier_freq) / this->sampling_freq, 0.05, 1.0);
    this->windowing_time_samples = windowing_samples;
}

itpp::vec QPSK::send(itpp::bvec &message)
{
    auto signal = this->fec->encode(message);
    auto modulated = this->modulator->modulate_bits(signal);
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

itpp::bvec QPSK::receive(itpp::vec &signal)
{
    itpp::cvec sampled_signal(signal.size() / this->windowing_time_samples);
    double estimated_window_imag = 0;
    double estimated_window_real = 0;
    std::complex<double> real;
    std::complex<double> imag;
    auto count = 0;
    for (auto k = 0; k < signal.size(); ++k)
    {
        estimated_window_imag += signal[k] * -1 * std::sin(k * this->time_per_sample * 2 * M_PI * this->carrier_freq);
        estimated_window_real += signal[k] * std::cos(k * this->time_per_sample * 2 * M_PI * this->carrier_freq);

        if ((int)k % this->windowing_time_samples == 0 && (k != 0 || this->windowing_time_samples == 1) || (k == signal.size() - 1))
        {
            using namespace std::complex_literals;
            if (abs(estimated_window_imag) < abs(estimated_window_real))
            {
                imag = 0i;
                if (estimated_window_real < 0)
                {
                    real = -1;
                }
                else
                {
                    real = 1;
                }
            }
            else
            {
                real = 0;
                if (estimated_window_imag < 0)
                {
                    imag = -1i;
                }
                else
                {
                    imag = 1i;
                }
            }
            estimated_window_real = estimated_window_imag = 0;
            sampled_signal[count] = real + imag;
            ++count;
        }
    }

    auto received_bits = this->modulator->demodulate_bits(sampled_signal);
    auto output = this->fec->decode(received_bits);
    return output;
}
