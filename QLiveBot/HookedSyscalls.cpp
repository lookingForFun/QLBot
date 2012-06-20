#include <sstream>
#include <iomanip>
#include <memory>
#include "Engine.h"
#include "DrawUtils.h"
#include "../Common/AutoFire.h"
#include "AccuracyCounter.h"
#include "Timers.h"

//Arbitrary
#define ENTTYPE_ITEM_NUM 2222

void DoPredictedAF()
{
	static bool autoshooting = false;
	static int startTime = 0;
	if(AutoFire::AimingAtTarget())
	{
		if(ImportExport::Variables[AUTOFIRE]->IsEnabled())
		{
			int afdr = ImportExport::Variables[AUTOFIRE_DELAY_RANDOM]->IntValue();
			if(startTime == 0 && (afdr > 0 || ImportExport::Variables[AUTOFIRE_DELAY]->IntValue() > 0))
				startTime = Engine::GetTime() + ImportExport::Variables[AUTOFIRE_DELAY]->IntValue() + (afdr > 0 ? rand() % afdr : 0);
			if((Engine::GetTime() - startTime) >= 0)
			{
				Engine::Click();
				autoshooting = true;
			}
		}
	}
	else 
	{
		startTime = 0;
		if(autoshooting)
		{
			autoshooting = false;
			Engine::Unclick();
		}
	}
}

int EffectsEnt_Filter(int entityNum)
{
	int qeVal = ImportExport::Variables[QUADENTITIES]->IntValue();
	if(entityNum >= 0 && entityNum < 64)
	{
		if(ImportExport::AimBot->IsValidEnemy(entityNum))
			return 1+(qeVal > 0 ? qeVal : 0);
	}
	else if(entityNum == ENTTYPE_ITEM_NUM)
	{
		return 1+(qeVal > 0 && (qeVal & 16) ? qeVal : 0);
	}
	return 0;
}

extern int healths[MAX_CLIENTS];
__declspec(noinline) int CG_S_StartLocalSound(int shaderHandle, int arg1)
{
	if(shaderHandle == Shaders[XORSTR("hs0")] || shaderHandle == Shaders[XORSTR("hs1")] || shaderHandle == Shaders[XORSTR("hs2")] || shaderHandle == Shaders[XORSTR("hs3")])
	{
		AccuracyManager::HitRegistered(ImportExport::Entities->GetPlayer(Engine::GetClientNum())->Weapon().Id());
	}
	else if(shaderHandle == Shaders[XORSTR("fightSound")])
	{
		for(int i = 0; i < MAX_CLIENTS; ++i)
			healths[i] = 100;
		AccuracyManager::Init();
		timers.Clear();
	}
	return Original_Syscall(CG_S_STARTLOCALSOUND, shaderHandle, arg1);
}

__declspec(noinline) int CG_S_StartSound(int arg0, int arg1, int arg2, int arg3)
{
	if(arg3 == Shaders[XORSTR("quadSound")])
	{
		timers.Add(XORSTR("Quad Damage"), Engine::GetGameType() < 3 ? 60000 : 120000);
	}
	else if(arg3 == Shaders[XORSTR("megaSound")])
	{
		timers.Add(XORSTR("Mega Health"), 35000);
	}
	else if(arg1 == Engine::GetClientNum() && ImportExport::Variables[MUTESELF]->IsEnabled())
	{
		return 0;
	}
	return Original_Syscall(CG_S_STARTSOUND, arg0, arg1, arg2, arg3);
}

