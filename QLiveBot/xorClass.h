/*
	If you can build your own preprocessor (no, I am not releasing mine and it's trivial to build one anyway)
	you can add a pre-build step and use instances of XorStr instead of plain character literals.
	This adds a layer of security in case you're dealing with an anticheat that scans memory for known text strings
	If you decide to do this, make sure XORPROCESSED is defined at build time, and make sure you restore the sources
	with a post-build step
*/
#ifndef XORCLASSH
#ifdef XORPROCESSED
#define XORSTR(x) ""

template <size_t BUFLEN, unsigned int xorKey> class XorStr
{
private:
	char s[BUFLEN];
	const char * _encrypted;
public: 
	XorStr(const char* xs)
	{
		_encrypted = xs;
	};

	__forceinline const char * decrypt()
	{
		for (unsigned i = 0; i < BUFLEN-1; i++ ) 
			s[i] = _encrypted[i] ^ xorKey;
		s[BUFLEN-1] = 0;
		return s;
	}

	~XorStr()
	{ 
		// clear string from stack
		memset(s, 0, _countof(s));
	};
};
#else
#define XORSTR(x) x
#endif // XORPROCESSED
#endif // XORCLASSH
