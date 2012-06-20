#pragma once

#include <windows.h>
#include <string>
using namespace std;

// This could be improved by setting a variable type upon initialization and caching the proper value
// In that case, one would need to either overload the assignment operator or to create a setter for Value and
// a getter for the string representation (or an overload for operator<<)
class Variable
{
public:
	Variable( string value )
	{
		Value = value;
	}

	string Value;

	int IntValue()
	{
		return atoi(Value.c_str());
	}

	bool IsEnabled()
	{
		return IntValue() != 0;
	}

	float FloatValue()
	{
		return (float)atof( Value.c_str() );
	}
};