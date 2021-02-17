#pragma once
#include <string>
#include <bitset>
#include <vector>

std::string string_to_binary(std::string &str_in)
{
    std::string str_out;
    for (int i = 0; i < str_in.size(); ++i)
    {
        str_out += std::bitset<8>(str_in[i]).to_string();
    }
    return str_out;
}

std::vector<int> string_to_binary_vec(std::string &str_in)
{
    std::vector<int> vec_out(str_in.size() * 8);
    for (int i = 0; i < str_in.size(); ++i)
    {
        auto bits = std::bitset<8>(str_in[i]);
        for (int k = bits.size() - 1; k >= 0; --k)
        {
            vec_out[(8 * i) + k] = bits[7 - k]; // this is kind of ugly
        }
    }
    return vec_out;
}


template<typename T>
std::string binary_to_string(T &vec_in) // Takes Vector of ints and returns a string
{
    std::string str_out;
    for (int i = 0; i < vec_in.size(); i += 8)
    {
        std::bitset<8> bits;
        for (int k = 0; k < bits.size(); ++k)
        {
            bits[7 - k] = (int)vec_in[i + k]; // due to endianness??
        }
        str_out += (char)bits.to_ulong();
    }
    str_out += char(0);

    return str_out;
}


template<typename T>
double get_average(T &vec_in)
{
    double res = 0;
    for (auto i = 0; i < vec_in.size(); ++i)
    {
        res += vec_in[i];
    }
    return (res / vec_in.size());
}