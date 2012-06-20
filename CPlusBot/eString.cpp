#include "../Common/eString.h"

eString::eString(const string& src)
{
	source = src;
	size_t pos, lastPos = 0;
	
	while((pos = source.find('\r')) != string::npos)
		source.erase(source.begin()+pos, source.begin()+pos+1);
	
	while((pos = source.find('\n', lastPos)) < source.length())
	{
		string nextLine = source.substr(lastPos, pos - lastPos);
		lines.push_back(eLine(nextLine));
		lastPos = pos + 1;
	}
	if(lastPos < source.length())
		lines.push_back(source.substr(lastPos, source.length() - lastPos));
}

bool eString::getLine(unsigned int lineNum, eLine &output)
{
	if(lineNum < lines.size())
	{
		output = lines[lineNum];
		return true;
	}
	else
		return false;
}

const vector<eLine> eString::getLines()
{
	return lines;
}

eLine::eLine(const char * src)
{
	lineSrc = string(src);
	size_t pos, lastPos = 0;
	while((pos = lineSrc.find(' ', lastPos)) != string::npos)
	{
		string nextLine = lineSrc .substr(lastPos, pos - lastPos);
		words.push_back(nextLine);
		lastPos = pos + 1;
	}
	if(lastPos < lineSrc.length())
		words.push_back(lineSrc.substr(lastPos, lineSrc.length() - lastPos));
}

eLine::eLine( string line )
{
	lineSrc = line;
	size_t pos, lastPos = 0;
	while((pos = lineSrc.find(' ', lastPos)) != string::npos)
	{
		string nextLine = lineSrc .substr(lastPos, pos - lastPos);
		words.push_back(nextLine);
		lastPos = pos + 1;
	}
	if(lastPos < lineSrc.length())
		words.push_back(lineSrc.substr(lastPos, lineSrc.length() - lastPos));
}

const char *eLine::getWord(unsigned int wordNum)
{
	if(wordNum < words.size())
		return words[wordNum].c_str();
	else
		return nullptr;
}