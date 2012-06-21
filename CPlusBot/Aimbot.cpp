#include "../Common/Aimbot.h"
#include "../Common/Angles.h"
#include "../Common/Matrix.h"
#include "../Common/ImportExport.h"
#include "qcomplex.h"
#include <tuple>
#include <algorithm>


struct AimpointInfo
{
	AimpointInfo() { valid = false; }
	AimpointInfo(int idx, idVec3 v, double score) { 
		this->idx = idx; 
		this->v = v; 
		valid = true; 
		this->score = score;
	}
	int idx;
	idVec3 v;
	double score;
	bool valid;
};

idVec3 Aimbot::TargetOrigin()
{
	return _TargetOrigin;
}

idVec3 Aimbot::TargetAimpoint()
{
	return _TargetAimpoint;
}

idAngles Aimbot::Main( float fov, float ping, bool humanize, float smoothMultiplier, float smoothTransposition )
{
#define FRAME_TIME_SECONDS 0.008f
		Ping = ping;
	FOV = fov;
	_Muzzle = ImportExport::User->EyePosition() + ImportExport::User->ViewAngles().ToMat3()[0]*14.0f + EvaluatePath(idVec3(0,0,0), 
		ImportExport::User->Velocity(), FRAME_TIME_SECONDS);
	if ( ScanForTarget() )
	{
		idAngles angles = AimAt( TargetOrigin() );
		return humanize ? Humanize( smoothMultiplier, smoothTransposition, angles ) : angles;
	}
	return idAngles(0, 0, 0);
}

bool Aimbot::IsValidEnemy( int entityNum )
{
	IPlayer * ent = ImportExport::Entities->GetPlayer (entityNum);
	if(ent != nullptr && ImportExport::User != nullptr && ent->IsValid())
	{
		return ent->IsEnemyOf( ImportExport::User ) && ent->Health() > 0 && ent->Weapon().Id() > 0;
	}
	return false;
}

IAimable * Aimbot::CurrentTarget()
{
	return _CurrentTarget;
}


bool Aimbot::ScanForTarget()
{
	AimpointInfo results[64];
	float fovToCheck = max(ImportExport::Variables[BOXFOV]->FloatValue(), this->FOV);
	std::find_if(begin(ImportExport::Entities->Players()), end(ImportExport::Entities->Players()), [&] (const IPlayer* p) -> bool {
		if(p->IsValid() && p->Health() > 0 && p->IsEnemyOf(ImportExport::User) && p->Id() != ImportExport::User->Id())
		{
			auto c = (p->Master() != nullptr ? p->Master() : p);
			auto prediction = Predict(c);
			if(!prediction.first) {
				return false;
			}
			idVec3 predictedOrigin = c->Origin() + prediction.second;
			if(IsInFov(predictedOrigin, fovToCheck) && ImportExport::User->Weapon().MaxRange() >= ImportExport::User->DistanceTo(predictedOrigin))
			{
				auto aimpoints = p->Aimpoints();
				for(vector<idVec3>::size_type j = 0; j < aimpoints.size(); ++j)
				{
					idVec3 testOrigin = predictedOrigin + aimpoints[j];
					if(ImportExport::User->CanSee(testOrigin, p->Id()))
					{
						results[p->Id()] = AimpointInfo(j, testOrigin, ImportExport::User->GetScore(testOrigin) + j / 2.0);
						return true;
					}
				}
			}
		}
		return false;
	});
	
	int bestTarget = -1;
	double bestScore;
	int boxBestTarget = -1;
	double boxBestScore;
	for (int i = 0; i < 64; ++i)
	{
		if(results[i].valid)
		{
			double curScore = results[i].score;
			if(boxBestTarget == -1 || curScore < boxBestScore)
			{
				boxBestTarget = i;
				boxBestScore = curScore;
			}
			if(IsInFov(results[i].v) && (bestTarget == -1 || curScore < bestScore))
			{
				bestTarget = i;
				bestScore = curScore;
			}
		}
	}

	if(boxBestTarget != -1)
	{
		_BoxTargetValid = true;
		_BoxTargetOrigin = results[boxBestTarget].v;
		_BoxTarget = ImportExport::Entities->GetPlayer(boxBestTarget);
	}
	else
		_BoxTargetValid = false;

	if(bestTarget != -1)
	{
		AimpointInfo& targetInfo = results[bestTarget];
		_CurrentTarget = ImportExport::Entities->GetPlayer(bestTarget);
		_TargetOrigin = targetInfo.v;
		//FIXME: doesn't consider if the target is a player or its master IAimable
		_TargetAimpoint = _CurrentTarget->Aimpoints()[targetInfo.idx];
		return true;
	}

	_CurrentTarget = nullptr;
	return false;
}

