#ifndef INIPARSER_H
#define INIPARSER_H

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <stdexcept>

class IniParserException : public std::runtime_error
{
public:
    IniParserException(std::string message, unsigned int line = 0, std::string line_content = "");
    unsigned int getLine() const;
    const std::string& getLineContent() const;

protected:
    std::string message;
    unsigned int line;
    std::string line_content;
};

class IniParserDataException : public std::runtime_error
{
public:
    IniParserDataException(std::string what);
};

class IniParserTypeConversionException : public std::runtime_error
{
public:
    IniParserTypeConversionException(std::string section, std::string key, std::string type);
    const char* what() const noexcept;

protected:
    std::string reason;
};

class IniParser
{
public:
    using SectionData = std::map<std::string, std::string>;
    using SectionNameDataMap = std::map<std::string, SectionData>;

    IniParser();
    void parse(std::istream& s);

    const std::string& getOption(const std::string& section, const std::string& key) const; // TODO: Rename to getOptionAsString
    bool getOptionAsBool(const std::string& section, const std::string& key) const;
    unsigned int getOptionAsUnsignedInteger(const std::string& section, const std::string& key) const;
    float getOptionAsFloat(const std::string& section, const std::string& key) const;

    const SectionNameDataMap& getData() const;

protected:
    bool parseSection(const std::string& line, std::string& section_name);
    bool parseAssignment(const std::string& line, std::string& key, std::string& value);
    bool parseEmptyStatement(const std::string& line);

    SectionNameDataMap data;
    bool parsed;
};

#endif // INIPARSER_H
