#include "iniparser.h"
#include <cstdio>
#include <regex>

// TODO: https://docs.python.org/3/library/configparser.html#supported-ini-file-structure
// TODO: Handle comments!

IniParser::IniParser()
    : parsed(false)
{}

void IniParser::parse(std::istream& s)
{
    if (this->parsed)
        throw IniParserException("parse() can only be called once.");

    enum class ParserState
    {
        INITIAL,
        IN_SECTION
    } parser_state = ParserState::INITIAL;

    IniParser::SectionNameDataMap new_data;
    SectionData* current_section = nullptr;

    for (std::string line; std::getline(s, line); /* nothing */)
    {
        // std::cout << "Line: " << line << std::endl;

        // Try to parse section definitions.
        std::string section_name;
        if (this->parseSection(line, section_name))
        {
            // std::cout << "Found a section: " << section_name << std::endl;
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
            // std::cout << "Found an assignment: " << key << " = " << value << std::endl;
            if (parser_state == ParserState::INITIAL)
                throw IniParserException("Found assignment before section definition");

            (*current_section)[key] = value; // TODO: This looks hacky...
            continue;
        }

        // Try to parse "empty" statements.
        if (this->parseEmptyStatement(line))
        {
            // std::cout << "Found an 'empty' line." << std::endl;
            continue;
        }

        // Found an unknown statement.
        std::cerr << line << std::endl;
        throw IniParserException("Do not know how to parse this line.");
    }

    this->data = new_data;
    this->parsed = true;
}

const std::string& IniParser::getOption(const std::string& section, const std::string& key) const
{
    return this->data.at(section).at(key);
}

const IniParser::SectionNameDataMap& IniParser::getData() const
{
    return this->data;
}

bool IniParser::parseSection(const std::string& line, std::string& section_name)
{
    std::smatch matches;
    if (!std::regex_match(line, matches, std::regex("\\s*\\[(.*)\\]\\s*"), std::regex_constants::match_default))
        return false;

    section_name = matches[1];
    return true;
}

bool IniParser::parseAssignment(const std::string& line, std::string& key, std::string& value)
{
    std::smatch matches;
    if (!std::regex_match(line, matches, std::regex("\\s*(\\w+)\\s*=\\s*([\\w\\.\"]+)\\s*"), std::regex_constants::match_default))
        return false;

    key = matches[1];
    value = matches[2];
    return true;
}

bool IniParser::parseEmptyStatement(const std::string& line)
{
    // TODO: Handle comments!
    if (!std::regex_match(line, std::regex("\\s*(?:;.*)?")))
        return false;

    return true;
}