idAngles Aimbot::AimAt( idVec3 origin )
{
	idAngles idealAngles = ( origin - _Muzzle ).ToAngles();
	return ( idealAngles - ImportExport::User->ViewAngles() ).Normalize180();
}

bool Aimbot::IsInFov( idVec3 v, float fov )
{
	idAngles ang = AimAt( v );
	if ( idMath::Cos( DEG2RAD( ang.yaw ) ) < idMath::Cos( DEG2RAD( fov / 2.0f ) ) )
		return false;
	if ( idMath::Cos( DEG2RAD( ang.pitch ) ) < idMath::Cos( DEG2RAD( fov / 2.0f ) ) )
		return false;
	return true;
}

bool Aimbot::IsInFov( idVec3 v )
{
	return IsInFov(v, FOV);
}

float Aimbot::AdvancedHumanization( float distance, idVec3 targetVelocity )
{
	//targetVelocity.z = 0;
	float targetSpeed = targetVelocity.Length(); //speed in units/seconds
	float returnValue = max( distance / 1000.0f, 1.192092896e-07f );
	return returnValue + targetSpeed / 320.0f;
}

#pragma warning(disable: 4244)
idAngles Aimbot::Humanize( float smoothMultiplier, float smoothTransposition, idAngles inAngles )
{
	smoothMultiplier = 1.0f + AdvancedHumanization( ImportExport::User->DistanceTo( TargetOrigin() ), 
		CurrentTarget()->Velocity() ) * ImportExport::Variables[SMOOTHADVANCEDSCALING]->FloatValue();

	if(ImportExport::Variables[NEWHUMANIZATION]->IntValue())
	{
#define SUPER_SMOOTH_FUNC( x ) (float)(log(1.0 + x) / log(2.0))
		float totalDist = sqrt(inAngles.pitch*inAngles.pitch + inAngles.yaw*inAngles.yaw);
		float slowDown = SUPER_SMOOTH_FUNC( totalDist ) * smoothMultiplier * ImportExport::Variables[SMOOTHMULTIPLIER]->FloatValue()  + ImportExport::Variables[SMOOTHTRANSPOSITION]->FloatValue();
		inAngles.pitch /= max( slowDown, 1.0f );
		inAngles.yaw /= max( slowDown, 1.0f ) ;
	}
	else
	{
#define OLD_SMOOTH_FUNC( x )( (float) ( 9.4984838 - 3.9040848 * idMath::Cos( 0.20078448 * x ) - 2.8554108 * idMath::Sin( 0.20078448 * x ) - 2.9001113 * idMath::Cos( 0.40156896 * x ) - 2.4931047 * idMath::Sin( 0.40156896 * x ) - 0.9005837 * idMath::Cos( 0.60235344 * x ) + 0.35869083 * idMath::Sin( 0.60235344 * x ) - 0.29057134 * idMath::Cos( 0.80313792 * x ) + 0.4051369 * idMath::Sin( 0.80313792 * x ) + 0.15789337 * idMath::Cos( 1.0039224 * x ) + 0.086771276 * idMath::Sin( 1.0039224 * x ) + 0.049832578 * idMath::Cos( 1.2047069 * x ) - 0.14713865 * idMath::Sin( 1.2047069 * x ) - 0.030558821 * idMath::Cos( 1.4054914 * x ) - 0.029112203 * idMath::Sin( 1.4054914 * x ) ) )
		float absPitch = abs( inAngles.pitch );
		float absYaw = abs( inAngles.yaw );
		absYaw /= 6.0f;
		absPitch /= 3.0f;
		float ySlowDown = OLD_SMOOTH_FUNC( absPitch );
		float xSlowDown = OLD_SMOOTH_FUNC( absYaw );
		ySlowDown += smoothTransposition;
		xSlowDown += smoothTransposition;
		xSlowDown *= smoothMultiplier * ( smoothMultiplier / 2.0f );
		ySlowDown *= smoothMultiplier * smoothMultiplier;
		inAngles.pitch /= max( ySlowDown, 1.0f );
		inAngles.yaw /= max( xSlowDown, 1.0f );
	}
	return inAngles;
}
#pragma warning(default:4244)


