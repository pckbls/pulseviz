#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <vector>

namespace util
{

std::string dirname(const std::string& path);
std::string getHomeDirectory();
bool dir_exists(const std::string& path);

template<typename T>
std::vector<T> linspace(T start, T stop, std::size_t n)
{
    std::vector<T> result(n);
    float step = (stop - start) / (n - 1);
    for (unsigned int i = 0; i < result.size(); i++)
        result[i] = static_cast<float>(i) * step;
    return result;
}

float clip(float n, float lower, float upper);

}

#endif // UTIL_H
