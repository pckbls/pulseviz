#include "util.h"
#include <unistd.h>
#include <sys/types.h>
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
