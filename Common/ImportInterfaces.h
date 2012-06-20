#pragma once
#include <Windows.h>
#include <vector>
#include <string>
#include <map>
#include "Math.h"
#include "Vector.h"
#include "Math.h"


using namespace std;

class IEntity
{
public:
	virtual ~IEntity() {}
	virtual idVec3 Origin() const = 0;
	virtual idVec3 Velocity() const = 0;
	virtual int Id() const = 0;

	virtual float DistanceTo( const idVec3& v ) const = 0;
	virtual float DistanceTo( const IEntity& ent ) const = 0;

	virtual bool IsValid() const = 0;
};

class IPlayer;
class IAimable : public IEntity
{
public:
	virtual ~IAimable() {}
	virtual const vector< idVec3 >& Aimpoints() const = 0;

	virtual idVec3 Acceleration() const = 0;

	virtual double GetScore( const idVec3 &targetPos ) const = 0;

	virtual bool IsEnemyOf( IPlayer * ent ) const = 0;

	virtual bool IsOnGround() const = 0;

	virtual idVec3 TraceTo( const idVec3 &pos ) const = 0;	
};


class IWeapon
{
public:
	virtual ~IWeapon() {}
	virtual int Id() const = 0;
	virtual float BulletSpeed() const = 0;
	virtual float MaxRange() const = 0;
};

class IPlayer : public IAimable
{
public:
	virtual ~IPlayer() {}
	virtual idAngles ViewAngles() const = 0;
	virtual idVec3 EyePosition() const = 0;
	virtual const IWeapon& Weapon() const = 0;
	virtual int Health() const = 0;
	virtual string Name() const = 0;

	virtual bool IsSpectating() const = 0;

	virtual IAimable * Master() const = 0;

	virtual bool CanSee( const idVec3 &pos ) const = 0;
	virtual bool CanSee( const idVec3 &pos, int skipId ) const = 0;
	virtual bool CanSee( const idVec3 &pos, int skipNum, int * hitEnt ) const = 0;
};

class IEntityList
{
public:
	virtual ~IEntityList() {}
	virtual const vector< IEntity* > Entities() const = 0;
	virtual const vector< IPlayer* > Players() const = 0;
	virtual IEntity* GetEntity(int id) const = 0;
	virtual IPlayer* GetPlayer(int id) const = 0;
};