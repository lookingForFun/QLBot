#pragma once
#include "ImportInterfaces.h"

class IAimbot
{
public:
	virtual ~IAimbot() {}
	
	virtual idVec3 TargetOrigin() = 0;

	virtual idVec3 TargetAimpoint() = 0;

	virtual idAngles Main( float FOV, float ping, bool humanize, float smoothMultiplier, float smoothTransposition ) = 0;

	virtual bool IsValidEnemy( int entityNum ) = 0;

	virtual IAimable * CurrentTarget() = 0;

	virtual idVec3 BoxTargetOrigin() = 0;
	virtual bool BoxTargetValid() = 0;
};

class Aimbot : public IAimbot
{
private:
	float FOV;
	idVec3 _TargetOrigin;
	IAimable * _CurrentTarget;
	idVec3 _TargetAimpoint;
	idVec3 _Muzzle;

	bool _BoxTargetValid;
	idVec3 _BoxTargetOrigin;
	IAimable * _BoxTarget;

protected:
	float Ping;
	
public:
	Aimbot() {}
	virtual ~Aimbot() {}
	idVec3 TargetOrigin();
	idVec3 TargetAimpoint();
	idAngles Main( float FOV, float ping, bool humanize, float smoothMultiplier, float smoothTransposition );
	bool IsValidEnemy( int entityNum );
	IAimable * CurrentTarget();
	idVec3 BoxTargetOrigin() { return _BoxTargetOrigin; };
	bool BoxTargetValid() { return _BoxTargetValid; }

private:
	bool ScanForTarget();
	idAngles AimAt(idVec3 origin);
	bool IsInFov(idVec3 v, float fov);
	bool IsInFov(idVec3 v);
	static float AdvancedHumanization( float distance, idVec3 targetVelocity );
	idAngles Humanize( float smoothMultiplier, float smoothTransposition, idAngles inAngles );

protected:
	pair<bool,idVec3> Predict( const IAimable * ent );
	static idVec3 EvaluatePath( const idVec3 &enemyAcc, const idVec3 &EnemyVelocity, float time );
	static void SolveForTime( double ProjectileSpeed, const idVec3 &EnemyAcceleration, const idVec3 &EnemyVelocity,
		const idVec3 &EnemyLocation, const idVec3 &OriginLocation, double Times[4] );
};