pair<bool, idVec3> Aimbot::Predict( const IAimable * ent )
{
	double t = 0.0;
	idVec3 entAcc = !ent->IsOnGround() ? ent->Acceleration() : idVec3(0, 0, 0);
	float bulletSpeed = ImportExport::User->Weapon().BulletSpeed();
	if ( bulletSpeed > 0.0f )
	{
		idVec3 entVel = ent->Velocity();
		if(ent->IsOnGround())
		{
			entVel.x *= 0.825f;
			entVel.y *= 0.825f;
		}
		double times[4];
		SolveForTime( bulletSpeed, entAcc, entVel, ent->Origin(), _Muzzle, times );

		bool hasTime = false;
		double bestTime;
		for(unsigned i = 0; i < 4; ++i)
			if(times[i] > 0.0 && (!hasTime || bestTime > times[i]))
			{
				hasTime = true;
				bestTime = times[i];
			}

		if ( hasTime )
			t = bestTime;
		else
			return pair<bool, idVec3>(false, idVec3(0, 0, 0));
	}
	else
		entAcc /= 10.0f;
	return pair<bool, idVec3>(true,
		ent->TraceTo( ent->Origin() + EvaluatePath( entAcc, ent->Velocity(), static_cast<float> ( t + (Ping - (ImportExport::User->Weapon().BulletSpeed() > 0.0f ? 50.0 : 0.0f)) / 1000.0 ) ) ) - ent->Origin());
}

idVec3 Aimbot::EvaluatePath(const idVec3 &playerAcc, const idVec3 &playerVel, float time)
{
	idVec3 enemyLocationAtTime = 0.5f * playerAcc * ( time * time );
	return enemyLocationAtTime + playerVel*time;
}

void Aimbot::SolveForTime( double ProjectileSpeed, const idVec3 &acc, const idVec3 &vel,
					  const idVec3 &EnemyLocation, const idVec3 &OriginLocation, double Times[4] )
{
	double a, b, c, d, e, alpha, beta, gamma;
	qcomplex mid1, mid2, mid3, mid4, mid5, mid6, mid7, term1, term2, term3a, term3b, temp;
	double speed;
#define pow2t3 1.2599210498948731647672106072782

	idVec3 dist = EnemyLocation - OriginLocation;
	speed = ProjectileSpeed;

	a = (acc * acc) / 4.0f;
	b = acc * vel;
	c = acc * dist + vel * vel - speed * speed;
	d = 2.0 * (vel * dist);
	e = dist * dist;

	if( a == 0.0f )
	{
		alpha = sqrt(d*d - 4*c*e);
		beta  = -d;
		gamma = 2*c;
		Times[0] = Times[1] = (beta + alpha) / gamma;
		Times[2] = Times[3] = (beta - alpha) / gamma;
	}
	else
	{
		mid1 = c*c - 3*b*d + 12*e;

		mid2 = -2*c*c*c + 9*b*c*d - 27*d*d - 27*b*b*e + 72*c*e;

		mid3 = -4;
		mid3 = (mid3*mid1*mid1*mid1 + mid2*mid2).Pow(0.5);

		mid4 = (-mid2 - mid3).Pow(1.0/3.0);

		temp = 3.0*pow2t3;
		mid5 = pow2t3/3.0;
		mid5 = mid5 * mid1/mid4 + mid4/temp;

		mid6 = b*b/2 - (4*c)/3;
		mid6 = mid6 - mid5;

		temp = 4;
		mid7 = b*b/4 - (2*c)/3;
		mid7 = (mid7 + mid5).Pow(0.5) * temp;
		temp = -b*b*b + 4*b*c - 8*d;
		mid7 = temp / mid7;

		term1 = -b/4;

		term2 = b*b/4 - (2*c)/3;
		temp = 2.0;
		term2 = (term2 + mid5).Pow(0.5) / temp;

		term3a = (mid6 - mid7).Pow(0.5) / temp;
		term3b = (mid6 + mid7).Pow(0.5) / temp;

		Times[0] = (term1 - term2 - term3a).RIPart();
		Times[1] = (term1 - term2 + term3a).RIPart();
		Times[2] = (term1 + term2 - term3b).RIPart();
		Times[3] = (term1 + term2 + term3b).RIPart();
	}
}
