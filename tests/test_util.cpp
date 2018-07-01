#include "gtest/gtest.h"
#include "../src/util.h"

namespace
{

TEST(UtilTest, dirname)
{
    // TODO: Compare results against other implementations
    EXPECT_THROW(util::dirname(""), std::exception); // TODO: Proper exception type!
    EXPECT_THROW(util::dirname("   \t    "), std::exception); // TODO: Proper exception type!
    EXPECT_EQ("/", util::dirname("/foo"));
    EXPECT_EQ("/foo/", util::dirname("/foo/"));
    EXPECT_EQ("/foo/", util::dirname("/foo/bar"));
    EXPECT_EQ("/foo/bar/", util::dirname("/foo/bar/"));
}

TEST(UtilTest, getHomeDirectory)
{
    EXPECT_NO_THROW(util::getHomeDirectory());
}

TEST(UtilTest, trim)
{
    ASSERT_EQ(1, 0);
}

TEST(UtilTest, linspace)
{
    auto foo = util::linspace(0.0f, 10.0f, 50);
    ASSERT_EQ(foo.size(), 50);
    ASSERT_FLOAT_EQ(foo.front(), 0.0f);
    ASSERT_FLOAT_EQ(foo.back(), 10.0f);
}

} // namespace
