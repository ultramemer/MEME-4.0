/*
Syn's AyyWare Framework 2015
*/

#include "RageBot.h"
#include "RenderManager.h"
#include "Resolver.h"
#include "Autowall.h"
#include <iostream>
#include "UTIL Functions.h"
#define TIME_TO_TICKS( dt )	( ( int )( 0.5f + ( float )( dt ) / Interfaces::Globals->interval_per_tick ) )
#define M_PI 3.14159265358979323846
#define M_PI_F 3.14159265358979323846
static bool bFlip;


void CRageBot::Init()
{
	IsAimStepping = false;
	IsLocked = false;
	TargetID = -1;
}

void CRageBot::Draw()
{

}


bool IsAbleToShoot(IClientEntity* pLocal)
{
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());

	if (!pLocal)
		return false;

	if (!pWeapon)
		return false;

	float flServerTime = pLocal->GetTickBase() * Interfaces::Globals->interval_per_tick;

	return (!(pWeapon->GetNextPrimaryAttack() > flServerTime));
}

Vector TickPrediction(Vector AimPoint, IClientEntity* pTarget)
{
	return AimPoint + (pTarget->GetVelocity() * Interfaces::Globals->interval_per_tick);
}

float hitchance(IClientEntity* pLocal, CBaseCombatWeapon* pWeapon)
{
	//	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());
	float hitchance = 101;
	if (!pWeapon) return 0;
	if (Menu::Window.RageBotTab.AccuracyHitchance.GetValue() > 1)
	{//Inaccuracy method
		float inaccuracy = pWeapon->GetInaccuracy();
		if (inaccuracy == 0) inaccuracy = 0.0000001;
		inaccuracy = 1 / inaccuracy;
		hitchance = inaccuracy;

	}
	return hitchance;
}

float InterpolationFix()
{
	static ConVar* cvar_cl_interp = Interfaces::CVar->FindVar("cl_interp");
	static ConVar* cvar_cl_updaterate = Interfaces::CVar->FindVar("cl_updaterate");
	static ConVar* cvar_sv_maxupdaterate = Interfaces::CVar->FindVar("sv_maxupdaterate");
	static ConVar* cvar_sv_minupdaterate = Interfaces::CVar->FindVar("sv_minupdaterate");
	static ConVar* cvar_cl_interp_ratio = Interfaces::CVar->FindVar("cl_interp_ratio");

	IClientEntity* pLocal = hackManager.pLocal();
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());

	float cl_interp = cvar_cl_interp->GetFloat();
	int cl_updaterate = cvar_cl_updaterate->GetInt();
	int sv_maxupdaterate = cvar_sv_maxupdaterate->GetInt();
	int sv_minupdaterate = cvar_sv_minupdaterate->GetInt();
	int cl_interp_ratio = cvar_cl_interp_ratio->GetInt();

	if (sv_maxupdaterate <= cl_updaterate)
		cl_updaterate = sv_maxupdaterate;

	if (sv_minupdaterate > cl_updaterate)
		cl_updaterate = sv_minupdaterate;

	float new_interp = (float)cl_interp_ratio / (float)cl_updaterate;

	if (new_interp > cl_interp)
		cl_interp = new_interp;

	return max(cl_interp, cl_interp_ratio / cl_updaterate);
}

// (DWORD)g_pNetVars->GetOffset("DT_BaseCombatWeapon", "m_flNextPrimaryAttack");
// You need something like this
bool CanOpenFire() // Creds to untrusted guy
{
	IClientEntity* pLocalEntity = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	if (!pLocalEntity)
		return false;

	CBaseCombatWeapon* entwep = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocalEntity->GetActiveWeaponHandle());

	float flServerTime = (float)pLocalEntity->GetTickBase() * Interfaces::Globals->interval_per_tick;
	float flNextPrimaryAttack = entwep->GetNextPrimaryAttack();

	std::cout << flServerTime << " " << flNextPrimaryAttack << std::endl;

	return !(flNextPrimaryAttack > flServerTime);
}

void CRageBot::Move(CUserCmd *pCmd, bool &bSendPacket)
{
	IClientEntity* pLocalEntity = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	IClientEntity* LocalPlayer;
	if (!pLocalEntity)
		return;

	if (!Menu::Window.RageBotTab.AimbotEnable.GetState())
		return;

	if (Menu::Window.RageBotTab.AntiAimEnable.GetState())
	{
		static int ChokedPackets = -1;

		CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());
		if (!pWeapon)
			return;

		if (ChokedPackets < 1 && pLocalEntity->GetLifeState() == LIFE_ALIVE && pCmd->buttons & IN_ATTACK && CanOpenFire() && GameUtils::IsBallisticWeapon(pWeapon))
		{
			bSendPacket = false;
		}
		else
		{
			if (pLocalEntity->GetLifeState() == LIFE_ALIVE)
			{
				DoAntiAim(pCmd, bSendPacket);
			}
			ChokedPackets = -1;
		}
	}

	if (Menu::Window.RageBotTab.AccuracyPositionAdjustment.GetState())
		pCmd->tick_count = TIME_TO_TICKS(InterpolationFix());

	if (Menu::Window.RageBotTab.AccuracyPosition.GetState())
		PositionAdjustment(pCmd);

	if (Menu::Window.RageBotTab.AimbotEnable.GetState())
		DoAimbot(pCmd, bSendPacket);

	if (Menu::Window.RageBotTab.AccuracyRecoil.GetState())
		DoNoRecoil(pCmd);

	if (Menu::Window.RageBotTab.AimbotAimStep.GetState())
	{
		Vector AddAngs = pCmd->viewangles - LastAngle;
		if (AddAngs.Length2D() > 25.f)
		{
			Normalize(AddAngs, AddAngs);
			AddAngs *= 25;
			pCmd->viewangles = LastAngle + AddAngs;
			GameUtils::NormaliseViewAngle(pCmd->viewangles);
		}
	}
	LastAngle = pCmd->viewangles;
}

Vector BestPoint(IClientEntity *targetPlayer, Vector &final)
{
	IClientEntity* pLocal = hackManager.pLocal();

	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;

	filter.pSkip = targetPlayer;
	ray.Init(final + Vector(0, 0, 10), final);
	Interfaces::Trace->TraceRay(ray, MASK_SHOT, &filter, &tr);

	final = tr.endpos;
	return final;
}

