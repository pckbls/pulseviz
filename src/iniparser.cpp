#include "iniparser.h"
#include <cstdio>
#include <regex>

IniParserException::IniParserException(std::string message, unsigned int line, std::string line_content)
    :
    std::runtime_error(message.c_str()),
    message(message),
    line(line),
    line_content(line_content)
{}

unsigned int IniParserException::getLine() const
{
    return this->line;
}

const std::string &IniParserException::getLineContent() const
{
    return this->line_content;
}

IniParserTypeConversionException::IniParserTypeConversionException(std::string section, std::string key, std::string type)
    :
    std::runtime_error("YOU SHOULD NEVER SEE THIS AT ALL!")
{
    reason = "Attempt to decode [" + section + "]." + key + " as " + type + " failed.";
}

const char* IniParserTypeConversionException::what() const noexcept
{
    return this->reason.c_str();
}

IniParserDataException::IniParserDataException(std::string what)
    :
    std::runtime_error(what.c_str())
{}

IniParser::IniParser()
    : parsed(false)
{}

void IniParser::parse(std::istream& s)
{
    this->parsed = false;
    this->data = {};

    enum class ParserState
    {
        INITIAL,
        IN_SECTION
    } parser_state = ParserState::INITIAL;

    IniParser::SectionNameDataMap new_data;
    SectionData* current_section = nullptr;

    unsigned int line_number;
    std::string line;
    for (line_number = 1; std::getline(s, line); line_number++)
    {
        // Try to parse section definitions.
        std::string section_name;
        if (this->parseSection(line, section_name))
        {
            if (parser_state == ParserState::INITIAL)
                parser_state = ParserState::IN_SECTION;

            // According to the docs, using the [ ] syntax to access a std::map
            // implicitly creates the member if it does not exist yet.
            current_section = &new_data[section_name];
            continue;
        }

        // Try to parse assignment statements.
        std::string key, value;
        if (this->parseAssignment(line, key, value))
        {
            if (parser_state == ParserState::INITIAL)
                throw IniParserException("Found assignment before section definition", line_number, line);

            (*current_section)[key] = value; // TODO: This looks hacky...
            continue;
        }

        // Try to parse "empty" statements.
        if (this->parseEmptyStatement(line))
        {
            continue;
        }

        // Found an unknown statement.
        throw IniParserException("Do not know how to parse this line.", line_number, line);
    }

    this->data = new_data;
    this->parsed = true;
}

const std::string& IniParser::getOption(const std::string& section, const std::string& key) const
{
    const auto& data = this->getData();

    try
    {
        const auto& section_data = data.at(section);
        try
        {
            const auto& result = section_data.at(key);
            return result;
        }
        catch (std::out_of_range&)
        {
            throw IniParserDataException("Cannot find key '" + key + "' in section '" + section + "'.");
        }
    }
    catch (std::out_of_range&)
    {
        throw IniParserDataException("Cannot find section '" + section + "'.");
    }
}

bool IniParser::getOptionAsBool(const std::string &section, const std::string &key) const
{
    auto string = this->getOption(section, key);

    if (string == "true")
        return true;
    else if (string == "false")
        return false;

    int number;
    std::string::size_type len;
    try
    {
        number = std::stoi(string, &len);
    }
    catch (std::invalid_argument&)
    {
        throw IniParserTypeConversionException(section, key, "boolean");
    }

    if (len != string.length())
        throw IniParserTypeConversionException(section, key, "boolean");

    if (number == 1)
        return true;
    else if (number == 0)
        return false;
    else
        throw IniParserTypeConversionException(section, key, "boolean");
}

unsigned int IniParser::getOptionAsUnsignedInteger(const std::string &section, const std::string &key) const
{
    auto string = this->getOption(section, key);

    std::string::size_type len;
    int value;
    try
    {
        value = std::stoi(string, &len);
    }
    catch (std::invalid_argument&)
    {
        throw IniParserTypeConversionException(section, key, "unsigned integer");
    }

    if (value < 0 || len != string.length())
        throw IniParserTypeConversionException(section, key, "unsigned integer");
    else
        return static_cast<unsigned int>(value);
}

float IniParser::getOptionAsFloat(const std::string &section, const std::string &key) const
{
    auto string = this->getOption(section, key);

    std::string::size_type len;
    float value;
    try
    {
        value = std::stof(string, &len);
    }
    catch (std::invalid_argument&)
    {
        throw IniParserTypeConversionException(section, key, "float");
    }

    if (len != string.length())
        throw IniParserTypeConversionException(section, key, "float");
    else
        return value;
}

const IniParser::SectionNameDataMap& IniParser::getData() const
{
    if (!this->parsed)
        throw IniParserDataException("parse() has to be called first.");

    return this->data;
}

bool IniParser::parseSection(const std::string& line, std::string& section_name)
{
    std::regex regex{
        "\\s*"            // leading white space
        "\\[([^\\s]*)\\]" // section name including surrounding brackets
        "\\s*"            // trailing white space
        "(?:;.*)?"        // optional comment
    };

    std::smatch matches;
    if (!std::regex_match(line, matches, regex))
        return false;

    section_name = matches[1];
    return true;
}

bool IniParser::parseAssignment(const std::string& line, std::string& key, std::string& value)
{
    std::regex regex{
        "\\s*"       // leading white space
        "([^=\\s]+)" // key name
        "\\s*=\\s*"  // assignment operator incl. surrounding white space
        "([^;\\s]*)" // value
        "\\s*"       // trailing white space
        "(?:;.*)?"   // optional comment
    };

    std::smatch matches;
    if (!std::regex_match(line, matches, regex))
        return false;

    key = matches[1];
    value = matches[2];
    return true;
}

bool IniParser::parseEmptyStatement(const std::string& line)
{
    std::regex regex{
        "\\s*"     // white space
        "(?:;.*)?" // optional comment
    };

    if (!std::regex_match(line, regex))
        return false;

    return true;
}
