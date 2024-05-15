#include <sstream>

#include "Utils/String.h"

std::vector<std::string> Turtle::String::Split(const std::string& text, const char& separator)
{
    std::stringstream ss(text);
    std::string item;
    std::vector<std::string> elems;
    while (std::getline(ss, item, separator))
    {
        elems.emplace_back(std::move(item));
    }

    return elems;
}
std::string Turtle::String::Concat(const std::vector<std::string>& texts, const std::string& separator)
{
    std::stringstream ss;
    for (int i = 0; i < texts.size(); ++i)
    {
        ss << texts[i];
    	if (i != texts.size() - 1)
            ss << separator;
    }

    return ss.str();
}
void Turtle::String::LTrim(std::string& text, const char& trimmedChar)
{
    text.erase(
        text.begin(),
        std::find_if(
            text.begin(),
            text.end(), 
            [trimmedChar](const char& ch) { return ch == trimmedChar; }
		)
    );
}
void Turtle::String::RTrim(std::string& text, const char& trimmedChar)
{
    text.erase(
        std::find_if(
            text.rbegin(),
            text.rend(),
            [trimmedChar](const char& ch) { return ch == trimmedChar; }
		).base(),
        text.end()
    );
}
void Turtle::String::Trim(std::string& text, const char& trimmedChar)
{
    LTrim(text, trimmedChar);
    RTrim(text, trimmedChar);
}

