#pragma once

#include <vector>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <iostream>

#if defined(XWIN_WIN32)
#include <direct.h>
#else
#include <unistd.h>
#endif

// Common Utils
namespace raw
{
    
std::vector<char> readFile(const std::string& filename);

template <typename T>
inline T clamp(const T& value, const T& low, const T& high)
{
	return value < low ? low : (value > high ? high : value);
}

}