// Functionality
void CRageBot::DoAimbot(CUserCmd *pCmd, bool &bSendPacket) /*------[Credits-Super]------*/
{
	IClientEntity* pTarget = nullptr;
	IClientEntity* LocalPlayer = hackManager.pLocal();
	CBaseCombatWeapon* CSWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(LocalPlayer->GetActiveWeaponHandle());
	bool NewTarget = true;

	/*	if (*CSWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == WEAPON_REVOLVER && Menu::Window.RageBotTab.AutoRevolver.GetState() && !Globals::Target)
	{
	static int delay = 0;
	delay++;

	if (delay <= 15)
	pCmd->buttons |= IN_ATTACK;
	else
	delay = 0;
	}
	*/
	if (CSWeapon)
	{
		if (CSWeapon->GetAmmoInClip() == 0 || !GameUtils::IsBallisticWeapon(CSWeapon))
		{
			return;
		}
	}

	if (IsLocked && TargetID >= 0 && HitBox >= 0)
	{
		pTarget = Interfaces::EntList->GetClientEntity(TargetID);
		if (pTarget  && TargetMeetsRequirements(pTarget))
		{
			HitBox = HitScan(pTarget);
			if (HitBox >= 0)
			{
				Vector ViewOffset = LocalPlayer->GetOrigin() + LocalPlayer->GetViewOffset();
				Vector View;
				Interfaces::Engine->GetViewAngles(View);

				float FoV = FovToPlayer(ViewOffset, View, pTarget, HitBox);
				if (FoV < Menu::Window.RageBotTab.AimbotFov.GetValue())
					NewTarget = false;
			}
		}
	}

	if (NewTarget)
	{
		TargetID = 0;
		pTarget = nullptr;
		HitBox = -1;

		switch (Menu::Window.RageBotTab.TargetSelection.GetIndex()) /*Target-Selection*/
		{
		case 0:
			TargetID = GetTargetCrosshair();
			break;
		case 1:
			TargetID = GetTargetDistance();
			break;
		case 2:
			TargetID = GetTargetHealth();
			break;
		}

		if (TargetID >= 0)
		{
			pTarget = Interfaces::EntList->GetClientEntity(TargetID);
		}
		else
		{
			pTarget = nullptr;
			HitBox = -1;
		}
	}

	if (TargetID >= 0 && pTarget) /*If we have a target*/
	{
		HitBox = HitScan(pTarget);

		if (!CanOpenFire())
			return;

		if (Menu::Window.RageBotTab.AimbotKeyPress.GetState()) /*Aim-On-Key*/
		{
			int Key = Menu::Window.RageBotTab.AimbotKeyBind.GetKey();
			if (Key >= 0 && !GUI.GetKeyState(Key))
			{
				TargetID = -1;
				pTarget = nullptr;
				HitBox = -1;
				return;
			}
		}




		Vector Point;
		Vector AimPoint = GetHitboxPosition(pTarget, HitBox);

		if (Menu::Window.RageBotTab.TargetMultipoint.GetState())
		{
			Point = BestPoint(pTarget, AimPoint);
		}
		else
		{
			Point = AimPoint;
		}

		pTarget->GetPredicted(AimPoint); /*Velocity Prediction*/
										 // Lets open fire

		if (GameUtils::IsScopedWeapon(CSWeapon) && !CSWeapon->IsScoped() && Menu::Window.RageBotTab.AccuracyAutoScope.GetState()) // Autoscope
		{
			pCmd->buttons |= IN_ATTACK2;
		}
		else
		{
			if ((Menu::Window.RageBotTab.AccuracyHitchance.GetValue() * 1.5 <= hitchance(LocalPlayer, CSWeapon)) || Menu::Window.RageBotTab.AccuracyHitchance.GetValue() == 0 || *CSWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == 64)
			{
				if (AimAtPoint(LocalPlayer, Point, pCmd, bSendPacket))
				{
					if (Menu::Window.RageBotTab.AimbotAutoFire.GetState() && !(pCmd->buttons & IN_ATTACK))
					{
						pCmd->buttons |= IN_ATTACK;
					}
					else
					{
						return;
					}
				}
				else if (Menu::Window.RageBotTab.AimbotAutoFire.GetState() && !(pCmd->buttons & IN_ATTACK))
				{
					pCmd->buttons |= IN_ATTACK;
				}
			}
		}
		if (Menu::Window.RageBotTab.PerfectAccuracy.GetState())
			Point = TickPrediction(Point, pTarget);

		if (IsAbleToShoot(LocalPlayer) && pCmd->buttons & IN_ATTACK)
			Globals::Shots += 1;

		// Stop and Crouch
		if (TargetID >= 0 && pTarget)
		{
			if (Menu::Window.RageBotTab.AccuracyAutoStop.GetState())
			{
				pCmd->forwardmove = 0.f;
				pCmd->sidemove = 0.f;
				pCmd->buttons |= IN_DUCK;
			}
		}
	}

	// Auto Pistol
	if (GameUtils::IsPistol(CSWeapon) && Menu::Window.RageBotTab.AimbotAutoPistol.GetState())
	{
		if (pCmd->buttons & IN_ATTACK)
		{
			static bool WasFiring = false;
			WasFiring = !WasFiring;

			if (WasFiring)
			{
				pCmd->buttons &= ~IN_ATTACK;
			}
		}
	}

	if (*CSWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == WEAPON_REVOLVER && Menu::Window.RageBotTab.AutoRevoler.GetState() && !Globals::Target)
	{
		static int delay = 0;
		delay++;

		if (delay <= 15)
			pCmd->buttons |= IN_ATTACK;
		else
			delay = 0;
	}
}



bool CRageBot::TargetMeetsRequirements(IClientEntity* pEntity)
{
	// Is a valid player
	if (pEntity && pEntity->IsDormant() == false && pEntity->IsAlive() && pEntity->GetIndex() != hackManager.pLocal()->GetIndex())
	{
		// Entity Type checks
		ClientClass *pClientClass = pEntity->GetClientClass();
		player_info_t pinfo;
		if (pClientClass->m_ClassID == (int)CSGOClassID::CCSPlayer && Interfaces::Engine->GetPlayerInfo(pEntity->GetIndex(), &pinfo))
		{
			// Team Check
			if (pEntity->GetTeamNum() != hackManager.pLocal()->GetTeamNum() || Menu::Window.RageBotTab.TargetFriendlyFire.GetState())
			{
				// Spawn Check
				if (!pEntity->HasGunGameImmunity())
				{
					return true;
				}
			}
		}
	}

	// They must have failed a requirement
	return false;
}

float CRageBot::FovToPlayer(Vector ViewOffSet, Vector View, IClientEntity* pEntity, int aHitBox)
{
	// Anything past 180 degrees is just going to wrap around
	CONST FLOAT MaxDegrees = 180.0f;

	// Get local angles
	Vector Angles = View;

	// Get local view / eye position
	Vector Origin = ViewOffSet;

	// Create and intiialize vectors for calculations below
	Vector Delta(0, 0, 0);
	//Vector Origin(0, 0, 0);
	Vector Forward(0, 0, 0);

	// Convert angles to normalized directional forward vector
	AngleVectors(Angles, &Forward);
	Vector AimPos = GetHitboxPosition(pEntity, aHitBox);
	// Get delta vector between our local eye position and passed vector
	VectorSubtract(AimPos, Origin, Delta);
	//Delta = AimPos - Origin;

	// Normalize our delta vector
	Normalize(Delta, Delta);

	// Get dot product between delta position and directional forward vectors
	FLOAT DotProduct = Forward.Dot(Delta);

	// Time to calculate the field of view
	return (acos(DotProduct) * (MaxDegrees / PI));
}

int CRageBot::GetTargetCrosshair()
{
	// Target selection
	int target = -1;
	float minFoV = Menu::Window.RageBotTab.AimbotFov.GetValue();

	IClientEntity* pLocal = hackManager.pLocal();
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector View; Interfaces::Engine->GetViewAngles(View);

	for (int i = 0; i < Interfaces::EntList->GetMaxEntities(); i++) //GetHighestEntityIndex()
	{
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntity))
		{
			int NewHitBox = HitScan(pEntity);
			if (NewHitBox >= 0)
			{
				float fov = FovToPlayer(ViewOffset, View, pEntity, 0);
				if (fov < minFoV)
				{
					minFoV = fov;
					target = i;
				}
			}
		}
	}

	return target;
}

int CRageBot::GetTargetDistance()
{
	// Target selection
	int target = -1;
	int minDist = 99999;

	IClientEntity* pLocal = hackManager.pLocal();
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector View; Interfaces::Engine->GetViewAngles(View);

	for (int i = 0; i < Interfaces::EntList->GetMaxEntities(); i++)
	{
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntity))
		{
			int NewHitBox = HitScan(pEntity);
			if (NewHitBox >= 0)
			{
				Vector Difference = pLocal->GetOrigin() - pEntity->GetOrigin();
				int Distance = Difference.Length();
				float fov = FovToPlayer(ViewOffset, View, pEntity, 0);
				if (Distance < minDist && fov < Menu::Window.RageBotTab.AimbotFov.GetValue())
				{
					minDist = Distance;
					target = i;
				}
			}
		}
	}

	return target;
}

int CRageBot::GetTargetHealth()
{
	// Target selection
	int target = -1;
	int minHealth = 101;

	IClientEntity* pLocal = hackManager.pLocal();
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector View; Interfaces::Engine->GetViewAngles(View);

	for (int i = 0; i < Interfaces::EntList->GetMaxEntities(); i++)
	{
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntity))
		{
			int NewHitBox = HitScan(pEntity);
			if (NewHitBox >= 0)
			{
				int Health = pEntity->GetHealth();
				float fov = FovToPlayer(ViewOffset, View, pEntity, 0);
				if (Health < minHealth && fov < Menu::Window.RageBotTab.AimbotFov.GetValue())
				{
					minHealth = Health;
					target = i;
				}
			}
		}
	}

	return target;
}

int CRageBot::HitScan(IClientEntity* pEntity)
{
	IClientEntity* pLocal = hackManager.pLocal();
	std::vector<int> HitBoxesToScan;

	// Get the hitboxes to scan
#pragma region GetHitboxesToScan
	int HitScanMode = Menu::Window.RageBotTab.TargetHitscan.GetIndex();
	int iSmart = Menu::Window.RageBotTab.AccuracySmart.GetValue();
	bool AWall = Menu::Window.RageBotTab.AccuracyAutoWall.GetState();
	bool Multipoint = Menu::Window.RageBotTab.TargetMultipoint.GetState();
	static bool enemyHP = false;
	if (HitScanMode == 0)
	{
		switch (Menu::Window.RageBotTab.TargetHitbox.GetIndex()) /*Single-Hitbox*/
		{
		case 0:
			HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
			break;
		case 1:
			HitBoxesToScan.push_back((int)CSGOHitboxID::Neck);
			HitBoxesToScan.push_back((int)CSGOHitboxID::NeckLower);
			break;
		case 2:
			HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
			break;
		case 3:
			HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
			break;
		case 4:
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightShin);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftShin);
			break;
		}
	}
	else
	{
		switch (HitScanMode)
		{
		case 1:
			/*Scan-Normal*/
			HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Neck);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
			HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftUpperArm);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightUpperArm);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftThigh);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightThigh);
			break;
		case 2:
			/*Scan-High*/
			HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Neck);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
			HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftUpperArm);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightUpperArm);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftThigh);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightThigh);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftShin);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightShin);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftLowerArm);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightLowerArm);
		case 3:
			/*Scan-All*/
			HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Neck);
			HitBoxesToScan.push_back((int)CSGOHitboxID::NeckLower);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
			HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LowerChest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftUpperArm);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightUpperArm);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftThigh);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightThigh);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftHand);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightHand);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftFoot);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightFoot);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftShin);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightShin);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftLowerArm);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightLowerArm);
		case 4:
			HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
			HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LowerChest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftFoot);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightFoot);
		}
	}
