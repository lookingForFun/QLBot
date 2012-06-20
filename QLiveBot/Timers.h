#ifndef TIMERSH
#define TIMERSH

#include <string>
#include <vector>

using namespace std;

typedef struct itemTimer
{
	int nextSpawn;
	string name;
	itemTimer(int ns, const char *n) { nextSpawn = ns; name = n; }
} itemTimer;

class Timers {
private:

	vector<itemTimer> timers;

public:

	Timers() {}

	void Clear();
	void Add(const string& name, int time);
	void RemoveTimer(const string& name);
	const vector<itemTimer>& GetTimers() const { return timers; }
};

extern Timers timers;

#endif