/*
AyyWare 2 - Extreme Alien Technology
By Syn
*/

#pragma once

#include "Interfaces.h"
#include "Hacks.h"

class cGameEventManager : public IGameEventListener
{
public:
	void FireGameEvent(IGameEvent* event);
	void RegisterSelf();
	bool ShouldHitmarker();
	bool ShouldRoundStart();

private:
	bool doHitmarker;
	bool dodmg;
	bool doRoundStart;
};

class CHitMarker : public CHack
{
public:
	void Init();
	void Draw();
	void Move(CUserCmd *pCmd, bool &bSendPacket) {}
	cGameEventManager EventMan;
};

struct DamageIndicator_t
{
	int iDamage;
	bool bInitialized;
	float flEraseTime;
	float flLastUpdate;
	IClientEntity * Player;
	Vector Position;
};

extern CHitMarker HitMarker;