#pragma endregion Get the list of shit to scan

	// check hits
	// check hits
	if (AWall == 100) {
		enemyHP = true;
	}
	else {
		enemyHP = false;
	}


#pragma endregion Get the list of shit to scan

	// check hits
	// check hits
	for (auto HitBoxID : HitBoxesToScan)
	{
		if (AWall <= 99)
		{


			Vector Point = GetHitboxPosition(pEntity, HitBoxID);


			float Damage = 0.f;
			Color c = Color(255, 255, 255, 255);
			if (CanHit(Point, &Damage))
			{
				autowalldmgtest[pEntity->GetIndex()] = Damage;

				c = Color(0, 255, 0, 255);


				if (Damage >= Menu::Window.RageBotTab.AccuracyMinimumDamage.GetValue())
				{

					return HitBoxID;

				}
			}
			else {
				autowalldmgtest[pEntity->GetIndex()] = 0;
			}
		}
		else if (enemyHP) {
			Vector Point = GetHitboxPosition(pEntity, HitBoxID);
			float Damage = 0.f;
			Color c = Color(255, 255, 255, 255);
			if (CanHit(Point, &Damage))
			{
				autowalldmgtest[pEntity->GetIndex()] = Damage;
				c = Color(0, 255, 0, 255);
				if (Damage >= pEntity->GetHealth())
				{

					return HitBoxID;

				}
			}
		}
		else
		{
			if (GameUtils::IsVisible(hackManager.pLocal(), pEntity, HitBoxID))
				return HitBoxID;
		}
	}

	return -1;
}

void CRageBot::LBYJITTER3(CUserCmd* cmd, bool& packet)
{
	static bool ySwitch;
	static bool jbool;
	static bool jboolt;
	ySwitch = !ySwitch;
	jbool = !jbool;
	jboolt = !jbool;
	if (ySwitch)
	{
		if (jbool)
		{
			if (jboolt)
			{
				cmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() - 90.f;
				packet = false;
			}
			else
			{
				cmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() + 90.f;
				packet = false;
			}
		}
		else
		{
			if (jboolt)
			{
				cmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() - 125.f;
				packet = false;
			}
			else
			{
				cmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() + 125.f;
				packet = false;
			}
		}
	}
	else
	{
		cmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw();
		packet = true;
	}
}


void CRageBot::PositionAdjustment(CUserCmd* pCmd)
{
	static ConVar* cvar_cl_interp = Interfaces::CVar->FindVar("cl_interp");
	static ConVar* cvar_cl_updaterate = Interfaces::CVar->FindVar("cl_updaterate");
	static ConVar* cvar_sv_maxupdaterate = Interfaces::CVar->FindVar("sv_maxupdaterate");
	static ConVar* cvar_sv_minupdaterate = Interfaces::CVar->FindVar("sv_minupdaterate");
	static ConVar* cvar_cl_interp_ratio = Interfaces::CVar->FindVar("cl_interp_ratio");

	IClientEntity* pLocal = hackManager.pLocal();

	if (!pLocal)
		return;

	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());
	if (!pWeapon)
		return;

	float cl_interp = cvar_cl_interp->GetFloat();
	int cl_updaterate = cvar_cl_updaterate->GetInt();
	int sv_maxupdaterate = cvar_sv_maxupdaterate->GetInt();
	int sv_minupdaterate = cvar_sv_minupdaterate->GetInt();
	int cl_interp_ratio = cvar_cl_interp_ratio->GetInt();

	if (sv_maxupdaterate <= cl_updaterate)
		cl_updaterate = sv_maxupdaterate;

	if (sv_minupdaterate > cl_updaterate)
		cl_updaterate = sv_minupdaterate;

	float new_interp = (float)cl_interp_ratio / (float)cl_updaterate;

	if (new_interp > cl_interp)
		cl_interp = new_interp;

	float flSimTime = pLocal->GetSimulationTime();
	float flOldAnimTime = pLocal->GetAnimTime();

	int iTargetTickDiff = (int)(0.5f + (flSimTime - flOldAnimTime) / Interfaces::Globals->interval_per_tick);

	int result = (int)floorf(TIME_TO_TICKS(cl_interp)) + (int)floorf(TIME_TO_TICKS(pLocal->GetSimulationTime()));

	if ((result - pCmd->tick_count) >= -50)
	{
		pCmd->tick_count = result;
	}
}

void CRageBot::DoNoRecoil(CUserCmd *pCmd)
{
	IClientEntity* pLocal = hackManager.pLocal();
	if (pLocal)
	{
		Vector AimPunch = pLocal->localPlayerExclusive()->GetAimPunchAngle();
		if (AimPunch.Length2D() > 0 && AimPunch.Length2D() < 150)
		{
			pCmd->viewangles -= AimPunch * 2;
			GameUtils::NormaliseViewAngle(pCmd->viewangles);
		}
	}
}

void VectorAngles2(const Vector &vecForward, Vector &vecAngles)
{
	Vector vecView;
	if (vecForward[1] == 0.f && vecForward[0] == 0.f)
	{
		vecView[0] = 0.f;
		vecView[1] = 0.f;
	}
	else
	{
		vecView[1] = atan2(vecForward[1], vecForward[0]) * 180.f / M_PI;

		if (vecView[1] < 0.f)
			vecView[1] += 360.f;

		vecView[2] = sqrt(vecForward[0] * vecForward[0] + vecForward[1] * vecForward[1]);

		vecView[0] = atan2(vecForward[2], vecView[2]) * 180.f / M_PI;
	}

	vecAngles[0] = -vecView[0];
	vecAngles[1] = vecView[1];
	vecAngles[2] = 0.f;
}
void AngleVectors2(const Vector& qAngles, Vector& vecForward)
{
	float sp, sy, cp, cy;
	SinCos((float)(qAngles[1] * (M_PI / 180.f)), &sy, &cy);
	SinCos((float)(qAngles[0] * (M_PI / 180.f)), &sp, &cp);

	vecForward[0] = cp*cy;
	vecForward[1] = cp*sy;
	vecForward[2] = -sp;
}



bool EdgeAntiAim(IClientEntity* pLocalBaseEntity, CUserCmd* cmd, float flWall, float flCornor)
{
	Ray_t ray;
	trace_t tr;

	CTraceFilter traceFilter;
	traceFilter.pSkip = pLocalBaseEntity;

	auto bRetVal = false;
	auto vecCurPos = pLocalBaseEntity->GetEyePosition();

	for (float i = 0; i < 360; i++)
	{
		Vector vecDummy(10.f, cmd->viewangles.y, 0.f);
		vecDummy.y += i;

		NormalizeVector(vecDummy);

		Vector vecForward;
		AngleVectors2(vecDummy, vecForward);

		auto flLength = ((16.f + 3.f) + ((16.f + 3.f) * sin(DEG2RAD(10.f)))) + 7.f;
		vecForward *= flLength;

		ray.Init(vecCurPos, (vecCurPos + vecForward));
		Interfaces::Trace->TraceRay(ray, MASK_SHOT, (CTraceFilter *)&traceFilter, &tr);

		if (tr.fraction != 1.0f)
		{
			Vector qAngles;
			auto vecNegate = tr.plane.normal;

			vecNegate *= -1.f;
			VectorAngles2(vecNegate, qAngles);

			vecDummy.y = qAngles.y;

			NormalizeVector(vecDummy);
			trace_t leftTrace, rightTrace;

			Vector vecLeft;
			AngleVectors2(vecDummy + Vector(0.f, 30.f, 0.f), vecLeft);

			Vector vecRight;
			AngleVectors2(vecDummy - Vector(0.f, 30.f, 0.f), vecRight);

			vecLeft *= (flLength + (flLength * sin(DEG2RAD(30.f))));
			vecRight *= (flLength + (flLength * sin(DEG2RAD(30.f))));

			ray.Init(vecCurPos, (vecCurPos + vecLeft));
			Interfaces::Trace->TraceRay(ray, MASK_SHOT, (CTraceFilter*)&traceFilter, &leftTrace);

			ray.Init(vecCurPos, (vecCurPos + vecRight));
			Interfaces::Trace->TraceRay(ray, MASK_SHOT, (CTraceFilter*)&traceFilter, &rightTrace);

			if ((leftTrace.fraction == 1.f) && (rightTrace.fraction != 1.f))
				vecDummy.y -= flCornor; // left
			else if ((leftTrace.fraction != 1.f) && (rightTrace.fraction == 1.f))
				vecDummy.y += flCornor; // right			

			cmd->viewangles.y = vecDummy.y;
			cmd->viewangles.y -= flWall;
			cmd->viewangles.x = 89.0f;
			bRetVal = true;
		}
	}
	return bRetVal;
}

