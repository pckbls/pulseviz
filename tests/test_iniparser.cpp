#include "gtest/gtest.h"
#include "../src/iniparser.h"

namespace
{

TEST(IniParser, parseValidIni)
{
    std::stringstream ss;
    ss.str(R"ini(
        [one]
        foo = "bar"
        integer = 1337
        float = 42.24

        [two]
        ; section is empty
    )ini");

    IniParser parser = IniParser();
    EXPECT_NO_THROW(parser.parse(ss));

    EXPECT_EQ(parser.getData().size(), 2);

    auto items = parser.getData().at("one");
    EXPECT_EQ(items.size(), 3);
    EXPECT_EQ(items.at("foo"), "\"bar\"");
    EXPECT_EQ(items.at("integer"), "1337");
    EXPECT_EQ(items.at("float"), "42.24");

    items = parser.getData().at("two");
    EXPECT_EQ(items.size(), 0);

    // TODO: Remove?
    EXPECT_EQ(parser.getOption("one", "foo"), "\"bar\"");
}

TEST(IniParser, AssignmentWithoutSectionShouldFail)
{
    std::stringstream ss;
    ss.str(R"ini(
        foo = "bar"
        [one]
    )ini");

    IniParser parser = IniParser();
    EXPECT_THROW(parser.parse(ss), IniParserException);
}

TEST(IniParser, dataShouldBeEmptyAfterParseFailure)
{
    std::stringstream ss;
    ss.str(R"ini(
        [one]
        foo = "bar"
        reage
    )ini");

    IniParser parser = IniParser();
    EXPECT_THROW(parser.parse(ss), IniParserException);
    EXPECT_EQ(parser.getData().size(), 0);
}

TEST(IniParser, callingParseMultipleTimesShouldFail)
{
    std::stringstream ss;
    ss.str("");

    IniParser parser = IniParser();
    EXPECT_NO_THROW(parser.parse(ss));
    EXPECT_THROW(parser.parse(ss), IniParserException);
}

} // namespace
