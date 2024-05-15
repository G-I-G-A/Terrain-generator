// String.h
#ifndef STRING_H // include guard
#define STRING_H
#include <string>
#include <vector>

namespace Turtle
{
namespace String
{
	// Split a string by the separator, defaults to splitting by spaces
	std::vector<std::string> Split(const std::string& text, const char& separator = ' ');
	// Concat some strings with a separator, defaults to an empty separator
	std::string Concat(const std::vector<std::string>& texts, const std::string& separator = "");
	// Trim every trimmedChar at the left of the text
	void LTrim(std::string& text, const char& trimmedChar = ' ');
	// Trim every trimmedChar at the right of the text
	void RTrim(std::string& text, const char& trimmedChar = ' ');
	// Trim every trimmedChar at the left and the right of the text
	void Trim(std::string& text, const char& trimmedChar = ' ');
}
}

#endif /* STRING_H */