bool CRageBot::AimAtPoint(IClientEntity* pLocal, Vector point, CUserCmd *pCmd, bool &bSendPacket)
{
	bool ReturnValue = false;
	// Get the full angles
	if (point.Length() == 0) return ReturnValue;

	Vector angles;
	Vector src = pLocal->GetOrigin() + pLocal->GetViewOffset();

	CalcAngle(src, point, angles);
	GameUtils::NormaliseViewAngle(angles);

	if (angles[0] != angles[0] || angles[1] != angles[1])
	{
		return ReturnValue;
	}


	IsLocked = true;
	//-----------------------------------------------

	// Aim Step Calcs
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	if (!IsAimStepping)
		LastAimstepAngle = LastAngle; // Don't just use the viewangs because you need to consider aa

	float fovLeft = FovToPlayer(ViewOffset, LastAimstepAngle, Interfaces::EntList->GetClientEntity(TargetID), 0);

	if (fovLeft > 25.0f && Menu::Window.RageBotTab.AimbotAimStep.GetState())
	{
		Vector AddAngs = angles - LastAimstepAngle;
		Normalize(AddAngs, AddAngs);
		AddAngs *= 25;
		LastAimstepAngle += AddAngs;
		GameUtils::NormaliseViewAngle(LastAimstepAngle);
		angles = LastAimstepAngle;
	}
	else
	{
		ReturnValue = true;
	}

	// Silent Aim
	if (Menu::Window.RageBotTab.AimbotSilentAim.GetState() && !Menu::Window.RageBotTab.AimbotPerfectSilentAim.GetState())
	{
		pCmd->viewangles = angles;
	}

	// Normal Aim
	if (!Menu::Window.RageBotTab.AimbotSilentAim.GetState() && !Menu::Window.RageBotTab.AimbotPerfectSilentAim.GetState())
	{
		Interfaces::Engine->SetViewAngles(angles);
	}

	// pSilent Aim 
	Vector Oldview = pCmd->viewangles;

	if (Menu::Window.RageBotTab.AimbotPerfectSilentAim.GetState())
	{
		static int ChokedPackets = -1;
		ChokedPackets++;

		if (ChokedPackets < 6)
		{
			bSendPacket = false;
			pCmd->viewangles = angles;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles = Oldview;
			ChokedPackets = -1;
			ReturnValue = false;
		}
		
		//pCmd->viewangles.z = 0;
	}

	return ReturnValue;
}

namespace AntiAims // CanOpenFire checks for fake anti aims?
{
	// Pitches

	void JitterPitch(CUserCmd *pCmd)
	{
		static bool up = true;
		if (up)
		{
			pCmd->viewangles.x = 45;
			up = !up;
		}
		else
		{
			pCmd->viewangles.x = 89;
			up = !up;
		}
	}

	void perfect(CUserCmd *pCmd)
	{
		static bool iverse = false;
		static bool getbox = false;
		{
			if (iverse)
				pCmd->viewangles.x = 15;
			else
				pCmd->viewangles.x = -89;
		}
		iverse = !iverse;
		{
			if (getbox)
				pCmd->viewangles.x = -89;
			else
				pCmd->viewangles.x = Interfaces::Globals->curtime * 360;
		}
		getbox = !getbox;

	}

	void FakePitch(CUserCmd *pCmd, bool &bSendPacket)
	{	
		static int ChokedPackets = -1;
		ChokedPackets++;
		if (ChokedPackets < 1)
		{
			bSendPacket = false;
			pCmd->viewangles.x = 89;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles.x = 51;
			ChokedPackets = -1;
		}
	}

	void StaticJitter(CUserCmd *pCmd)
	{
		static bool down = true;
		if (down)
		{
			pCmd->viewangles.x = 179.0f;
			down = !down;
		}
		else
		{
			pCmd->viewangles.x = 89.0f;
			down = !down;
		}
	}

	void clickbait(CUserCmd *pCmd)
	{
		{
			int random = rand() % 100;
			int random2 = rand() % 1000;

			static bool dir;
			static float current_x = pCmd->viewangles.x;

			if (random == 1) dir = !dir;

			if (dir)
				current_x += 700;
			else
				current_x -= 34;

			pCmd->viewangles.x = current_x;

			if (random == random2)
				pCmd->viewangles.x += random;

		}

	}

	void FakeSlant(CUserCmd* pCmd, bool& bSendPacket)
	{
		static bool flip;
		flip = !flip;
		if (flip)
		{
			pCmd->viewangles.z = 25;
			bSendPacket = false;
		}
		else
		{
			pCmd->viewangles.z = -25;
			bSendPacket = true;
		}
	}


	void NASA(CUserCmd* pCmd, bool& bSendPacket)
	{
		static bool up = true;
		static bool flip;
		flip = !flip;
		if (flip)
		{
			pCmd->viewangles.z = 89.00;
			bSendPacket = false;
		}
		else
		{
			pCmd->viewangles.z = -89.00;
			bSendPacket = true;
		}
		if (up)
		{ 
			pCmd->viewangles.z = -1080.0;
			bSendPacket = true;
		}

	}

	void NASA3(CUserCmd* pCmd, bool& bSendPacket)
	{
		static bool flip;
		flip = !flip;
		if (flip)
		{
			pCmd->viewangles.z = 179.00;
			bSendPacket = false;
		}
		else
		{
			pCmd->viewangles.z = -0.01;
			bSendPacket = true;
		}
	}

	// Yaws

	void FastSpin(CUserCmd *pCmd)
	{
		static int y2 = -179;
		int spinBotSpeedFast = 100;

		y2 += spinBotSpeedFast;

		if (y2 >= 179)
			y2 = -179;

		pCmd->viewangles.y = y2;
	}

	void FakeZeroEmotion(CUserCmd *cmd, bool& bSendPacket)
	{
		static bool ySwitch = false;
		if (ySwitch)
		{
			bSendPacket = true;
			cmd->viewangles.x = 89.f;
			ySwitch = false;
		}
		else
		{
			bSendPacket = false;
			cmd->viewangles.x = 0;
			ySwitch = true;
		}
	}

	void KolyaAA(CUserCmd *pCmd) // best antiaim 9D
	{
		static bool Kolya = false;
		static bool Stas = false;
		static bool Nast9 = false;
		{
			if (Kolya)
				pCmd->viewangles.y = +89;
			else
				pCmd->viewangles.y = -89;

			Kolya = !Kolya;
		}
		{
			if (Stas)
				pCmd->viewangles.y = +11.0909384;
			else
				pCmd->viewangles.y = -12.0940304;
		}
		if (Kolya && Nast9)
		{
			Stas = true;
		}
		Stas = !Stas;
		Nast9 = !Nast9;
	}

	void KolyaAA1(CUserCmd *pCmd) // best antiaim 9D
	{
		static bool Kolya = false;
		static bool Stas = false;
		static bool Nast9 = false;
		{
			if (Kolya)
				pCmd->viewangles.y = +89;
			else
				pCmd->viewangles.y = -89;

			Kolya = !Kolya;
		}
		{
			if (Stas)
				pCmd->viewangles.y = +11.0909384;
			else
				pCmd->viewangles.y = -12.0940304;
		}
		if (Kolya && Nast9)
		{
			Stas = true;
		}
		Stas = !Stas;
		Nast9 = !Nast9;
	}

