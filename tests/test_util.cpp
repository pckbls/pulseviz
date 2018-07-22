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
    ASSERT_EQ(1, 0); // TODO: Implement!
}

TEST(UtilTest, linspace)
{
    auto foo = util::linspace(0.0f, 10.0f, 50);
    ASSERT_EQ(foo.size(), 50);
    ASSERT_FLOAT_EQ(foo.front(), 0.0f);
    ASSERT_FLOAT_EQ(foo.back(), 10.0f);
}

TEST(UtilTest, clip)
{
    ASSERT_FLOAT_EQ(util::clip(0.9f, 1.0f, 2.0f), 1.0f);
    ASSERT_FLOAT_EQ(util::clip(1.0f, 1.0f, 2.0f), 1.0f);
    ASSERT_FLOAT_EQ(util::clip(1.5f, 1.0f, 2.0f), 1.5f);
    ASSERT_FLOAT_EQ(util::clip(2.0f, 1.0f, 2.0f), 2.0f);
    ASSERT_FLOAT_EQ(util::clip(2.1f, 1.0f, 2.0f), 2.0f);
}

TEST(UtilTest, parseRGBHexString)
{
    ASSERT_THROW(util::parseRGBHexString(""), std::runtime_error);
    ASSERT_THROW(util::parseRGBHexString("abc"), std::runtime_error);
    ASSERT_THROW(util::parseRGBHexString("ff,,ff"), std::runtime_error);
    ASSERT_THROW(util::parseRGBHexString("fffffff"), std::runtime_error);
    ASSERT_THROW(util::parseRGBHexString("fffff"), std::runtime_error);

    using Foo = std::tuple<unsigned char, unsigned char, unsigned char>;
    ASSERT_EQ(util::parseRGBHexString("000000"), Foo(0, 0, 0));
    ASSERT_EQ(util::parseRGBHexString("ff0000"), Foo(255, 0, 0));
    ASSERT_EQ(util::parseRGBHexString("00ff00"), Foo(0, 255, 0));
    ASSERT_EQ(util::parseRGBHexString("0000ff"), Foo(0, 0, 255));
    ASSERT_EQ(util::parseRGBHexString("ffffff"), Foo(255, 255, 255));
    ASSERT_EQ(util::parseRGBHexString("029adf"), Foo(2, 154, 223));
}

} // namespace
