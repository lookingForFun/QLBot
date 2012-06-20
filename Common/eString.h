#ifndef ESTRINGH
#define ESTRINGH
#include <iostream>
#include <vector>
#include <string>

using namespace std;

class eLine
{
public:
	eLine() {}
	eLine(const char * line);
	eLine(string line);

	const char *getWord(unsigned int wordNum) const;
	std::string getSource() const { return lineSrc; }
	size_t	    wordCount() const { return words.size(); }

private:
	string			lineSrc;
	vector<string>	words;
};

class eString
{
public:
	eString(const string& src);
	bool getLine(unsigned int lineNum, eLine &output);
	const vector<eLine> getLines();

private:
	string		  source;
	vector<eLine> lines;
};

#endif