	void FakeEdge(CUserCmd *pCmd, bool &bSendPacket)
	{
		IClientEntity* pLocal = hackManager.pLocal();
		
		Vector vEyePos = pLocal->GetOrigin() + pLocal->GetViewOffset();

		CTraceFilter filter;
		filter.pSkip = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

		for (int y = 0; y < 360; y++)
		{
			Vector qTmp(10.0f, pCmd->viewangles.y, 0.0f);
			qTmp.y += y;

			if (qTmp.y > 180.0)
				qTmp.y -= 360.0;
			else if (qTmp.y < -180.0)
				qTmp.y += 360.0;

			GameUtils::NormaliseViewAngle(qTmp);

			Vector vForward;

			VectorAngles(qTmp, vForward);

			float fLength = (19.0f + (19.0f * sinf(DEG2RAD(10.0f)))) + 7.0f;
			vForward *= fLength;

			trace_t tr;

			Vector vTraceEnd = vEyePos + vForward;

			Ray_t ray;

			ray.Init(vEyePos, vTraceEnd);
			Interfaces::Trace->TraceRay(ray, MASK_PLAYERSOLID_BRUSHONLY, &filter, &tr);

			if (tr.fraction != 1.0f)
			{
				Vector angles;

				Vector vNegative = Vector(tr.plane.normal.x * -1.0f, tr.plane.normal.y * -1.0f, tr.plane.normal.z * -1.0f);

				VectorAngles(vNegative, angles);

				GameUtils::NormaliseViewAngle(angles);

				qTmp.y = angles.y;

				GameUtils::NormaliseViewAngle(qTmp);

				trace_t trLeft, trRight;

				Vector vLeft, vRight;
				VectorAngles(qTmp + Vector(0.0f, 30.0f, 0.0f), vLeft);
				VectorAngles(qTmp + Vector(0.0f, 30.0f, 0.0f), vRight);

				vLeft *= (fLength + (fLength * sinf(DEG2RAD(30.0f))));
				vRight *= (fLength + (fLength * sinf(DEG2RAD(30.0f))));

				vTraceEnd = vEyePos + vLeft;

				ray.Init(vEyePos, vTraceEnd);
				Interfaces::Trace->TraceRay(ray, MASK_PLAYERSOLID_BRUSHONLY, &filter, &trLeft);

				vTraceEnd = vEyePos + vRight;

				ray.Init(vEyePos, vTraceEnd);
				Interfaces::Trace->TraceRay(ray, MASK_PLAYERSOLID_BRUSHONLY, &filter, &trRight);

				if ((trLeft.fraction == 1.0f) && (trRight.fraction != 1.0f))
					qTmp.y -= 90.f;
				else if ((trLeft.fraction != 1.0f) && (trRight.fraction == 1.0f))
					qTmp.y += 90.f;

				if (qTmp.y > 180.0)
					qTmp.y -= 360.0;
				else if (qTmp.y < -180.0)
					qTmp.y += 360.0;

				pCmd->viewangles.y = qTmp.y;

				int offset = Menu::Window.RageBotTab.AntiAimOffset.GetValue();

				static int ChokedPackets = -1;
				ChokedPackets++;
				if (ChokedPackets < 1)
				{
					bSendPacket = false; // +=180?
				}
				else
				{
					bSendPacket = true;
					pCmd->viewangles.y -= offset;
					ChokedPackets = -1;
				}
				return;
			}
		}
		pCmd->viewangles.y += 360.0f;
	}
	
	void BackJitter(CUserCmd *pCmd)
	{
		int random = rand() % 100;

		// Small chance of starting fowards
		if (random < 98)
			// Look backwards
			pCmd->viewangles.y -= 180;

		// Some gitter
		if (random < 15)
		{
			float change = -70 + (rand() % (int)(140 + 1));
			pCmd->viewangles.y += change;
		}
		if (random == 69)
		{
			float change = -90 + (rand() % (int)(180 + 1));
			pCmd->viewangles.y += change;
		}
	}

	void FakeSideways(CUserCmd *pCmd, bool &bSendPacket)
	{
		pCmd->viewangles.y -= bFlip ? -90.0f : -90.0f;
		bFlip = bFlip;

		if (pCmd->viewangles.y -= bFlip ? -90.0f : -90.0f)
			pCmd->viewangles.y -= bFlip ? 90.0f : -90.0f;

		else if (pCmd->viewangles.y -= bFlip ? 90.0f : -90.0f)
			pCmd->viewangles.y -= bFlip ? -90.0f : 90.0f;

		return;
	}

	void TEST(CUserCmd *pCmd, bool &bSendPacket)
	{
		static float fYaw = 0.0f;
		static float fYaw2 = 0.0f;
		static float rYaw = 0.0f;
		static bool bFlip_0;

		if (bFlip)
		{
			bFlip_0 = !bFlip_0;
			pCmd->viewangles.y -= bFlip_0 ? 90.0f : -90.0f;
		}

		fYaw += 5.0f;

		if (fYaw > 100)
			fYaw = 0.0f;
		else if (fYaw < 50.0f)
			rYaw = 150.f;
		else if (fYaw > 100)
			rYaw = 210.f;

		pCmd->viewangles.y -= 180.0f;
	}

	void Jitter(CUserCmd *pCmd)
	{
		static int jitterangle = 0;

		if (jitterangle <= 1)
		{
			pCmd->viewangles.y += 90;
		}
		else if (jitterangle > 1 && jitterangle <= 3)
		{
			pCmd->viewangles.y -= 90;
		}

		int re = rand() % 4 + 1;


		if (jitterangle <= 1)
		{
			if (re == 4)
				pCmd->viewangles.y += 180;
			jitterangle += 1;
		}
		else if (jitterangle > 1 && jitterangle <= 3)
		{
			if (re == 4)
				pCmd->viewangles.y -= 180;
			jitterangle += 1;
		}
		else
		{
			jitterangle = 0;
		}
	}

	void Flip(CUserCmd *pCmd)
	{
		static bool back = false;
		back = !back;
		if (back)
			pCmd->viewangles.y -= rand() % 100;
		else
			pCmd->viewangles.y += rand() % 100;

	}

	void FastSpin2(CUserCmd *pCmd)
	{
		int random = rand() % 100;
		int random2 = rand() % 1000;

		static bool dir;
		static float current_y = pCmd->viewangles.y;

		if (random == 1) dir = !dir;

		if (dir)
			current_y += 100;
		else
			current_y -= 100;

		pCmd->viewangles.y = current_y;

		if (random == random2)
			pCmd->viewangles.y += random;
	}

	void Meme(CUserCmd *pCmd)
	{
		static bool aaSwitch = false;
		static bool pFake = false;
		if (aaSwitch)
			pCmd->viewangles.y += 360000000;
		else
			pCmd->viewangles.y -= 360000000;
		//it's only When psilent working, so u need add ur Config pSilent function like this
		// if (!Config->GetValue("Aimbot", "pSilent").m_bValue)
		if (pFake)
			pCmd->viewangles.x += 360 * 160;
		else
			pCmd->viewangles.y = pCmd->viewangles.y + pCmd->viewangles.x;
		aaSwitch = !aaSwitch;
		pFake = !pFake;
	}

	void BackJitter3(CUserCmd *pCmd)
	{
		static int jitterangle = 0;
		static bool sendpackets = false;
		static bool bSendPacket = true;
		if (sendpackets)
		{
			bSendPacket = true;
			if (jitterangle)
			{
				pCmd->viewangles.y -= 145.0f;
				jitterangle = false;
			}
			else
			{
				pCmd->viewangles.y -= 215.0f;
				jitterangle = true;
			}
			sendpackets = false;
		}
		else
		{
			bSendPacket = false;
			pCmd->viewangles.y -= 360.0f;
			sendpackets = true;
		}
	}

	void SidewaysJitter(CUserCmd *pCmd)
	{
		static int jitterswitch = 0;

		if (jitterswitch)
		{
			pCmd->viewangles.y += 90.0;
			jitterswitch = false;
		}
		else
		{
			pCmd->viewangles.y -= 90.0;

			jitterswitch = true;
		}
	}

	void FakeJitter2(CUserCmd* pCmd, bool &bSendPacket)
	{
		Vector vMove(pCmd->forwardmove, pCmd->sidemove, pCmd->upmove);
		float flSpeed = sqrt(vMove.x * vMove.x + vMove.y * vMove.y), flYaw;
		Vector vMove2;
		Vector vRealView(pCmd->viewangles);

		static int jitterangle = 0;

		if (jitterangle <= 1)
		{
			pCmd->viewangles.x = 88;
			pCmd->viewangles.y += 90;
		}
		else if (jitterangle > 1 && jitterangle <= 3)
		{
			pCmd->viewangles.x = 88;
			pCmd->viewangles.y -= 90;
		}

		static int iChoked = -1;
		iChoked++;
		{
			bSendPacket = false;
			if (jitterangle <= 1)
			{
				pCmd->viewangles.y += 180;
				jitterangle += 1;
			}
			else if (jitterangle > 1 && jitterangle <= 3)
			{
				pCmd->viewangles.y -= 180;
				jitterangle += 1;
			}
			else
			{
				jitterangle = 0;
			}
		}
	}
	void LBYJitter2(CUserCmd* cmd, bool& packet)
	{
		static bool ySwitch;
		static bool jbool;
		static bool jboolt;
		ySwitch = !ySwitch;
		jbool = !jbool;
		jboolt = !jbool;
		if (ySwitch)
		{
			if (jbool)
			{
				if (jboolt)
				{
					cmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() - 90.f;
					packet = false;
				}
				else
				{
					cmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() + 90.f;
					packet = false;
				}
			}
			else
			{
				if (jboolt)
				{
					cmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() - 125.f;
					packet = false;
				}
				else
				{
					cmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() + 125.f;
					packet = false;
				}
			}
		}
		else
		{
			cmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw();
			packet = true;
		}
	}

	void FakeJitterSynced(CUserCmd* pCmd, bool& bSendPacket)
	{
		static bool flip;
		flip = !flip;
		if (pCmd->command_number % 3)
		{
			if (flip)
			{
				pCmd->viewangles.y = pCmd->viewangles.y + 200.0;
				bSendPacket = false;
			}
			else
			{
				pCmd->viewangles.y = pCmd->viewangles.y - 200.0;
				bSendPacket = true;
			}
		}
	}

	void pAntiAim(CUserCmd* pCmd, bool& bSendPacket)
	{
		static bool flip;
		flip = !flip;
		if (pCmd->command_number % 3)
		{
			if (flip)
			{
				pCmd->viewangles.y = pCmd->viewangles.y + 36000180.0;
				bSendPacket = false;
			}
			else
			{
				pCmd->viewangles.y = pCmd->viewangles.y - 36000180.0;
				bSendPacket = true;
			}

		
		}

	}

