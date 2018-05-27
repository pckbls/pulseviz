#ifndef INIPARSER_H
#define INIPARSER_H

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <stdexcept>

class IniParserException : public std::runtime_error
{
// TODO: Why do I have to define this?
public:
    IniParserException(const char* message)
        : std::runtime_error(message)
    {}
};

class IniParser
{
public:
    using SectionData = std::map<std::string, std::string>;

    // TODO: Custom class that raises custom exception when using the [ ]
    // syntax for accessing members.
    using SectionNameDataMap = std::map<std::string, SectionData>;

    IniParser();
    void parse(std::istream& s);

    // TODO: Make this templated so that we can automatically decode the strings into
    // basic data types such as int/bool etc.
    const std::string& getOption(const std::string& section, const std::string& key) const;
    const SectionNameDataMap& getData() const;

protected:
    bool parseSection(const std::string& line, std::string& section_name);
    bool parseAssignment(const std::string& line, std::string& key, std::string& value);
    bool parseEmptyStatement(const std::string& line);

    SectionNameDataMap data;
    bool parsed;
};

#endif // INIPARSER_H
