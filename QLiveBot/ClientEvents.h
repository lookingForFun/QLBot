#pragma once

#include "Engine.h"
#include <map>
#include <string>
#include <functional>
#include <algorithm>

using namespace std::tr1;
using namespace std;

template <class T> class Event
{
private:
	map<string, function<void (T)>> callbacks;
public:
	Event() {}

	bool RegisterCallback(string name, function<void (T)> callback) {
		if(callbacks.find(name) == callbacks.end())
		{
			callbacks[name] = callback;
			return true;
		}
		return false;
	}

	bool UnregisterCallback(string name) {
		auto cb;
		if(cb != callbacks.end())
		{
			callbacks.erase(cb);
			return true;
		}
		return false;
	}

	void Trigger(T eventArg) {
		for_each(callbacks.begin(), callbacks.end(), [eventArg] (pair<string, function<void (T)>> entry) { entry.second(eventArg); });
	}
};

class State
{
private:
	bool liveClients[MAX_CLIENTS];
	int killCounts[MAX_CLIENTS];
	int killerCounts[MAX_CLIENTS];
	
	Event<int> onKill;
	Event<int> onDeath;

public:
	State() { 
		memset(liveClients, 0, _countof(liveClients));
		memset(killerCounts, 0, _countof(killerCounts)*sizeof(int));
		memset(killCounts, 0, _countof(killCounts)*sizeof(int));
	}

	void RegisterKill(int targetId) { 
		killCounts[targetId]++;
		onKill.Trigger(targetId);
	}

	void RegisterDeath(int killerId) {
		killerCounts[killerId]++;
		onDeath.Trigger(killerId);
	}
	
	Event<int> OnKill() { return onKill; }
	Event<int> OnDeath() { return onDeath; }
																		

	void ClientDisconnected(int clientId) { killCounts[clientId] = killerCounts[clientId] = 0; liveClients[clientId] = false; }

	
};