	void FakeStatic(CUserCmd *pCmd, bool &bSendPacket)
	{
		static int ChokedPackets = -1;
		ChokedPackets++;
		if (ChokedPackets < 1)
		{
			bSendPacket = false;
			static int y2 = -179;
			int spinBotSpeedFast = 360.0f / 1.618033988749895f;;

			y2 += spinBotSpeedFast;

			if (y2 >= 179)
				y2 = -179;

			pCmd->viewangles.y = y2;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles.y -= 180;
			ChokedPackets = -1;
		}
	}

	void TJitter(CUserCmd *pCmd)
	{
		static bool Turbo = true;
		if (Turbo)
		{
			pCmd->viewangles.y -= 90;
			Turbo = !Turbo;
		}
		else
		{
			pCmd->viewangles.y += 90;
			Turbo = !Turbo;
		}
	}

	void TFake(CUserCmd *pCmd, bool &bSendPacket)
	{
		static int ChokedPackets = -1;
		ChokedPackets++;
		if (ChokedPackets < 1)
		{
			bSendPacket = false;
			pCmd->viewangles.y = -90;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles.y = 90;
			ChokedPackets = -1;
		}
	}

	void FakeJitter(CUserCmd* pCmd, bool &bSendPacket)
	{
		static int jitterangle = 0;

		if (jitterangle <= 1)
		{
			pCmd->viewangles.y += 135;
		}
		else if (jitterangle > 1 && jitterangle <= 3)
		{
			pCmd->viewangles.y += 225;
		}

		static int iChoked = -1;
		iChoked++;
		if (iChoked < 1)
		{
			bSendPacket = false;
			if (jitterangle <= 1)
			{
				pCmd->viewangles.y += 45;
				jitterangle += 1;
			}
			else if (jitterangle > 1 && jitterangle <= 3)
			{
				pCmd->viewangles.y -= 45;
				jitterangle += 1;
			}
			else
			{
				jitterangle = 0;
			}
		}
		else
		{
			bSendPacket = true;
			iChoked = -1;
		}
	}

	void FakeSpin(CUserCmd* pCmd, bool& bSendPacket) {

		int random = rand() % 100;
		int random2 = rand() % 1000;

		static bool dir;
		static float current_y = pCmd->viewangles.y;

		if (random == 1) dir = !dir;

		if (dir)
			current_y += 36000.00f;
		else
			current_y -= 36000.00f;
		bSendPacket = true; //fake angle
		pCmd->viewangles.y = current_y;

		if (random == random2)
			pCmd->viewangles.y += random;

	}


	void Static(CUserCmd *pCmd)
	{
		static bool aa1 = false;
		aa1 = !aa1;
		if (aa1)
		{
			static bool turbo = false;
			turbo = !turbo;
			if (turbo)
			{
				pCmd->viewangles.y -= 90;
			}
			else
			{
				pCmd->viewangles.y += 90;
			}
		}
		else
		{
			pCmd->viewangles.y -= 180;
		}
	}
	void fakelbymeme(CUserCmd *pCmd, bool &bSendPacket)
	{
		if (pCmd->command_number % 2)
		{
			bSendPacket = false;
			pCmd->viewangles.y -= 180.f;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() + 90;
		}
	}
	void fakelowerbody(CUserCmd *pCmd, bool &bSendPacket)
	{
		static bool f_flip = true;
		f_flip = !f_flip;

		if (f_flip)
		{
			pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() + 90.00f;
			bSendPacket = false;
		}
		else if (!f_flip)
		{
			pCmd->viewangles.y += hackManager.pLocal()->GetLowerBodyYaw() - 90.00f;
			bSendPacket = true;
		}
	}
	void LBYJITTER(CUserCmd *pCmd, bool &bSendPacket)
	{
		static bool f_flip = true;
		f_flip = !f_flip;

		if (f_flip)
		{
			pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() + 179.0f;
			bSendPacket = false;
		}
		else if (!f_flip)
		{
			pCmd->viewangles.y += hackManager.pLocal()->GetLowerBodyYaw() + 0.1f;
			bSendPacket = true;
		}
	}
	void LBY1(CUserCmd *pCmd, bool &bSendPacket)
	{
		static bool f_flip = true;
		f_flip = !f_flip;

		if (f_flip)
		{
			pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() + 45.00f;
			bSendPacket = false;
		}
		else if (!f_flip)
		{
			pCmd->viewangles.y += hackManager.pLocal()->GetLowerBodyYaw() - 45.00f;
			bSendPacket = true;
		}
	}
	void LBY2(CUserCmd *pCmd, bool &bSendPacket)
	{
		static bool f_flip = true;
		f_flip = !f_flip;

		if (f_flip)
		{
			pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() + 35.00f;
			bSendPacket = false;
		}
		else if (!f_flip)
		{
			pCmd->viewangles.y += hackManager.pLocal()->GetLowerBodyYaw() - 35.00f;
			bSendPacket = true;
		}
	}

	/*
	___________________________________________________________________________________________________________________________________________________________________________
	________         __                        __      __
	/        |       /  |                      /  \    /  |
	$$$$$$$$/______  $$ |   __   ______        $$  \  /$$/______   __   __   __
	$$ |__  /      \ $$ |  /  | /      \        $$  \/$$//      \ /  | /  | /  |						bSendPacket = true  <-- Fake Angle
	$$    | $$$$$$  |$$ |_/$$/ /$$$$$$  |        $$  $$/ $$$$$$  |$$ | $$ | $$ |						bSendPacket = false <-- True Angle
	$$$$$/  /    $$ |$$   $$<  $$    $$ |         $$$$/  /    $$ |$$ | $$ | $$ |
	$$ |   /$$$$$$$ |$$$$$$  \ $$$$$$$$/           $$ | /$$$$$$$ |$$ \_$$ \_$$ |
	$$ |   $$    $$ |$$ | $$  |$$       |          $$ | $$    $$ |$$   $$   $$/
	$$/     $$$$$$$/ $$/   $$/  $$$$$$$/           $$/   $$$$$$$/  $$$$$/$$$$/
	___________________________________________________________________________________________________________________________________________________________________________
	*/
	//fake yaw
	void Arizona1(CUserCmd *pCmd, bool &bSendPacket) //Jitter Movement mit static bool Fast :)
	{
		static bool Fast = false;
		if (Fast)
		{
			bSendPacket = false; //true angle
			pCmd->viewangles.y = pCmd->viewangles.y - 134.0;
		}
		else
		{
			bSendPacket = true; //fake angle
			pCmd->viewangles.y = pCmd->viewangles.y - 226.0;
		}
		Fast = !Fast;
	}

	void Backwards1(CUserCmd *pCmd, bool &bSendPacket)
	{
		bSendPacket = true; //fake angle
		pCmd->viewangles.y -= 180.0f;
	}

	void Forward1(CUserCmd *pCmd, bool &bSendPacket)
	{
		bSendPacket = true; //fake angle
		pCmd->viewangles.y += 180.0f;
	}

	void SidewaysLeft1(CUserCmd *pCmd, bool &bSendPacket)
	{
		bSendPacket = true; //fake angle
		pCmd->viewangles.y -= 90.0f;
	}

	void SidewaysRight1(CUserCmd *pCmd, bool &bSendPacket)
	{
		bSendPacket = true; //fake angle
		pCmd->viewangles.y += 90.0f;
	}

	void SpinSlow1(CUserCmd *pCmd, bool &bSendPacket)
	{
		int random = rand() % 100;
		int random2 = rand() % 1000;

		static bool dir;
		static float current_y = pCmd->viewangles.y;

		if (random == 1) dir = !dir;

		if (dir)
			current_y += 10;
		else
			current_y -= 10;

		pCmd->viewangles.y = current_y;

		if (random == random2)
			pCmd->viewangles.y += random;


	}

	void SpinFast1(CUserCmd *pCmd, bool &bSendPacket)
	{
		int random = 160 + rand() % 40;
		static float current_y = pCmd->viewangles.y;
		current_y += random;
		bSendPacket = true; //fake angle
		pCmd->viewangles.y = current_y;
	}

	void JitterBackward1(CUserCmd *pCmd, bool &bSendPacket)
	{
		int random = rand() % 100;

		// Small chance of starting fowards
		if (random < 98)
			// Look backwards
			pCmd->viewangles.y -= 180;

		// Some gitter
		if (random < 15)
		{
			float change = -70 + (rand() % (int)(140 + 1));
			pCmd->viewangles.y += change;
		}
		if (random == 69)
		{
			float change = -90 + (rand() % (int)(180 + 1));
			pCmd->viewangles.y += change;
		}
	}

	void JitterForward1(CUserCmd *pCmd, bool &bSendPacket)
	{
		int random = rand() % 100;

		// Small chance of starting fowards
		if (random < 98)
			// Look backwards
			pCmd->viewangles.y += 180;

		// Some gitter
		if (random < 15)
		{
			float change = -70 + (rand() % (int)(140 + 1));
			pCmd->viewangles.y += change;
		}
		if (random == 69)
		{
			float change = -90 + (rand() % (int)(180 + 1));
			pCmd->viewangles.y += change;
		}
	}

