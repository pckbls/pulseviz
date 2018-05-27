#include "gtest/gtest.h"
#include "../src/util.h"

namespace
{

TEST(util, dirname)
{
    EXPECT_THROW(util::dirname(""), std::exception); // TODO: Proper exception type!
    EXPECT_THROW(util::dirname("   \t    "), std::exception); // TODO: Proper exception type!
    EXPECT_EQ("/", util::dirname("/foo"));
    EXPECT_EQ("/foo/", util::dirname("/foo/"));
    EXPECT_EQ("/foo/", util::dirname("/foo/bar"));
    EXPECT_EQ("/foo/bar/", util::dirname("/foo/bar/"));
}

TEST(util, getHomeDirectory)
{
    EXPECT_NO_THROW(util::getHomeDirectory());
}

} // namespace
