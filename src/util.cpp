#include "util.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>

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

float util::clip(float n, float lower, float upper)
{
    return std::max(lower, std::min(n, upper));
}
