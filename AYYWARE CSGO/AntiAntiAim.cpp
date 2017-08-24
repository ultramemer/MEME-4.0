#include "Interfaces.h"
#include "Menu.h"
#include "AntiAntiAim.h"
#include "Resolver.h"
#include "MiscDefinitions.h"
#include "Entities.h"
#include "Hooks.h"
#include "Hacks.h"



int Globals::Shots = 0;
bool Globals::change;
bool Globals::LBYUpdate;
CUserCmd* Globals::UserCmd;
int Globals::TargetID;
IClientEntity* Globals::Target;
float YawDelta[64];
float reset[64];
float Delta[64];
float OldLowerBodyYaw[64];
float Resolved_angles[64];
int iSmart;
static int jitter = -1;

void NormalizeVector1337(float& lowerDelta) {
	for (int i = 0; i < 3; ++i) {
		while (lowerDelta > 180.f)
			lowerDelta -= 360.f;

		while (lowerDelta < -180.f)
			lowerDelta += 360.f;
	}
	lowerDelta = 0.f;
}


int GetEstimatedServerTickCount(float latency)
{
	return (int)floorf((float)((float)(latency) / (float)((uintptr_t)&Interfaces::Globals->interval_per_tick)) + 0.5) + 1 + (int)((uintptr_t)&Interfaces::Globals->tickcount);
}




