#include "../Common/AutoFire.h"
#include "../Common/ImportExport.h"
#include "../Common/Angles.h"
#include "../Common/Matrix.h"

bool AutoFire::AimingAtTarget()
{
	IPlayer * user = ImportExport::User;
	if(ImportExport::AimBot->CurrentTarget() != nullptr)
	{
		idVec3 cmins = mins;
		idVec3 cmaxs = maxs;
		float zoffset = ImportExport::AimBot->TargetAimpoint().z;
		if (zoffset != 0.0f)
		{
			if (zoffset > 0.0f) 
			{
				cmaxs.z -= zoffset;
				cmins.z = -cmaxs.z;
			}
			else
			{
				cmins.z -= zoffset;
				cmaxs.z = -cmins.z;
			}
		}
		idVec3 targOrg = ImportExport::AimBot->TargetOrigin();
		idVec3 hit;
		if(CheckLineBox( targOrg + cmins, targOrg + cmaxs,
			user->EyePosition(), user->EyePosition() + (user->ViewAngles().ToMat3()[ 0 ] * user->DistanceTo( targOrg ) * 1.5f), hit))
		{
			int hitEnt = -1;
			return (user->CanSee(hit, user->Id(), &hitEnt) || hitEnt == ImportExport::AimBot->CurrentTarget()->Id());
		}
	}
	return false;
}

const idVec3 AutoFire::maxs = idVec3( 11.5f, 11.5f, 29.0f );
const idVec3 AutoFire::mins = idVec3( -11.5f, -11.5f, -23.0f );