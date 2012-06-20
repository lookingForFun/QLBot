#include "Timers.h"
#include "Engine.h"

Timers timers;

void Timers::Clear() {
	timers.clear();
}

void Timers::Add(const string& name, int time)
{
	RemoveTimer(name);
	timers.push_back(itemTimer(Engine::GetTime() + time, name.c_str()));
}

void Timers::RemoveTimer( const string& name )
{
	for(auto i = begin(timers); i != end(timers); ++i) {
		if(i->name == name) {
			timers.erase(i);
			return;
		}
	}
}
