#include "AccuracyCounter.h"
#include "xorClass.h"
#include "cPlayer.h"

#include <algorithm>

using namespace std;


map<int, AccuracyCounter> AccuracyManager::accuracies;

void AccuracyManager::Init()
{
	accuracies.clear();
	TrackAccuracy(2, string(XORSTR("MG")));
	TrackAccuracy(6, string(XORSTR("LG")));
	TrackAccuracy(7, string(XORSTR("RG")));
}

void AccuracyManager::TrackAccuracy(int weapon, const string& weaponName)
{
	accuracies[weapon] = AccuracyCounter(weaponName);
}

void AccuracyManager::ShotFired(int weapon)
{
	auto it = accuracies.find(weapon);
	if(it != accuracies.end())
		it->second.AddShot();
}

void AccuracyManager::HitRegistered(int weapon)
{
	auto it = accuracies.find(weapon);
	if(it != accuracies.end())
		it->second.AddHit();
}

vector<pair<string, double>> AccuracyManager::GetAccuracies()
{
	auto results = vector<pair<string, double>>();
	for_each(accuracies.cbegin(), accuracies.cend(), [&] (std::pair<int, AccuracyCounter> it)
	{
		results.push_back(pair<string, double>(it.second.GetName(), it.second.Accuracy()));
	});
	return results;
}