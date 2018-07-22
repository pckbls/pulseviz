#pragma once

#include <string>
#include <vector>
#include <tuple>

namespace util
{

std::string dirname(const std::string& path);
std::string getHomeDirectory();
bool dir_exists(const std::string& path);
std::tuple<unsigned char, unsigned char, unsigned char> parseRGBHexString(const std::string& string);

// See: https://stackoverflow.com/a/11747023
template<typename T>
std::vector<T> linspace(T start, T stop, unsigned int n)
{
    std::vector<T> result;
    float step = (stop - start) / (n - 1);

    while (start <= stop)
    {
        result.push_back(start);
        start += step;
    }

    return result;
}

template<typename T>
T clip(T n, T lower, T upper)
{
    return std::max(lower, std::min(n, upper));
}

}