	void pFakeRandom(bool& bSendPacket, CUserCmd *cmd)
	{
		static bool faked;
		if (faked)
		{
			bSendPacket = true;
			cmd->viewangles.y = rand() % 360;
			faked = false;
		}
		else
		{
			bSendPacket = false;
			if (Menu::Window.RageBotTab.RealRandom.GetIndex() == 0)//Forward
			{

			}
			if (Menu::Window.RageBotTab.RealRandom.GetIndex() == 1)//Backward
			{
				cmd->viewangles += 180;
			}
			if (Menu::Window.RageBotTab.RealRandom.GetIndex() == 2)//Left
			{
				cmd->viewangles += 90;
			}
			if (Menu::Window.RageBotTab.RealRandom.GetIndex() == 3)//Right
			{
				cmd->viewangles -= 90;
			}
			faked = true;
		}
	}

	void ZetaFakeBody(CUserCmd *pCmd, bool bSendPacket)
	{
		static int iChoked = -1;
		iChoked++;
		static bool baim;
		if (baim)
		{
			bSendPacket = false;
			pCmd->viewangles.y -= 92;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles.y += 92;
			iChoked = -1;
		}
	}

	void JitterSideways1(CUserCmd *pCmd, bool &bSendPacket)
	{
		int random = rand() % 100;

		// Small chance of starting fowards
		if (random < 98)
			// Look backwards
			pCmd->viewangles.y -= 90;

		// Some gitter
		if (random < 15)
		{
			float change = -70 + (rand() % (int)(140 + 1));
			pCmd->viewangles.y += change;
		}
		if (random == 69)
		{
			float change = -90 + (rand() % (int)(180 + 1));
			pCmd->viewangles.y += change;
		}
	}

	void YawZero1(CUserCmd *pCmd, bool &bSendPacket)
	{
		bSendPacket = true; //fake angle
		pCmd->viewangles.y = 0.f;
	}

	void Fsd(CUserCmd *pCmd, bool& bSendPacket)
	{
		static bool fkang = true;

		if (fkang)
		{
			pCmd->viewangles.y -= 130;
		}
		else
		{
			pCmd->viewangles.y += 30;
			bSendPacket = false;
		}
	}

	void PLISP(CUserCmd *pCmd, bool& bSendPacket)
	{
		static bool fkang = false;

		if (fkang)
		{
			pCmd->viewangles.y -= 140;
			bSendPacket = true;
		}
		else
		{
			pCmd->viewangles.y -= 140;
			bSendPacket = false;
		}
		fkang = !fkang;
	}

	/*void CustomFakeYaw1(CUserCmd *pCmd, bool &bSendPacket)
	{
		int pitchOffset = Menu::Window.RageBotTab2.CustomYaw.GetValue(); //AA Offset
		static int ChokedPackets = -1;
		ChokedPackets++;
		if (ChokedPackets < 1)
		{
			bSendPacket = false; // +=180?
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles.y = pitchOffset;
			ChokedPackets = -1;
		}
	}
	*/
	void LowerBody1(CUserCmd *pCmd, bool &bSendPacket)
	{
		static bool f_flip = true;
		f_flip = !f_flip;

		if (f_flip)
		{
			//pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() + 90.00f; //Yaw setzt sich zusammen aus LowerBody + 90° Drehung
			bSendPacket = false;
		}
		else if (!f_flip)
		{
			pCmd->viewangles.y += hackManager.pLocal()->GetLowerBodyYaw() - 90.00f;
			bSendPacket = true;
		}
		f_flip = !f_flip;
	}

	/*void FakeLowerBody1(CUserCmd *pCmd, bool &bSendPacket)
	{

		random = rand() % 100;
		maxJitter = rand() % (85 - 70 + 1) + 70;
		temp = hackManager.pLocal()->GetLowerBodyYaw() - (rand() % maxJitter);
		if (random < 35 + (rand() % 15))
		{
			bSendPacket = false;
			//pCmd->viewangles.y -= temp;
		}

		else if (random < 85 + (rand() % 15))
		{
			bSendPacket = true;
			pCmd->viewangles.y += temp;
		}
	}


	*/
	void AimAtTarget(CUserCmd *pCmd)
	{
		IClientEntity* pLocal = hackManager.pLocal();

		CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());

		if (!pLocal || !pWeapon)
			return;

		Vector eye_position = pLocal->GetEyePosition();

		float best_dist = pWeapon->GetCSWpnData()->flRange;

		IClientEntity* target = Interfaces::EntList->GetClientEntity(Globals::TargetID);

		if (target == NULL)
			return;

		if (target == pLocal)
			return;

		if ((target->GetTeamNum() == pLocal->GetTeamNum()) || target->IsDormant() || !target->IsAlive() || target->GetHealth() <= 0)
			return;

		Vector target_position = target->GetEyePosition();

		float temp_dist = eye_position.DistTo(target_position);

		if (best_dist > temp_dist)
		{
			best_dist = temp_dist;
			CalcAngle(eye_position, target_position, pCmd->viewangles);
		}
	}

	void EdgeDetect(CUserCmd* pCmd, bool &bSendPacket)
	{
		//Ray_t ray;
		//trace_t tr;

		IClientEntity* pLocal = hackManager.pLocal();

		CTraceFilter traceFilter;
		traceFilter.pSkip = pLocal;

		bool bEdge = false;

		Vector angle;
		Vector eyePos = pLocal->GetOrigin() + pLocal->GetViewOffset();

		for (float i = 0; i < 360; i++)
		{
			Vector vecDummy(10.f, pCmd->viewangles.y, 0.f);
			vecDummy.y += i;

			Vector forward = vecDummy.Forward();

			//vecDummy.NormalizeInPlace();

			float flLength = ((16.f + 3.f) + ((16.f + 3.f) * sin(DEG2RAD(10.f)))) + 7.f;
			forward *= flLength;

			Ray_t ray;
			CGameTrace tr;

			ray.Init(eyePos, (eyePos + forward));
			Interfaces::Trace->EdgeTraceRay(ray, traceFilter, tr, true);

			if (tr.fraction != 1.0f)
			{
				Vector negate = tr.plane.normal;
				negate *= -1;

				Vector vecAng = negate.Angle();

				vecDummy.y = vecAng.y;

				//vecDummy.NormalizeInPlace();
				trace_t leftTrace, rightTrace;

				Vector left = (vecDummy + Vector(0, 45, 0)).Forward(); // or 45
				Vector right = (vecDummy - Vector(0, 45, 0)).Forward();

				left *= (flLength * cosf(rad(30)) * 2); //left *= (len * cosf(rad(30)) * 2);
				right *= (flLength * cosf(rad(30)) * 2); // right *= (len * cosf(rad(30)) * 2);

				ray.Init(eyePos, (eyePos + left));
				Interfaces::Trace->EdgeTraceRay(ray, traceFilter, leftTrace, true);

				ray.Init(eyePos, (eyePos + right));
				Interfaces::Trace->EdgeTraceRay(ray, traceFilter, rightTrace, true);

				if ((leftTrace.fraction == 1.f) && (rightTrace.fraction != 1.f))
				{
					vecDummy.y -= 45; // left
				}
				else if ((leftTrace.fraction != 1.f) && (rightTrace.fraction == 1.f))
				{
					vecDummy.y += 45; // right     
				}

				angle.y = vecDummy.y;
				angle.y += 360;
				bEdge = true;
			}
		}

		if (bEdge)
		{
			static bool turbo = true;

			switch (Menu::Window.RageBotTab.AntiAimEdge.GetIndex())
			{
			case 0:
				// Nothing
				break;
			case 1:
				// Regular
				AntiAims::fakelowerbody(pCmd, bSendPacket);
				break;
			}
		}
	}
}

QAngle CreateMoveAngles;