void FixY(const CRecvProxyData *pData, void *pStruct, void *pOut)
{

	static Vector vLast[65];
	static bool bShotLastTime[65];
	static bool bJitterFix[65];

	float *flPitch = (float*)((DWORD)pOut - 4);
	float flYaw = pData->m_Value.m_Float;
	bool bHasAA;
	bool bSpinbot;


#define YawResolver Menu::Window.RageBotTab.AccuracyResolverYaw.GetIndex()
	switch (YawResolver)
	{

	case 0:
	{
		break;
	}
	case 1:
	{
		IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
		for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); ++i)
		{
			IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);


			if (!pEntity || pEntity->IsDormant() || !pEntity->IsAlive())
				continue;

			if (pEntity->GetTeamNum() == pLocal->GetTeamNum() || !pLocal->IsAlive())
				continue;

			player_info_t pTemp;
			if (!Interfaces::Engine->GetPlayerInfo(i, &pTemp))
				continue;
			Vector* eyeAngles = pEntity->GetEyeAnglesPointer();

			float oldlowerbodyyaw;
			bool bLowerBodyUpdated = false;
			bool IsUsingFakeAngles = false;
			static bool isMoving;
			float PlayerIsMoving = abs(pEntity->GetVelocity().Length());
			if (PlayerIsMoving > 0.1) isMoving = true;
			else if (PlayerIsMoving <= 0.1) isMoving = false;
			float TLBY = pEntity->GetLowerBodyYaw();
			float backtrack;



			if (!PlayerIsMoving || PlayerIsMoving) // HUUGE YAW RESOLVER
			{
				if (Menu::Window.RageBotTab.Sinister.GetState())
				{
					bool bLowerBodyUpdated = false;
					bool IsUsingFakeAngles = false;
					float oldlowerbodyyaw;
					if (oldlowerbodyyaw != TLBY)
					{
						bLowerBodyUpdated = true;
					}
					float bodyeyedelta = pEntity->GetEyeAngles().y - flYaw;
					if (PlayerIsMoving || bLowerBodyUpdated)// || LastUpdatedNetVars->eyeangles.x != CurrentNetVars->eyeangles.x || LastUpdatedNetVars->eyeyaw != CurrentNetVars->eyeangles.y)
					{
						if (bLowerBodyUpdated || (PlayerIsMoving && bodyeyedelta >= 35.0f))
						{
							flYaw = TLBY;
							oldlowerbodyyaw = flYaw;
						}

						IsUsingFakeAngles = false;
					}
					else
					{
						if (bodyeyedelta > 35.0f)
						{
							flYaw = oldlowerbodyyaw;
							IsUsingFakeAngles = true;
						}
						else
						{
							IsUsingFakeAngles = false;
						}
					}
					if (IsUsingFakeAngles)
					{
						int com = GetEstimatedServerTickCount(90);

						if (com % 2)
						{
							flYaw += 90;
						}
						else if (com % 3)
							flYaw -= 90;
						else
							flYaw -= 0;
					}
				}

				if (Menu::Window.RageBotTab.Resolver.GetState())
				{
					IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

					for (int i = 0; i < Interfaces::Engine->GetMaxClients(); ++i)
					{
						IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);

						if (!pEntity || pEntity->IsDormant() || !pEntity->IsAlive())
							continue;

						if (pEntity->GetTeamNum() == pLocal->GetTeamNum() || !pLocal->IsAlive())
							continue;

						pEntity->GetEyeAngles3()->y = *pEntity->GetLowerBodyYawTarget();
					}
				}

				if (Menu::Window.RageBotTab.AccuracyBrute.GetIndex() == 1)
				{
					int num = Globals::Shots % 9;
					switch (num) {
					case 0:flYaw = TLBY + 180; break;
					case 1:flYaw = TLBY - 90; break;
					case 2:flYaw = TLBY + 90; break;
					case 3:flYaw = TLBY + 179.95; break;
					case 4:flYaw = TLBY - 179.95; break;
					case 5:flYaw = TLBY + 40; break;
					case 6:flYaw = TLBY + 30; break;
					case 7:flYaw = TLBY - 30; break;
					case 8:flYaw = TLBY + 15; break;
					}
					if (Menu::Window.RageBotTab.AccuracyBrute.GetIndex() == 2)
					{
						// Anglefix
						bHasAA = ((*flPitch == 90.0f) || (*flPitch == 270.0f));
						bSpinbot = false;

						if (!bShotLastTime[((IClientEntity*)(pStruct))->GetIndex()]
							&& (fabsf(flYaw - vLast[((IClientEntity*)(pStruct))->GetIndex()].y) > 15.0f) && !bHasAA)
						{
							flYaw = vLast[((IClientEntity*)(pStruct))->GetIndex()].y;
							bShotLastTime[((IClientEntity*)(pStruct))->GetIndex()] = true;
						}
						else
						{
							if (bShotLastTime[((IClientEntity*)(pStruct))->GetIndex()]
								&& (fabsf(flYaw - vLast[((IClientEntity*)(pStruct))->GetIndex()].y) > 15.0f))
							{
								bShotLastTime[((IClientEntity*)(pStruct))->GetIndex()] = true;
								bSpinbot = true;
							}
							else
							{
								bShotLastTime[((IClientEntity*)(pStruct))->GetIndex()] = false;
							}
						}

						vLast[((IClientEntity*)(pStruct))->GetIndex()].y = flYaw;


						bool bTmp = bJitterFix[((IClientEntity*)(pStruct))->GetIndex()];

						bJitterFix[((IClientEntity*)(pStruct))->GetIndex()] = (flYaw >= 180.0f && flYaw <= 360.0f);

						if (bTmp && (flYaw >= 0.0f && flYaw <= 180.0f))
						{
							flYaw += 359.0f;
						}
						if (Menu::Window.RageBotTab.AccuracyBrute.GetIndex() == 3)
						{
							int ServerTime = (float)Interfaces::Globals->interval_per_tick * hackManager.pLocal()->GetTickBase() * 1.4000;
							bool flip = !flip;
							int value = ServerTime % 2;
							static bool Turbo = false;
							{

								switch (value) {
								case 1:

									if (Turbo)
									{
										flYaw -= TLBY / 2 + 135;
										Turbo = !Turbo;
									}
									else
									{
										flYaw += TLBY / 2 + 135;
										Turbo = !Turbo;
									}
									if (flYaw > 5) {
										flYaw = 180;
									}
									if (Menu::Window.RageBotTab.AccuracyBrute.GetIndex() == 4)
									{
										float spin;
										spin++;
										flYaw = spin;

									}
									if (Menu::Window.RageBotTab.AccuracyBrute.GetIndex() == 5)
									{
										static float OldLowerBodyYaws[64];
										static float OldYawDeltas[64];
										float CurYaw = TLBY;
										if (OldLowerBodyYaws[i] != CurYaw) {
											OldYawDeltas[i] = flYaw - CurYaw;
											OldLowerBodyYaws[i] = CurYaw;
											flYaw = CurYaw;
											continue;
										}
										else {
											flYaw = TLBY - OldYawDeltas[i];
										}
									}
									if (Menu::Window.RageBotTab.AccuracyBrute.GetIndex() == 6)
									{
										static float lowerDelta[64];
										static float lastYaw[64];

										float curLower = pEntity->GetLowerBodyYaw();
										float curYaw = flYaw;

										lowerDelta[i] = curYaw - curLower;
										NormalizeVector1337(lowerDelta[i]);

										if (fabs(lowerDelta[i]) > 15.f) {
											if (pEntity->isMoving()) {
												flYaw = TLBY;
											}
											else {
												if (curYaw != lastYaw[i]) {
													flYaw += lowerDelta[i];
													lastYaw[i] = curYaw;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}

		}
		break;
	}

	}
	*(float*)(pOut) = flYaw;
}


// Simple Resolver for Fake Down



RecvVarProxyFn oRecvnModelIndex;

void Hooked_RecvProxy_Viewmodel(CRecvProxyData *pData, void *pStruct, void *pOut)
{
	// Get the knife view model id's
	int default_t = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_default_t.mdl");
	int default_ct = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_default_ct.mdl");
	int iBayonet = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_bayonet.mdl");
	int iButterfly = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_butterfly.mdl");
	int iFlip = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_flip.mdl");
	int iGut = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_gut.mdl");
	int iKarambit = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_karam.mdl");
	int iM9Bayonet = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_m9_bay.mdl");
	int iHuntsman = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_tactical.mdl");
	int iFalchion = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_falchion_advanced.mdl");
	int iDagger = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_push.mdl");
	int iBowie = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_survival_bowie.mdl");

	int iGunGame = Interfaces::ModelInfo->GetModelIndex("models/weapons/v_knife_gg.mdl");

	// Get local player (just to stop replacing spectators knifes)
	IClientEntity* pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	if (Menu::Window.SkinchangerTab.SkinEnable.GetState() && pLocal)
	{
		// If we are alive and holding a default knife(if we already have a knife don't worry about changing)
		if (pLocal->IsAlive() && (
			pData->m_Value.m_Int == default_t ||
			pData->m_Value.m_Int == default_ct ||
			pData->m_Value.m_Int == iBayonet ||
			pData->m_Value.m_Int == iButterfly ||
			pData->m_Value.m_Int == iFlip ||
			pData->m_Value.m_Int == iGunGame ||
			pData->m_Value.m_Int == iGut ||
			pData->m_Value.m_Int == iKarambit ||
			pData->m_Value.m_Int == iM9Bayonet ||
			pData->m_Value.m_Int == iHuntsman ||
			pData->m_Value.m_Int == iFalchion ||
			pData->m_Value.m_Int == iDagger ||
			pData->m_Value.m_Int == iBowie))
		{
			// Set whatever knife we want
			if (Menu::Window.SkinchangerTab.KnifeModel.GetIndex() == 0)
				pData->m_Value.m_Int = iBayonet;
			else if (Menu::Window.SkinchangerTab.KnifeModel.GetIndex() == 1)
				pData->m_Value.m_Int = iBowie;
			else if (Menu::Window.SkinchangerTab.KnifeModel.GetIndex() == 2)
				pData->m_Value.m_Int = iButterfly;
			else if (Menu::Window.SkinchangerTab.KnifeModel.GetIndex() == 3)
				pData->m_Value.m_Int = iFalchion;
			else if (Menu::Window.SkinchangerTab.KnifeModel.GetIndex() == 4)
				pData->m_Value.m_Int = iFlip;
			else if (Menu::Window.SkinchangerTab.KnifeModel.GetIndex() == 5)
				pData->m_Value.m_Int = iGut;
			else if (Menu::Window.SkinchangerTab.KnifeModel.GetIndex() == 6)
				pData->m_Value.m_Int = iHuntsman;
			else if (Menu::Window.SkinchangerTab.KnifeModel.GetIndex() == 7)
				pData->m_Value.m_Int = iKarambit;
			else if (Menu::Window.SkinchangerTab.KnifeModel.GetIndex() == 8)
				pData->m_Value.m_Int = iM9Bayonet;
			else if (Menu::Window.SkinchangerTab.KnifeModel.GetIndex() == 9)
				pData->m_Value.m_Int = iDagger;
		}
	}

	// Carry on the to original proxy
	oRecvnModelIndex(pData, pStruct, pOut);
}

void ApplyAAAHooks()
{
	ClientClass *pClass = Interfaces::Client->GetAllClasses();
	while (pClass)
	{
		const char *pszName = pClass->m_pRecvTable->m_pNetTableName;
		if (!strcmp(pszName, "DT_CSPlayer"))
		{
			for (int i = 0; i < pClass->m_pRecvTable->m_nProps; i++)
			{
				RecvProp *pProp = &(pClass->m_pRecvTable->m_pProps[i]);
				const char *name = pProp->m_pVarName;


				// Yaw Fix
				if (!strcmp(name, "m_angEyeAngles[1]"))
				{
					Utilities::Log("Yaw Fix Applied");
					pProp->m_ProxyFn = FixY;

				}
			}
		}
		else if (!strcmp(pszName, "DT_BaseViewModel"))
		{
			for (int i = 0; i < pClass->m_pRecvTable->m_nProps; i++)
			{
				RecvProp *pProp = &(pClass->m_pRecvTable->m_pProps[i]);
				const char *name = pProp->m_pVarName;

				// Knives
				if (!strcmp(name, "m_nModelIndex"))
				{
					oRecvnModelIndex = (RecvVarProxyFn)pProp->m_ProxyFn;
					pProp->m_ProxyFn = Hooked_RecvProxy_Viewmodel;
				}
			}
		}
		pClass = pClass->m_pNext;
	}
}