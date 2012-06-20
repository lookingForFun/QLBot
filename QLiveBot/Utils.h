#pragma once

#include <string>

using namespace std;

class Utils
{
public:
	static wstring StringToWString(const string& s)
	{
		wstring temp(s.length(),L' ');
		copy(s.begin(), s.end(), temp.begin());
		return temp; 
	}
};