#pragma once
#include <math.h>
#include <vector>
#include <string>

using namespace std;

#include "../Common/ImportInterfaces.h"
#include "../Common/ImportExport.h"
#include "../Common/Angles.h"

#include "OffsetManager.h"

#define CENTITY_STRUCT_SIZE 0x2A0 ///
#define CENTITY_LERPORIGIN 0x288 ///
#define CENTITY_LERPANGLES 0x294 ///
#define CENTITY_CURRENTVALID 0x1B4 //CG_TransitionSnapshot
#define CENTITY_CURRENTSTATE_LEGSANIM 0xCC ///
//#define CLIENTINFO_SIZE 0x668
#define CLIENTINFO_TEAM 0x108//blue
#define ENTITYSTATE_GROUNDENTITYNUM 0x9C
#define ENTITYSTATE_CURRENTWEAPON 0xC8 //EV_RAILTRAIL
#define ENTITYSTATE_POS 0xC
#define SNAP_PS_VIEWHEIGHT 0xCC
#define ENTITY_FROZEN_FLAG 0x8000

#include "SDKIncludes.h"

int getGameType();

typedef void (__cdecl* tCG_Trace)( trace_t *result, const float * start, void * mins, void * maxs, const float * end, 
			  int skipNumber, int mask );

extern tCG_Trace g_CG_Trace;

extern vec3_t vec3_origin;

class cWeapon : public IWeapon
{
	int _Id;

public:
	cWeapon(int id) { _Id = id; }

	virtual int Id() const
	{
		return _Id;
	}

	virtual float BulletSpeed() const;

	virtual float MaxRange() const;
	
};

class cEntity : public IEntity
{
private:
	int _Id;

public:
	cEntity(int id) { _Id = id; }
	virtual ~cEntity() {}

	virtual idVec3 Origin() const;

	virtual int Id() const
	{
		return _Id;
	}

	virtual idVec3 Velocity() const;
	
	virtual float DistanceTo(const idVec3 &pos) const { return (pos - Origin()).Length(); }
	virtual float DistanceTo(const IEntity& ent) const { return DistanceTo(ent.Origin()); }

	virtual bool IsValid() const;
};

#include <array>

class cPlayer : public IPlayer, public cEntity
{
	static vector<idVec3> _aimPointsStanding;
	static vector<idVec3> _aimPointsCrouched;
	static vector<cWeapon> _weapons;
	bool IsCrouching() const;
	
	int _Id;

public:
	cPlayer(int id) : cEntity(id)
	{
		_Id = id;
	}
	
	virtual idVec3 Origin() const;

	virtual int Id() const
	{
		return _Id;
	}

	virtual idVec3 Velocity() const;

	virtual float DistanceTo(const idVec3 &pos) const { return (pos - Origin()).Length(); }

	virtual float DistanceTo(const IEntity& ent) const { return DistanceTo(ent.Origin()); }

	// IAimable members
	virtual const vector<idVec3>& Aimpoints() const
	{
		return IsCrouching() ? _aimPointsCrouched : _aimPointsStanding;
	}

	virtual idVec3 Acceleration() const
	{
		return idVec3(0, 0, -800.0f);
	}

	virtual double GetScore(const idVec3 &targetOrg) const;

	virtual bool IsEnemyOf(IPlayer * ent) const;

	virtual bool IsOnGround() const;

	//IPlayer
	virtual int Health() const;

	virtual idAngles ViewAngles() const;

	virtual IAimable * Master() const
	{
		return nullptr;
	}

	virtual idVec3 EyePosition() const;

	virtual const IWeapon& Weapon() const;

	virtual string Name() const;

	virtual bool IsSpectating() const;

	virtual bool CanSee( const idVec3 &pos ) const;

	virtual bool CanSee( const idVec3 &pos, int skipNum ) const;

	virtual bool CanSee( const idVec3 &pos, int skipNum, int * hitEnt ) const;

	virtual bool IsValid() const;

	virtual idVec3 TraceTo(const idVec3 &pos) const;
};

class cEntList : public IEntityList
{
	vector<IEntity*> ents;
	vector<IPlayer*> players;

public:
	cEntList()
	{
		ents = vector<IEntity*>();
		for(int i = 0; i < MAX_GENTITIES; ++i)
			ents.push_back(new cEntity(i));
		players = vector<IPlayer*>();
		for(int i = 0; i < MAX_CLIENTS; ++i)
			players.push_back(new cPlayer(i));
	}

	virtual const vector<IEntity*> Entities() const
	{
		return ents;
	}

	virtual const vector<IPlayer*> Players() const
	{
		return players;
	}

	virtual IEntity* GetEntity(int id) const
	{
		return ents[id];
	}

	virtual IPlayer* GetPlayer(int id) const
	{
		return players[id];
	}
};