// AntiAim
void CRageBot::DoAntiAim(CUserCmd *pCmd, bool &bSendPacket) // pCmd->viewangles.y = 0xFFFFF INT_MAX or idk
{
	IClientEntity* pLocal = hackManager.pLocal();

	if ((pCmd->buttons & IN_USE) || pLocal->GetMoveType() == MOVETYPE_LADDER)
		return;
	
	// If the aimbot is doing something don't do anything
	if ((IsAimStepping || pCmd->buttons & IN_ATTACK) && !Menu::Window.RageBotTab.AimbotPerfectSilentAim.GetState())
		return;

	// Weapon shit
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());
	if (pWeapon)
	{
		CSWeaponInfo* pWeaponInfo = pWeapon->GetCSWpnData();
		// Knives or grenades
		if (!GameUtils::IsBallisticWeapon(pWeapon))
		{
			if (Menu::Window.RageBotTab.AntiAimKnife.GetState())
			{
				if (!CanOpenFire() || pCmd->buttons & IN_ATTACK2)
					return;
			}
			else
			{
				return;
			}
		}
	}



	// Don't do antiaim
	// if (DoExit) return;

	// Anti-Aim Pitch
	switch (Menu::Window.RageBotTab.AntiAimPitch.GetIndex()) // Magic pitch is 69.69?
	{
	case 0:
		// No Pitch AA
		break;
	case 1:
		// Down
		//AntiAims::StaticPitch(pCmd, false);
		pCmd->viewangles.x = 89.0f;
		break;
	case 2:
		// Half Down
		pCmd->viewangles.x = 51.f;
		break;
	case 3:
		// SMAC / Casual safe
		AntiAims::JitterPitch(pCmd);
		break;
	case 4:
		// Jitter
		pCmd->viewangles.x = 179.0f;
		break;
	case 5:
		// Fake Pitch
		pCmd->viewangles.x = -181.f;
		break;
	case 6:
		// Static Down
		pCmd->viewangles.x = 1800089.0f;
		break;
	case 7:
		// Static Jitter
		pCmd->viewangles.x = -1800089.0f;
		break;
	case 8:
		// Emotion
		pCmd->viewangles.x = 89.0000;
		break;
	case 9:
		//Clickbait
		AntiAims::clickbait(pCmd);
		break;
	case 10:
		//FakeSlat
		AntiAims::FakeSlant(pCmd, bSendPacket);
		break;
	case 12:
		//ZERO
		pCmd->viewangles.x = 0.0f;
		break;
	case 11:
		//ZERO
		AntiAims::NASA(pCmd, bSendPacket);
		break;
	case 13:
		//ZERO
		AntiAims::NASA3(pCmd, bSendPacket);
		break;
	case 14:
		//ZERO
		AntiAims::perfect(pCmd);
		break;
	case 15:
		//FakeOverFlow
		pCmd->viewangles.x = -2137;
		pCmd->viewangles.x -= 2137;
		pCmd->viewangles.x += 1337;
		break;
	case 16:
		//FakeEmotionZero
		AntiAims::FakeZeroEmotion(pCmd, bSendPacket);
		break;
	}
	if (Menu::Window.RageBotTab.AntiAimTarget.GetState())
	{
		AntiAims::AimAtTarget(pCmd);
	}
	if (!bSendPacket && Menu::Window.RageBotTab.AntiAimYaw.GetIndex() > 0)
		CreateMoveAngles = pCmd->viewangles;
	//Anti-Aim Yaw
	switch (Menu::Window.RageBotTab.AntiAimYaw.GetIndex())
	{
	case 0:
		// No Yaw AA
		break;
	case 1:
		// Fake Inverse
		AntiAims::FakeEdge(pCmd, bSendPacket);
		break;
	case 2:
		// Fake Sideways
		AntiAims::fakelbymeme(pCmd, bSendPacket);
		break;
	case 3:
		// Fake Static
		AntiAims::FakeStatic(pCmd, bSendPacket);
		break;
	case 4:
		// Fake Inverse
		AntiAims::TFake(pCmd, bSendPacket);
		break;
	case 5:
		// Fake Jitter
		AntiAims::FakeJitter(pCmd, bSendPacket);
		break;
	case 6:
		// Jitter
		AntiAims::Static(pCmd);
		break;
	case 7:
		// T Jitter
		AntiAims::TJitter(pCmd);
		break;
	case 8:
		// Back Jitter
		AntiAims::BackJitter(pCmd);
		break; 
	case 9:
		// T Inverse
		pCmd->viewangles.y -= 180;
		break;
	case 10:
		// T Inverse
		AntiAims::fakelowerbody(pCmd, bSendPacket);
		break;
	case 11:
		// FakeSpin
		AntiAims::FakeSpin(pCmd, bSendPacket);
		break;
	case 12:
		// LBYJITTER
		AntiAims::LBYJITTER(pCmd, bSendPacket);
		break;
	case 13:
		// LBY1
		AntiAims::LBY1(pCmd, bSendPacket);
		break;
	case 14:
		// LBY2
		AntiAims::LBY2(pCmd, bSendPacket);
		break;
	case 15:
		// Meme
		AntiAims::Meme(pCmd);
		break;
	case 16:
		// FastSpin2
		AntiAims::FastSpin2(pCmd);
		break;
	case 17:
		// Flip
		AntiAims::Flip(pCmd);
		break;
	case 18:
		//		AntiAims::FakeJitterSynced(pCmd, bSendPacket);
		AntiAims::FakeJitterSynced(pCmd, bSendPacket);
		break;
	case 19:
		//		AntiAims::FakeJitterSynced(pCmd, bSendPacket);
		AntiAims::KolyaAA(pCmd);
		break;
	case 20:
		//		AntiAims::FakeJitterSynced(pCmd, bSendPacket);
		AntiAims::ZetaFakeBody(pCmd, bSendPacket);
		break;

	case 21:
		//		AntiAims::FakeJitterSynced(pCmd, bSendPacket);
		AntiAims::PLISP(pCmd, bSendPacket);
		break;

	case 22:
		//		AntiAims::FakeJitterSynced(pCmd, bSendPacket);
		AntiAims::Fsd(pCmd, bSendPacket);
		break;
	case 23:
		//		AntiAims::FakeJitterSynced(pCmd, bSendPacket);
		AntiAims::pAntiAim(pCmd, bSendPacket);
		break;

	case 24:
		//		AntiAims::FakeJitterSynced(pCmd, bSendPacket);
		AntiAims::TEST(pCmd, bSendPacket);
		break;
	}
	pCmd->viewangles.y += Menu::Window.RageBotTab.AntiAimOffset.GetValue();
	if (Menu::Window.RageBotTab.AntiResolver.GetIndex() == 0)
	{
		//off
	}

	if (Menu::Window.RageBotTab.AntiResolver.GetIndex() == 1)
	{
		static bool antiResolverFlip = false;
		if (pCmd->viewangles.y == pLocal->GetLowerBodyYaw())
		{
			if (antiResolverFlip)
				pCmd->viewangles.y += 60.f;
			else
				pCmd->viewangles.y -= 60.f;
			antiResolverFlip = !antiResolverFlip;
		}
	}
	if (Menu::Window.RageBotTab.AntiResolver.GetIndex() == 2)
	{
		static bool antiResolverFlip = false;
		if (pCmd->viewangles.y == pLocal->GetLowerBodyYaw())
		{
			if (antiResolverFlip)
				pCmd->viewangles.y += 50.f;
			else
				pCmd->viewangles.y -= 90.f;
			antiResolverFlip = !antiResolverFlip;
		}
	}
	if (Menu::Window.RageBotTab.AntiResolver.GetIndex() == 3)
	{
		static bool antiResolverFlip = false;
		if (pCmd->viewangles.y == pLocal->GetLowerBodyYaw())
		{
			if (antiResolverFlip)
				pCmd->viewangles.y += 20.f;
			else
				pCmd->viewangles.y -= 35.f;
			antiResolverFlip = !antiResolverFlip;
			if (!antiResolverFlip)
				pCmd->viewangles.y -= 45.f;
			else
				pCmd->viewangles.y -= 55.f;
			antiResolverFlip = !antiResolverFlip;
		}
	}

	//Anti-Aim Fake Yaw
	switch (Menu::Window.RageBotTab.FakeYawAA.GetIndex()) // Magic pitch is 69.69?
	{
	case 0:
		// No Yaw AA
		break;
	case 1:
		//
		AntiAims::Arizona1(pCmd, bSendPacket);
		break;
	case 2:
		// 
		AntiAims::Backwards1(pCmd, bSendPacket);
		break;
	case 3:
		// 
		AntiAims::Forward1(pCmd, bSendPacket);
		break;
	case 4:
		// 
		AntiAims::SidewaysLeft1(pCmd, bSendPacket);
		break;
	case 5:
		//
		AntiAims::SidewaysRight1(pCmd, bSendPacket);
		break;
	case 6:
		// 
		AntiAims::SpinSlow1(pCmd, bSendPacket);
		break;
	case 7:
		// 
		AntiAims::SpinFast1(pCmd, bSendPacket);
		break;
	case 8:
		// 
		AntiAims::JitterBackward1(pCmd, bSendPacket);
		break;
	case 9:
		// 
		AntiAims::JitterForward1(pCmd, bSendPacket);
		break;
	case 10:
		// 
		AntiAims::JitterSideways1(pCmd, bSendPacket);
		break;
	case 11:
		// 
		AntiAims::YawZero1(pCmd, bSendPacket);
		break;
	case 12:
		// 
		AntiAims::LowerBody1(pCmd, bSendPacket);
		break;
	case 13:
		
		AntiAims::fakelbymeme(pCmd, bSendPacket);
		break;

		
		//Anti-Aim Fake Yaw




	// Edge Anti Aim
	//AntiAims::EdgeDetect(pCmd, bSendPacket); this is broken it seems ill just remove it cuz what if it causes some crashes

	// Angle offset
	pCmd->viewangles.y += Menu::Window.RageBotTab.AntiAimOffset.GetValue();
}
}


