#include "cPlayer.h"
#include "Engine.h"


tCG_Trace g_CG_Trace;
vec3_t vec3_origin = {0.0f, 0.0f, 0.0f};

#define	EF_TALK				0x00001000


float getFovX()
{
	return *reinterpret_cast<float*>(offsets.cg_refdef_vieworg - 8);
}

float getFovY()
{
	return *reinterpret_cast<float*>(offsets.cg_refdef_vieworg - 4);
}

int getTime()
{
	return *reinterpret_cast<int*>(offsets.cg_time);
}

int getSnapshot()
{
	return *reinterpret_cast<int*>(offsets.cg_snap);
}

int getSnapFlags()
{
	return *reinterpret_cast<int*>(getSnapshot());
}

DWORD __stdcall getEntity(int id)
{
	return id * CENTITY_STRUCT_SIZE + offsets.cg_entities;
}

idVec3 entityGetOrigin(int entNum)
{
	return *reinterpret_cast<idVec3*>(getEntity(entNum) + CENTITY_LERPORIGIN);
}

idAngles entityGetAngles(int entNum)
{
	return *reinterpret_cast<idAngles*>(getEntity(entNum) + CENTITY_LERPANGLES);
}

bool entityIsValid(int entNum)
{
	return *reinterpret_cast<bool*>(getEntity(entNum) + CENTITY_CURRENTVALID);
}

trajectory_t entityGetTrajectory(int entNum)
{
	return *reinterpret_cast<trajectory_t*>(getEntity(entNum) + ENTITYSTATE_POS);
}

void CG_Trace( trace_t *result, const float * start, const vec3_t mins, const vec3_t maxs, const float * end, 
						  int skipNumber, int mask )
{
	float mins1[3];
	float maxs1[3];
	VectorCopy(mins, mins1);
	VectorCopy(maxs, maxs1);
	return g_CG_Trace(result, start, &mins1, &maxs1, end, skipNumber, mask);
}

int getLegsAnim(int entId)
{
	return *reinterpret_cast<int*>(getEntity(entId) + CENTITY_CURRENTSTATE_LEGSANIM);
}

int getGameType()
{
	return *reinterpret_cast<int*>(offsets.cgs_gametype);
}

int getEFlags(int entId)
{
	return *reinterpret_cast<int*>(getEntity(entId)+8);
}

char * getClientInfo(int clientNum)
{
	return reinterpret_cast<char*>(offsets.cgs_clientinfo + clientNum * offsets.clientinfo_size);
}

int getTeam(int clientNum)
{
	return *reinterpret_cast<int*>(getClientInfo(clientNum) + CLIENTINFO_TEAM);
}

idAngles getRefDefViewAngles()
{
	return *reinterpret_cast<idAngles*>(offsets.cg_refdefviewangles);
}

idVec3 getRefDefViewOrg()
{
	return *reinterpret_cast<idVec3*>(offsets.cg_refdef_vieworg);
}

int getClientNum()
{
	return *reinterpret_cast<int*>(offsets.cg_clientnum);
}

int getGroundEntity(int clientNum)
{
	return *reinterpret_cast<int*>(getEntity(clientNum) + ENTITYSTATE_GROUNDENTITYNUM);
}

int getWeapon(int clientNum)
{
	return *reinterpret_cast<int*>(getEntity(clientNum) + ENTITYSTATE_CURRENTWEAPON);
}

bool isInfoValid(int clientNum)
{
	return *reinterpret_cast<bool*>(getClientInfo(clientNum));
}

bool __cdecl CanSee(idVec3 start, idVec3 dest, int skipNum)
{
	trace_t t;
	CG_Trace(&t, start.ToFloatPtr(), vec3_origin, vec3_origin, dest.ToFloatPtr(), skipNum, MASK_SHOT);
	return t.fraction >= 1.0f;
}

bool trap_isClientPaused()
{
	return getEFlags(Engine::GetClientNum ()) & EF_TALK;
}

