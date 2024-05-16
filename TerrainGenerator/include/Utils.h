#ifndef UTILS_H
#define UTILS_H

#include <fstream>
#include <iostream>
#include <sstream>

namespace Utils
{
	std::string StringFromFile(const std::string& filePath)
	{
		std::ifstream file;
		file.open(filePath, std::ios::in);
		if (!file.is_open())
			throw std::runtime_error("Impossible to read file.");

		std::stringstream buffer;
        buffer << file.rdbuf();

		file.close();
        return buffer.str();
	}
}

#endif UTILS_H