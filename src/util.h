#ifndef UTIL_H
#define UTIL_H

#include <string>

namespace util
{

std::string dirname(const std::string& path);
std::string getHomeDirectory();
bool dir_exists(const std::string& path);

}

#endif // UTIL_H
