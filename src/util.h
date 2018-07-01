#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <vector>

namespace util
{

std::string dirname(const std::string& path);
std::string getHomeDirectory();
bool dir_exists(const std::string& path);

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

float clip(float n, float lower, float upper);

}

#endif // UTIL_H