float cWeapon::BulletSpeed() const
{
	static float weaponSpeeds[] = { 0.0f, 0.0f, 0.0f, 0.0f, 700.0f, 850.0f, 0.0f, 0.0, 2000.0f, 1800.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	return weaponSpeeds[_Id];
}

float cWeapon::MaxRange() const
{
	static float weaponRanges[] = { 0.0f, 100.0f, 3000.0f, 1500.0f, 1250.0f, 10000.0f, 768.0f, 16384.0, 10000.0f, 10000.0f, 5000.0f, 5000.0f, 5000.0f, 5000.0f, 5000.0f, 5000.0f };
	return weaponRanges[_Id];
}

///////////////////
//cEntity members//
///////////////////

idVec3 cEntity::Origin() const
{
	return entityGetOrigin(Id());
}

idVec3 cEntity::Velocity() const
{ 
	trajectory_t tr = entityGetTrajectory(Id());
	return idVec3(tr.trDelta[0], tr.trDelta[0], tr.trDelta[0]); 
}

bool cEntity::IsValid() const
{
	return entityIsValid(Id());
}

///////////////////
//cPlayer members//
///////////////////
#define LEGS_WALKCR 13
#define LEGS_IDLECR 23

bool cPlayer::IsCrouching() const
{
	int anim = getLegsAnim(Id()) & ~ANIM_TOGGLEBIT;
	return (anim == LEGS_WALKCR || anim == LEGS_IDLECR);
}

idVec3 cPlayer::Origin() const
{
	return entityGetOrigin(Id());
}

idVec3 cPlayer::Velocity() const
{ 
	trajectory_t tr = entityGetTrajectory(Id());
	return idVec3(tr.trDelta[0], tr.trDelta[1], tr.trDelta[2]); 
}

double cPlayer::GetScore(const idVec3& targetOrg) const
{
	double dist = DistanceTo(targetOrg) / 16384.0;
	double res = dist;
	res += Velocity().Length() / 3200.0;
	idAngles ang = ((targetOrg - EyePosition()).ToAngles() - ViewAngles()).Normalize180();
	res += ((fabs(ang.yaw) + fabs(ang.pitch)) / 270.0 /*max value*/) * (ImportExport::Variables[HUMAN]->IsEnabled() ? 6.0 : 0.0);
	return res;
}

bool cPlayer::IsOnGround() const
{
	return getGroundEntity(Id()) != 1023;
}

int cPlayer::Health() const
{
	if(getEFlags(Id()) & EF_DEAD)
		return 0;
	else
	{
		/*int health = cgs.clientinfo[Id].health;
		return health == 0 ? 100 : health;*/
		//return *(int*)((char*)getClientInfo(Id()) + 0x74);
		return 100;
	}
}

idAngles cPlayer::ViewAngles() const
{
	idAngles ang;
	if(Id() == getClientNum())
		ang = getRefDefViewAngles();
	else
		ang = entityGetAngles(Id());
	return ang;
}

idVec3 cPlayer::EyePosition() const
{
	if(Id() == getClientNum())
		return idVec3(getRefDefViewOrg().x, getRefDefViewOrg().y, getRefDefViewOrg().z);
	else
	{
		idVec3 ret = Origin();
		ret.z += IsCrouching() ? CROUCH_VIEWHEIGHT : DEFAULT_VIEWHEIGHT;
		return ret;
	}
}

const IWeapon& cPlayer::Weapon() const
{
	return _weapons[getWeapon(Id())];
}

string cPlayer::Name() const
{
	return string(getClientInfo(Id()) + 8);
}

bool cPlayer::CanSee(const idVec3& pos) const
{
	return CanSee(pos, Id());
}

bool cPlayer::CanSee(const idVec3& pos, int skipNum) const
{
	trace_t t;
	CG_Trace(&t, EyePosition().ToFloatPtr(), vec3_origin, vec3_origin, pos.ToFloatPtr(), skipNum, MASK_SHOT);
	return t.fraction >= 1.0f;
}

bool cPlayer::CanSee(const idVec3& pos, int skipNum, int * hitEnt) const
{
	trace_t t;
	CG_Trace(&t, EyePosition().ToFloatPtr(), vec3_origin, vec3_origin, pos.ToFloatPtr(), skipNum, MASK_SHOT);
	*hitEnt = t.entityNum;
	return t.fraction >= 1.0f;
}

bool cPlayer::IsValid() const
{
	return entityIsValid(Id()) && isInfoValid(Id()) && Weapon().Id() != 0 && !((*(DWORD*)(getClientInfo(Id()) + 0xC4) & ENTITY_FROZEN_FLAG)) /*last check: is the player frozen?*/;
}

idVec3 cPlayer::TraceTo(const idVec3& pos) const
{
	trace_t t;
	CG_Trace(&t, Origin().ToFloatPtr(), vec3_origin, vec3_origin, pos.ToFloatPtr(), Id(), MASK_SHOT);
	return idVec3(t.endpos[0], t.endpos[1], t.endpos[2]);
}

bool cPlayer::IsEnemyOf(IPlayer * ent) const
{ 
	if(getGameType() >= 3 /*GT_TEAM*/ &&
		getTeam(Id()) == getTeam(ent->Id()))
		return false;
	return true;
}

bool cPlayer::IsSpectating() const
{
	if(!isInfoValid(Id()))
		return false;
	return getTeam(Id()) == 3;
}

array<cWeapon, 15> cPlayer_weapons = { cWeapon(0), cWeapon(1), cWeapon(2), cWeapon(3),
	cWeapon(4), cWeapon(5), cWeapon(6), cWeapon(7), cWeapon(8), cWeapon(9), cWeapon(10),
	cWeapon(11), cWeapon(12), cWeapon(13), cWeapon(14) };

array<idVec3, 2> cPlayer_aimPointsCrouched = { idVec3(0, 0, 0), idVec3(0, 0, -10) };

array<idVec3, 12> cPlayer_aimPointsStanding = { idVec3(0, 0, 0), idVec3(8, 0, 0), idVec3(0, 8, 0),
	idVec3(8, 8, 0), idVec3(-8, 0, 0), idVec3(0, -8, 0), idVec3(-8, -8, 0),
	idVec3(-8, 8, 0), idVec3(8, -8, 0), idVec3(0, 0, -15.0f), idVec3(0, 0, 20.0f)  };

vector<idVec3> cPlayer::_aimPointsStanding = vector<idVec3>(begin(cPlayer_aimPointsStanding), end(cPlayer_aimPointsStanding));
vector<idVec3> cPlayer::_aimPointsCrouched = vector<idVec3>(begin(cPlayer_aimPointsCrouched), end(cPlayer_aimPointsCrouched));
vector<cWeapon> cPlayer::_weapons = vector<cWeapon>(begin(cPlayer_weapons), end(cPlayer_weapons));