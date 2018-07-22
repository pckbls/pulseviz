#include "util.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <exception>

std::string util::dirname(const std::string& path)
{
    return path.substr(0, path.find_last_of("/") + 1);
}

std::string util::getHomeDirectory()
{
    struct passwd* passwd = getpwuid(getuid());
    return passwd->pw_dir;
}

bool util::dir_exists(const std::string& path)
{
    // Thanks: https://stackoverflow.com/a/3828537
    struct stat sb;
    return stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode);
}

std::tuple<unsigned char, unsigned char, unsigned char> util::parseRGBHexString(const std::string& string)
{
    if (string.length() != 6)
        throw std::runtime_error("String has to be exactly 6 characters in length.");

    std::vector<unsigned char> nibbles(string.length());
    for (unsigned int i = 0; i < string.length(); i++)
    {
        char c = string[i];
        if (c >= '0' && c <= '9')
            nibbles[i] = static_cast<unsigned char>(c - '0');
        else if (c >= 'a' && c <= 'f')
            nibbles[i] = static_cast<unsigned char>(c - 'a') + 10;
        else if (c >= 'A' && c <= 'F')
            nibbles[i] = static_cast<unsigned char>(c - 'a') + 10;
        else
            throw std::runtime_error("Got unexpected character."); // TODO: Say which one!
    }

    unsigned char r = static_cast<unsigned char>(nibbles[0] << 4) + nibbles[1];
    unsigned char g = static_cast<unsigned char>(nibbles[2] << 4) + nibbles[3];
    unsigned char b = static_cast<unsigned char>(nibbles[4] << 4) + nibbles[5];

    return {r, g, b};
}
