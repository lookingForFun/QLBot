#pragma once

#include <string>
#include <map>
#include <vector>


class AccuracyCounter
{
	int hit;
	int total;
	std::string name;

public:
	AccuracyCounter() { hit = 0; total = 0; }
	AccuracyCounter(const std::string& name) { hit = 0; total = 0; SetName(name); }
	void SetName(const std::string& name) { this->name = name; }
	const std::string& GetName() { return name; }
	void AddHit() { hit++; }
	void AddShot() { total++; }
	double Accuracy() { return total > 0 ? (hit/static_cast<double>(total))*100.0 : 0.0; }
	void Clear() { hit = 0; total = 0; }
};

class AccuracyManager
{
	static std::map<int, AccuracyCounter> accuracies;

public:
	static void Init();
	static void TrackAccuracy(int weapon, const std::string& weaponName);
	static void ShotFired(int weapon);
	static void HitRegistered(int weapon);
	static std::vector<std::pair<std::string, double>> GetAccuracies();
};