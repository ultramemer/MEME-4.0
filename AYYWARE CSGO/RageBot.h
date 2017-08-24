/*
Syn's AyyWare Framework 2015
*/

#pragma once

#include "Hacks.h"
extern float autowalldmgtest[65];

class CRageBot : public CHack
{
public:
	void StartLagCompensation(IClientEntity * pEntity, CUserCmd * pCmd);
	void Init();
	void Draw();
	void Move(CUserCmd *pCmd, bool &bSendPacket);
private:
	// Targetting
	int GetTargetCrosshair();
	int GetTargetDistance();
	int GetTargetHealth();
	bool TargetMeetsRequirements(IClientEntity* pEntity);
	float FovToPlayer(Vector ViewOffSet, Vector View, IClientEntity* pEntity, int HitBox);
	int HitScan(IClientEntity* pEntity);
	void LBYJITTER3(CUserCmd * cmd, bool & packet);
	void circlestrafer(CUserCmd * cmd, Vector & Originalview);
	bool AimAtPoint(IClientEntity* pLocal, Vector point, CUserCmd *pCmd, bool &bSendPacket);


	// Functionality
	void DoAimbot(CUserCmd *pCmd, bool &bSendPacket);
	void NoSpread(CUserCmd * pCmd);
	void DoNoRecoil(CUserCmd *pCmd);
	void PositionAdjustment(CUserCmd* pCmd);

	// AntiAim
	void DoAntiAim(CUserCmd *pCmd, bool&bSendPacket);
	//bool EdgeAntiAim(IClientEntity* pLocalBaseEntity, CUserCmd* cmd, float flWall, float flCornor);
	//bool bEdge;

	// AimStep
	bool IsAimStepping;
	Vector LastAimstepAngle;
	Vector LastAngle;

	// Aimbot
	bool IsLocked;
	int TargetID;
	int HitBox;
	Vector AimPoint;
};