__declspec(noinline) int CG_R_AddRefEntityToScene(LPINT ent)
{
	if((*ent == 0 || (*ent == 2 && Engine::GetGameType() != 4)))//check if reType == RT_MODEL or RT_SPRITE - simpleitems
	{
		int action = EffectsEnt_Filter(g_EntityBeingAdded);
		if(action)
		{
			if(ImportExport::Variables[WALLHACK]->IsEnabled()) //wallhack
				Engine::SetWallhack(ent, true);
			if(action > 1) //then add glowshells
			{
				if(action == 2) //the glowshell will not be visible through walls
				{
					Original_Syscall(CG_R_ADDREFENTITYTOSCENE, ent);
					Engine::SetWallhack(ent, false);
					*(int*)(ent + REFENTITY_CUSTOMSHADER) = Shaders[XORSTR("regen")]; //ent.customShader = regenShader
					return Original_Syscall(CG_R_ADDREFENTITYTOSCENE, ent);
				}

				if(action == 3) //the glowshell will be visible through walls
				{
					Original_Syscall(CG_R_ADDREFENTITYTOSCENE, ent);
					Engine::SetWallhack(ent, true);
					*(int*)(ent + REFENTITY_CUSTOMSHADER) = Shaders[XORSTR("regen")]; //ent.customShader = regenShader
					return Original_Syscall(CG_R_ADDREFENTITYTOSCENE, ent);
				}
				if(action == 4) //the glowshell will be visible through the walls and there will be the white overlay on the player
				{
#define SETSHADERCOLOR(ent, r, g, b, a) { *reinterpret_cast<byte*>(ent + REFENTITY_SHADERRGBA) = r; \
					*reinterpret_cast<byte*>(ent + REFENTITY_SHADERRGBA + 1) = g; \
					*reinterpret_cast<byte*>(ent + REFENTITY_SHADERRGBA + 2) = b; \
					*reinterpret_cast<byte*>(ent + REFENTITY_SHADERRGBA + 3) = a; }
					//add a flat white cham
					*(int*)(ent + REFENTITY_CUSTOMSHADER) = Shaders[XORSTR("white")]; //ent.customShader = whiteShader
					SETSHADERCOLOR(ent, 0xFF, 0x00, 0x00, 0xFF);
					Original_Syscall(CG_R_ADDREFENTITYTOSCENE, ent);
					//add the glowshell
					Engine::SetWallhack(ent, true);
					*(int*)(ent + REFENTITY_CUSTOMSHADER) = Shaders[XORSTR("regen")]; //ent.customShader = regenShader
					Original_Syscall(CG_R_ADDREFENTITYTOSCENE, ent);
					return Original_Syscall(CG_R_ADDREFENTITYTOSCENE, ent);
				}
				//ignore any other value
				return Original_Syscall(CG_R_ADDREFENTITYTOSCENE, ent);
			}
			else
			{
				return Original_Syscall(CG_R_ADDREFENTITYTOSCENE, ent); //no glowshells, just wallhack, at most
			}
		}
	}
	return Original_Syscall(CG_R_ADDREFENTITYTOSCENE, ent);
}

bool g_firstRenderScene = false;
__declspec(noinline) int CG_R_RenderScene(int arg0)
{
	// RenderScene is called many times per frame
	if(g_firstRenderScene)
	{
		g_firstRenderScene = false;
		// Let's make sure that the map is fully loaded
		if(Engine::HasValidSnapshot() && Engine::GetTime() != 0)
		{
			// Get aiming angles for the ideal target, after performing humanization if required
			idAngles ang = ImportExport::AimBot->Main(ImportExport::Variables[FOV]->FloatValue(),
				8.0f /*+ ((float)(*(int*)(getSnapshot()+4)))*/,
				ImportExport::Variables[HUMAN]->IsEnabled(),
				ImportExport::Variables[SMOOTHMULTIPLIER]->FloatValue(),
				ImportExport::Variables[SMOOTHTRANSPOSITION]->FloatValue());
			// If the aimbot has a target and if auto-aim is enabled, then update the angles
			// REMARK: this will only be effective upon NEXT frame
			if(ImportExport::AimBot->CurrentTarget() != nullptr && ImportExport::Variables[AIM]->IsEnabled())
			{
				idAngles * engineAngles = Engine::GetAngles();
				engineAngles->yaw += ang.yaw;
				engineAngles->pitch += ang.pitch;
			}
			// If the aimbot found a target for which a prediction bos can be drawn and the user wants prediction boxes, draw the thing
			if(ImportExport::Variables[BOX]->IsEnabled() && ImportExport::AimBot->BoxTargetValid())
				DrawUtils::Draw3dBox(AutoFire::mins+ImportExport::AimBot->BoxTargetOrigin(),
				AutoFire::maxs+ImportExport::AimBot->BoxTargetOrigin(),
				Colors::TransGreen);
			DoPredictedAF();
		}
	}
	return Original_Syscall(CG_R_RENDERSCENE, arg0);
}