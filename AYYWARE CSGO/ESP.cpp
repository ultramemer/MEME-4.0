#include "ESP.h"
#include "Interfaces.h"
#include "RenderManager.h"
#include "Autowall.h"
#include "RageBot.h"
#include "Resolver.h"
#include <iostream>
#include "UTIL Functions.h"

void CEsp::Init()
{
	BombCarrier = nullptr;
}

// Yeah dude we're defo gunna do some sick moves for the esp yeah
void CEsp::Move(CUserCmd *pCmd, bool& bSendPacket)
{

}

float DistanceTo(Vector vecDst, Vector vecSrc)
{
	Vector vDelta = vecDst - vecSrc;

	float fDistance = ::sqrtf((vDelta.Length()));

	if (fDistance < 1.0f)
		return 1.0f;

	return fDistance;
}


// Main ESP Drawing loop
void CEsp::Draw()
{
	IClientEntity *pLocal = hackManager.pLocal();

	// Loop through all active entitys
	for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
	{
		// Get the entity
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);
		player_info_t pinfo;

		// The entity isn't some laggy peice of shit or something

		if (pEntity &&  pEntity != pLocal && !pEntity->IsDormant())
		{
			// Is it a player?!
				if (Interfaces::Engine->GetPlayerInfo(i, &pinfo)&& pEntity->IsAlive())
				{
					DrawPlayer(pEntity, pinfo);

					if (Menu::Window.VisualsTab.selfesp.GetState())
					{

						IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
						DrawPlayer(pLocal, pinfo);
					}

				}

			// ~ Other ESP's here (items and shit) ~ //
			ClientClass* cClass = (ClientClass*)pEntity->GetClientClass();

			// Dropped weapons
			if ( cClass->m_ClassID != (int)CSGOClassID::CBaseWeaponWorldModel && ((strstr(cClass->m_pNetworkName, "Weapon") || cClass->m_ClassID == (int)CSGOClassID::CDEagle || cClass->m_ClassID == (int)CSGOClassID::CAK47)))
			{
				DrawDrop(pEntity, cClass);
			}
			// If entity is the bomb
			if (Menu::Window.VisualsTab.OptionsPlant.GetState())
			{
				if (cClass->m_ClassID == (int)CSGOClassID::CPlantedC4)
					DrawBombPlanted2(pEntity, cClass);

				if (cClass->m_ClassID == (int)CSGOClassID::CPlantedC4)
					DrawBombPlanted(pEntity, cClass);

				if (cClass->m_ClassID == (int)CSGOClassID::CC4)
					DrawBomb(pEntity, cClass);
			}

			if (Menu::Window.VisualsTab.Nades.GetState())
			{
				if (cClass->m_ClassID == (int)CSGOClassID::CBaseCSGrenadeProjectile)
					DrawHE(pEntity, cClass);

				if (cClass->m_ClassID == (int)CSGOClassID::CMolotovProjectile)
					DrawMolotov(pEntity, cClass);

				if (cClass->m_ClassID == (int)CSGOClassID::CDecoyProjectile)
					DrawDecoy(pEntity, cClass);

				if (cClass->m_ClassID == (int)CSGOClassID::CSensorGrenadeProjectile)
					DrawMolotov(pEntity, cClass);

				if (cClass->m_ClassID == (int)CSGOClassID::CSmokeGrenadeProjectile)
					DrawSmoke(pEntity, cClass);
			}
		}
	}
}





//  Yeah m8
void CEsp::DrawPlayer(IClientEntity* pEntity, player_info_t pinfo)
{
	ESPBox Box;
	Color Color;

	// Show own team false? well gtfo teammate
	if (Menu::Window.VisualsTab.FiltersEnemiesOnly.GetState() && (pEntity->GetTeamNum() == hackManager.pLocal()->GetTeamNum()))
		return;

	if (GetBox(pEntity, Box))
	{
		Color = GetPlayerColor(pEntity);

		if (Menu::Window.VisualsTab.OptionsBox.GetState())
			DrawBox(Box, Color);

		if (Menu::Window.VisualsTab.OptionsFill.GetState())
			Fill(Box, Color);

		if (Menu::Window.VisualsTab.OptionsName.GetState())
			DrawName(pinfo, Box);

		if (Menu::Window.VisualsTab.OptionsHealth.GetState())
			DrawHealth(pEntity, Box);

		if (Menu::Window.VisualsTab.OptionsHealth.GetState())
			redraw1(pEntity, Box);

		if (Menu::Window.VisualsTab.OptionsInfo.GetState() || Menu::Window.VisualsTab.OptionsWeapon.GetIndex())
			DrawInfo(pEntity, Box);

		if (Menu::Window.VisualsTab.OptionsAimSpot.GetState())
			DrawCross(pEntity);

		if (Menu::Window.VisualsTab.Distanse.GetState())
			DrawDistanse(pEntity, Box);

		if (Menu::Window.VisualsTab.DrawMoney.GetState())
			Ammo(pEntity, pinfo, Box);

		if (Menu::Window.VisualsTab.DrawMoney.GetState())
			Ammo1(pEntity, pinfo, Box);

		if (Menu::Window.VisualsTab.TextHp.GetState())
			TextHp(pEntity, pinfo, Box);

		if (Menu::Window.VisualsTab.Lines.GetState())
			Lines(Box, Color, pEntity);

		if (Menu::Window.VisualsTab.Grenades.GetState())
			DiLight();

		if (Menu::Window.VisualsTab.BulletTrace.GetState())
			BulletTrace(pEntity, Color);

		if (Menu::Window.VisualsTab.OptionsPlant.GetState())
			DefuseWarning(pEntity);

		if (Menu::Window.VisualsTab.Testing2.GetState())
			DrawGrenades(pEntity);


		if (Menu::Window.VisualsTab.lbycheck.GetState())
			lbyup(pEntity, Box);



		if (Menu::Window.VisualsTab.canhit.GetState())
			canhit(pEntity, Box, pinfo);

		if (Menu::Window.VisualsTab.Skeleton.GetState())
			DrawSkeleton(pEntity);


	}
}

CEsp::ESPBox CEsp::GetBOXX(IClientEntity* pEntity)
{
	ESPBox result;
	// Variables
	Vector  vOrigin, min, max, sMin, sMax, sOrigin,
		flb, brt, blb, frt, frb, brb, blt, flt;
	float left, top, right, bottom;

	// Get the locations
	vOrigin = pEntity->GetOrigin();
	min = pEntity->collisionProperty()->GetMins() + vOrigin;
	max = pEntity->collisionProperty()->GetMaxs() + vOrigin;

	// Points of a 3d bounding box
	Vector points[] = { Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z) };

	// Get screen positions
	if (!Render::WorldToScreen(points[3], flb) || !Render::WorldToScreen(points[5], brt)
		|| !Render::WorldToScreen(points[0], blb) || !Render::WorldToScreen(points[4], frt)
		|| !Render::WorldToScreen(points[2], frb) || !Render::WorldToScreen(points[1], brb)
		|| !Render::WorldToScreen(points[6], blt) || !Render::WorldToScreen(points[7], flt))
		return result;

	// Put them in an array (maybe start them off in one later for speed?)
	Vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

	// Init this shit
	left = flb.x;
	top = flb.y;
	right = flb.x;
	bottom = flb.y;

	// Find the bounding corners for our box
	for (int i = 1; i < 8; i++)
	{
		if (left > arr[i].x)
			left = arr[i].x;
		if (bottom < arr[i].y)
			bottom = arr[i].y;
		if (right < arr[i].x)
			right = arr[i].x;
		if (top > arr[i].y)
			top = arr[i].y;
	}

	// Width / height
	result.x = left;
	result.y = top;
	result.w = right - left;
	result.h = bottom - top;

	return result;
}

void CEsp::redraw1(IClientEntity* pEntity, CEsp::ESPBox size)
{
	float h = (size.h);
	float offset = (h / 4.f) + 5;
	float w = h / 64.f;
	float health = pEntity->GetHealth();
	UINT hp = h - (UINT)((h * health) / 100); // Percentage

	int Red = 255 - (health*2.55);
	int Green = health*2.55;


	int Health = pEntity->GetHealth();
	char nameBuffer[512];
	sprintf_s(nameBuffer, "%d", Health);
	if (Health < 100)
	{
		Render::Text(size.x - 15, size.y + hp, Color(255, 255, 255, 255), Render::Fonts::ESP, nameBuffer);

	}

}


void CEsp::DrawMolotov(IClientEntity* pEntity, ClientClass* cClass)
{
	ESPBox Box;

	if (GetBox(pEntity, Box))
	{
		player_info_t pinfo; strcpy_s(pinfo.name, "Fire");
		if (Menu::Window.VisualsTab.Nades.GetState() == 1)
			DrawName(pinfo, Box);
	}
}

void CEsp::DrawSmoke(IClientEntity* pEntity, ClientClass* cClass)
{
	ESPBox Box;

	if (GetBox(pEntity, Box))
	{
		player_info_t pinfo; strcpy_s(pinfo.name, "Smoke");
		if (Menu::Window.VisualsTab.Nades.GetState() == 1)
			DrawName(pinfo, Box);
	}
}

void CEsp::DrawDecoy(IClientEntity* pEntity, ClientClass* cClass)
{
	ESPBox Box;

	if (GetBox(pEntity, Box))
	{
		player_info_t pinfo; strcpy_s(pinfo.name, "Decoy");
		if (Menu::Window.VisualsTab.Nades.GetState() == 1)
			DrawName(pinfo, Box);
	}
}

void CEsp::DrawHE(IClientEntity* pEntity, ClientClass* cClass)
{
	ESPBox Box;

	if (GetBox(pEntity, Box))
	{
		player_info_t pinfo; strcpy_s(pinfo.name, "HE or Flash");
		if (Menu::Window.VisualsTab.Nades.GetState() == 1)
			DrawName(pinfo, Box);
	}
}




void DiLight()
{
	IClientEntity *pLocal = hackManager.pLocal();
	IClientEntity *pEntity = hackManager.pLocal();
	CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());
	const float TIMEALIVE = 5.f;
	const float GRENADE_COEFFICIENT_OF_RESTITUTION = 0.4f;

	float fStep = 0.1f;
	float fGravity = 800.0f / 8.f;

	Vector vPos;
	Vector vStart;
	Vector vThrow;
	Vector vThrow2;
	int iCollisions = 0;

	Vector vViewAngles;
	Interfaces::Engine->GetViewAngles(vViewAngles);

	vThrow[0] = vViewAngles[0];
	vThrow[1] = vViewAngles[1];
	vThrow[2] = vViewAngles[2];

	if (vThrow[0] < 0)
		vThrow[0] = -10 + vThrow[0] * ((90 - 10) / 90.0);
	else
		vThrow[0] = -10 + vThrow[0] * ((90 + 10) / 90.0);

	float fVel = (90 - vThrow[0]) * 4;
	if (fVel > 500)
		fVel = 500;

	AngleVectors(vThrow, &vThrow2);

	Vector vEye = pEntity->GetEyePosition();
	vStart[0] = vEye[0] + vThrow2[0] * 16;
	vStart[1] = vEye[1] + vThrow2[1] * 16;
	vStart[2] = vEye[2] + vThrow2[2] * 16;

	vThrow2[0] = (vThrow2[0] * fVel) + pEntity->GetVelocity()[0];
	vThrow2[1] = (vThrow2[1] * fVel) + pEntity->GetVelocity()[1];
	vThrow2[2] = (vThrow2[2] * fVel) + pEntity->GetVelocity()[2]; // casualhacker for da magic calc help

	for (float fTime = 0.0f; fTime < TIMEALIVE; fTime += fStep)
	{
		vPos = vStart + vThrow2 * fStep;

		Ray_t ray;
		trace_t tr;

		CTraceFilter loc;
		loc.pSkip = pEntity;

		ray.Init(vStart, vPos);
		Interfaces::Trace->TraceRay(ray, MASK_SOLID, &loc, &tr);

		if (tr.DidHit()) // if the trace hits a surface
		{
			//float proj = DotProduct(throwvec, tr.plane.normal);
			vThrow2 = tr.plane.normal * -2.0f *DotProduct(vThrow2, tr.plane.normal) + vThrow2;
			vThrow2 *= GRENADE_COEFFICIENT_OF_RESTITUTION;

			iCollisions++;
			if (iCollisions > 2)
				break;

			vPos = vStart + vThrow2*tr.fraction*fStep;

			fTime += (fStep * (1 - tr.fraction));
		} // end of bounce

		Vector vOutStart, vOutEnd;
		if (GameUtils::IsG(pWeapon))
		{
			if (Render::WorldToScreen(vStart, vOutStart), Render::WorldToScreen(vPos, vOutEnd))
			{
				Render::Line(vOutStart.x, vOutStart.y, vOutEnd.x, vOutEnd.y, Color(0, 150, 255, 255));
			}

			vStart = vPos;
			vThrow2.z -= fGravity*tr.fraction*fStep;
		}
	}
}
 

void CEsp::DrawMoney(IClientEntity* pEntity, player_info_t pinfo, CEsp::ESPBox size)
{
	int a = pEntity->GetMoney();
	int radix = 10;
	char buffer[20];
	char *p;
	p = itoa(a, buffer, radix);
	RECT nameSize = Render::GetTextSize(Render::Fonts::ESP, pinfo.name);
	Render::Text(size.x + (size.w / 2) - (nameSize.right / 2), size.y - 28,
		Color(255, 215, 0, 255), Render::Fonts::ESP, p );
}


void CEsp::Ammo(IClientEntity* pEntity, player_info_t pinfo, CEsp::ESPBox size)
{

	//IClientEntity *pLocal = hackManager.pLocal();

	CBaseCombatWeapon* CSWeapon = (CBaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pEntity->GetActiveWeaponHandle());

	int a = CSWeapon->GetAmmoInClip();
	int radix = 10;
	char buffer[20];
	char *p;

	p = itoa(a, buffer, radix);
	RECT nameSize = Render::GetTextSize(Render::Fonts::ESP, pinfo.name );
	Render::Text(size.x + size.w + 3, size.y + (0.3*(nameSize.bottom + 95)),
		Color(255, 255, 255, 255), Render::Fonts::ESP,p);
}

void CEsp::Ammo1(IClientEntity* pEntity, player_info_t pinfo, CEsp::ESPBox size)
{

	//IClientEntity *pLocal = hackManager.pLocal();


	RECT nameSize = Render::GetTextSize(Render::Fonts::ESP, "");
	Render::Text(size.x + 10 + size.w + 3, size.y + (0.3*(nameSize.bottom + 95)),
		Color(255, 255, 255, 255), Render::Fonts::ESP, "bullets");
}


void CEsp::DrawGrenades(IClientEntity* pEntity)
{
	Vector vGrenadePos2D = Vector(0.f, 0.f, 0.f);
	Vector vGrenadePos3D = Vector(0.f, 0.f, 0.f);
	float fGrenadeModelSize = 0.0f;
	Color colGrenadeColor = Color(0, 0, 0, 0);
	const model_t *model = pEntity->GetModel();

	if (pEntity->GetClientClass())
	{
		if (model)
		{
			studiohdr_t* hdr = Interfaces::ModelInfo->GetStudiomodel(pEntity->GetModel());
			if (hdr)
			{
				std::string hdrName = hdr->name;
				if (hdrName.find("thrown") != std::string::npos)
				{
					vGrenadePos3D = pEntity->GetOrigin();
					fGrenadeModelSize = hdr->hull_max.DistTo(hdr->hull_min);

					if (!Render::WorldToScreen(vGrenadePos3D, vGrenadePos2D))
						return;

					if (hdrName.find("flash") != std::string::npos)
					{
						//Warning color
						colGrenadeColor = Color(0, 0, 255, 255);
					}
					else if (hdrName.find("incendiarygrenade") != std::string::npos || hdrName.find("molotov") != std::string::npos || hdrName.find("fraggrenade") != std::string::npos)
					{
						//Dangerous color
						colGrenadeColor = Color(255, 0, 0, 255);
					}

					else if (hdrName.find("smoke") != std::string::npos || hdrName.find("decoy") != std::string::npos)
					{
						//Peaceful color
						colGrenadeColor = Color(0, 255, 0, 255);
					}

					Render::Outline(static_cast<int>(vGrenadePos2D.x), static_cast<int>(vGrenadePos2D.y), static_cast<int>(fGrenadeModelSize), 600, colGrenadeColor);
				}
			}
		}
	}
}

void CEsp::Lines(CEsp::ESPBox size, Color color, IClientEntity* pEntity)
{
	Vector src3D, src;
	src3D = pEntity->GetOrigin() - Vector(0, 0, 0);

	if (!Render::WorldToScreen(src3D, src))
		return;

	int ScreenWidth, ScreenHeight;
	Interfaces::Engine->GetScreenSize(ScreenWidth, ScreenHeight);

	int x = (int)(ScreenWidth * 0.5f);
	int y = 0;


	y = ScreenHeight;

	Render::Line((int)(src.x), (int)(src.y), x, y, Color(0, 255, 0, 255));
}


void CEsp::BulletTrace(IClientEntity* pEntity, Color color)
{
	Vector src3D, dst3D, forward, src, dst;
	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;

	AngleVectors(pEntity->GetEyeAngles(), &forward);
	filter.pSkip = pEntity;
	src3D = pEntity->GetBonePos(6) - Vector(0, 0, 0);
	dst3D = src3D + (forward * Menu::Window.VisualsTab.BulletTraceLength.GetValue());

	ray.Init(src3D, dst3D);

	Interfaces::Trace->TraceRay(ray, MASK_SHOT, &filter, &tr);

	if (!Render::WorldToScreen(src3D, src) || !Render::WorldToScreen(tr.endpos, dst))
		return;



	Render::DrawLine(src.x, src.y, dst.x, dst.y, Color(255, 255, 255, 225));
	Render::DrawRect(dst.x - 3, dst.y - 3, 6, 6, Color(255, 255, 255, 255));
};

// Gets the 2D bounding box for the entity
// Returns false on failure nigga don't fail me
bool CEsp::GetBox(IClientEntity* pEntity, CEsp::ESPBox &result)
{
	// Variables
	Vector  vOrigin, min, max, sMin, sMax, sOrigin,
		flb, brt, blb, frt, frb, brb, blt, flt;
	float left, top, right, bottom;

	// Get the locations
	vOrigin = pEntity->GetOrigin();
	min = pEntity->collisionProperty()->GetMins() + vOrigin;
	max = pEntity->collisionProperty()->GetMaxs() + vOrigin;

	// Points of a 3d bounding box
	Vector points[] = { Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z) };

	// Get screen positions
	if (!Render::WorldToScreen(points[3], flb) || !Render::WorldToScreen(points[5], brt)
		|| !Render::WorldToScreen(points[0], blb) || !Render::WorldToScreen(points[4], frt)
		|| !Render::WorldToScreen(points[2], frb) || !Render::WorldToScreen(points[1], brb)
		|| !Render::WorldToScreen(points[6], blt) || !Render::WorldToScreen(points[7], flt))
		return false;

	// Put them in an array (maybe start them off in one later for speed?)
	Vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

	// Init this shit
	left = flb.x;
	top = flb.y;
	right = flb.x;
	bottom = flb.y;

	// Find the bounding corners for our box
	for (int i = 1; i < 8; i++)
	{
		if (left > arr[i].x)
			left = arr[i].x;
		if (bottom < arr[i].y)
			bottom = arr[i].y;
		if (right < arr[i].x)
			right = arr[i].x;
		if (top > arr[i].y)
			top = arr[i].y;
	}

	// Width / height
	result.x = left;
	result.y = top;
	result.w = right - left;
	result.h = bottom - top;

	return true;
}

// Get an entities color depending on team and vis ect
Color CEsp::GetPlayerColor(IClientEntity* pEntity)
{
	int TeamNum = pEntity->GetTeamNum();
	bool IsVis = GameUtils::IsVisible(hackManager.pLocal(), pEntity, (int)CSGOHitboxID::Head);

	Color color;

	if (TeamNum == TEAM_CS_T)
	{
		if (IsVis)
			color = Color(255, 165, 0, 255);
		else
			color = Color(255, 165, 0, 255);
	}
	else
	{
		if (IsVis)
			color = Color(65, 105, 225, 255);
		else
			color = Color(65, 105, 225, 255);
	}


	return color;
}

// 2D  Esp box
void CEsp::DrawBox(CEsp::ESPBox size, Color color)
{
	Render::Outline(size.x, size.y, size.w, size.h, color);
	Render::Outline(size.x - 1, size.y - 1, size.w + 2, size.h + 2, Color(0, 0, 0, 255));
	Render::Outline(size.x + 1, size.y + 1, size.w - 2, size.h - 2, Color(0, 0, 0, 255));

}

void CEsp::Fill(CEsp::ESPBox size, Color color)
{
	if (Menu::Window.VisualsTab.OptionsBox.GetState())
	{
		Render::Clear(size.x, size.y, size.w, size.h, Color(20, 20, 20, 120));
	}
	else {

	}
}//+



// Unicode Conversions
static wchar_t* CharToWideChar(const char* text)
{
	size_t size = strlen(text) + 1;
	wchar_t* wa = new wchar_t[size];
	mbstowcs_s(NULL, wa, size / 4, text, size);
	return wa;
}

void CEsp::DrawName(player_info_t pinfo, CEsp::ESPBox size)
{
	RECT nameSize = Render::GetTextSize(Render::Fonts::ESP, pinfo.name);
	Render::Text(size.x + (size.w / 2) - (nameSize.right / 2), size.y - 16,
		Color(255, 255, 255, 255), Render::Fonts::ESP, pinfo.name);
}


// Draw a health bar. For Tf2 when a bar is bigger than max health a second bar is displayed
void CEsp::DrawHealth(IClientEntity* pEntity, CEsp::ESPBox size)
{
	ESPBox BOX = GetBOXX(pEntity);
	int Line_Size = (BOX.y - BOX.h) / 8;
	int Line_Size2 = (BOX.y + BOX.h) / 8;
	int Health = pEntity->GetHealth();
	if (Health > 100)
		Health = 100;
	int healthG = Health * 2.55;
	int healthR = 255 - healthG;
	Interfaces::Surface->DrawSetColor(0, 0, 0, 150);
	Interfaces::Surface->DrawOutlinedRect(BOX.x - 5, BOX.y - 1, BOX.x - 1, BOX.y + BOX.h + 1);
	//Interfaces::Surface->DrawOutlinedRect(BOX.x + BOX.w + 1, BOX.y - 1, BOX.x + BOX.w + 5, BOX.y + BOX.h + 1);

	int hpBarH = Health * BOX.h / 100;

	if (Health > 0) {
		Interfaces::Surface->DrawSetColor(healthR, healthG, 0, 255);
		Interfaces::Surface->DrawFilledRect(BOX.x - 4,
			BOX.y + BOX.h - hpBarH,
			BOX.x - 2,
			BOX.y + BOX.h);
	}

}

void CEsp::TextHp(IClientEntity* pEntity, player_info_t pinfo, CEsp::ESPBox size)
{
	int d = pEntity->GetHealth();
	int radix = 10;
	char buffer[20];
	char *g;
	int i = 0;
	g = itoa(d, buffer, radix);
	RECT nameSizes = Render::GetTextSize(Render::Fonts::ESP, pinfo.name);
	Render::Text(size.x + size.w + 3, size.y + (i*(nameSizes.bottom + 2)),
		Color(255, 255, 255, 255), Render::Fonts::ESP, g);
}

void CEsp::DrawDistanse(IClientEntity* pEntity, CEsp::ESPBox size)
{
	IClientEntity *pLocal = hackManager.pLocal();

	Vector vecOrigin = pEntity->GetOrigin();
	Vector vecOriginLocal = pLocal->GetOrigin();
	static RECT defSize = Render::GetTextSize(Render::Fonts::Default, "");

	char dist_to[32];
	sprintf_s(dist_to, "%.0f FT", DistanceTo(vecOrigin, vecOriginLocal));

	Render::Text(size.x - 12 + (size.w / 2) - (defSize.right / 1), size.y + size.h + 12, Color(255, 255, 255, 255), Render::Fonts::ESP, dist_to);
}


// Cleans the internal class name up to something human readable and nice
std::string CleanItemName(std::string name)
{
	std::string Name = name;
	// Tidy up the weapon Name
	if (Name[0] == 'C')
		Name.erase(Name.begin());

	// Remove the word Weapon
	auto startOfWeap = Name.find("Weapon");
	if (startOfWeap != std::string::npos)
		Name.erase(Name.begin() + startOfWeap, Name.begin() + startOfWeap + 6);

	return Name;
}

// Anything else: weapons, class state? idk
void CEsp::DrawInfo(IClientEntity* pEntity, CEsp::ESPBox size)
{
	std::vector<std::string> Info;

	std::vector<std::string> Info2;

	std::vector<std::string> Info3;

	// Player Weapon ESP
	IClientEntity* pWeapon = Interfaces::EntList->GetClientEntityFromHandle((HANDLE)pEntity->GetActiveWeaponHandle());
	if (Menu::Window.VisualsTab.OptionsWeapon.GetIndex() && pWeapon)
	{
		ClientClass* cClass = (ClientClass*)pWeapon->GetClientClass();
		if (cClass)
		{
			std::string meme = CleanItemName(cClass->m_pNetworkName);
			RECT nameSize = Render::GetTextSize(Render::Fonts::ESP, meme.c_str());
			if (Menu::Window.VisualsTab.OptionsWeapon.GetIndex() == 1) {
				Render::Text(size.x + (size.w / 2) - (nameSize.right / 2), size.y + size.h + 1,
					Color(255, 255, 255, 255), Render::Fonts::ESP, meme.c_str());
			}
		}
	}

	// Bomb Carrier

	// Comp Rank
	if (Menu::Window.VisualsTab.OptionsCompRank.GetState())
	{
		int CompRank = GameUtils::GetPlayerCompRank(pEntity);
		static const char *Ranks[] =
		{
			"-",
			"Silver I",
			"Silver II",
			"Silver III",
			"Silver IV",
			"Silver Elite",
			"Silver Elite Master",

			"Gold Nova I",
			"Gold Nova II",
			"Gold Nova III",
			"Gold Nova Master",
			"Master Guardian I",
			"Master Guardian II",

			"Master Guardian Elite",
			"Distinguished Master Guardian",
			"Legendary Eagle",
			"Legendary Eagle Master",
			"Supreme Master First Class",
			"Global Elite"
		};
		if (CompRank >= 0 && CompRank <= 18)
			Info.push_back(Ranks[CompRank]);
	}
	if (Menu::Window.VisualsTab.OptionsInfo.GetState() && pEntity->HasHelmet())
	{
		RECT nameSize = Render::GetTextSize(Render::Fonts::ESP, "");
		Render::Text(size.x + size.w + 3, size.y + (0.3*(nameSize.bottom + 15)),
			Color(255, 255, 255, 255), Render::Fonts::ESP, "HK");
	}

	if (Menu::Window.VisualsTab.OptionsInfo.GetState() && pEntity->IsScoped())
	{
		RECT nameSize = Render::GetTextSize(Render::Fonts::ESP, "");
		Render::Text(size.x + size.w + 3, size.y + (0.3*(nameSize.bottom + 45)),
			Color(0, 51, 102, 255), Render::Fonts::ESP, "ZOOM");
	}

	// Bomb Carrier
	if (Menu::Window.VisualsTab.OptionsInfo.GetState() && pEntity == BombCarrier)
	{
		RECT nameSize = Render::GetTextSize(Render::Fonts::ESP, "");
		Render::Text(size.x + size.w + 3, size.y + (0.3*(nameSize.bottom + 75)),
			Color(153, 0, 0, 255), Render::Fonts::ESP, "B");

	}

}

void CEsp::canhit(IClientEntity* pEntity, CEsp::ESPBox size, player_info_t pinfo)
{
	IClientEntity* pLocal = hackManager.pLocal();
	std::vector<int> HitBoxesToScan;

	// Get the hitboxes to scan
#pragma region GetHitboxesToScan
	int HitScanMode = Menu::Window.RageBotTab.TargetHitscan.GetIndex();
	int iSmart = Menu::Window.RageBotTab.AccuracySmart.GetValue();
	bool AWall = Menu::Window.RageBotTab.AccuracyAutoWall.GetState();
	bool Multipoint = Menu::Window.RageBotTab.TargetMultipoint.GetState();

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
	for (auto HitBoxID : HitBoxesToScan)
	{
		if (!pEntity || pEntity->IsDormant() || !pEntity->IsAlive())
			continue;


		if (pEntity->GetTeamNum() == pLocal->GetTeamNum() || !pLocal->IsAlive())
			continue;

		if (AWall)
		{
			Vector Point = GetHitboxPosition(pEntity, HitBoxID);
			float Damage = 0.f;
			Color c = Color(255, 255, 255, 255);
			if (CanHit(Point, &Damage))
			{
				c = Color(0, 255, 0, 255);
				//45 first number
				RECT nameSize = Render::GetTextSize(Render::Fonts::ESP, "");
				Render::Text(size.x + size.w + 3, size.y + (0.3*(nameSize.bottom + 105)),
					Color(255, 0, 0, 255), Render::Fonts::ESP, "CAN HIT");
			}
		}
	}

	if (autowalldmgtest[pEntity->GetIndex()] != 0) {

		char asasaasasas[64];
		RECT nameSize = Render::GetTextSize(Render::Fonts::ESP, pinfo.name);
		sprintf_s(asasaasasas, "DAMAGE:%f", autowalldmgtest[pEntity->GetIndex()]);
		Render::Text(size.x + size.w + 3, size.y + (0.3*(nameSize.bottom + 135)), Color(255, 0, 0, 255), Render::Fonts::ESP, asasaasasas);
	}


	return;
}

// Little cross on their heads
void CEsp::DrawCross(IClientEntity* pEntity)
{
	Vector cross = pEntity->GetHeadPos(), screen;
	static int Scale = 2;
	if (Render::WorldToScreen(cross, screen))
	{
		Render::Clear(screen.x - Scale, screen.y - (Scale * 2), (Scale * 2), (Scale * 4), Color(20, 20, 20, 160));
		Render::Clear(screen.x - (Scale * 2), screen.y - Scale, (Scale * 4), (Scale * 2), Color(20, 20, 20, 160));
		Render::Clear(screen.x - Scale - 1, screen.y - (Scale * 2) - 1, (Scale * 2) - 2, (Scale * 4) - 2, Color(0, 0, 0, 250));
		Render::Clear(screen.x - (Scale * 2) - 1, screen.y - Scale - 1, (Scale * 4) - 2, (Scale * 2) - 2, Color(0, 0, 0, 250));
	}
}

void CEsp::DefuseWarning(IClientEntity* pEntity)
{
		if (pEntity->IsDefusing())
			Render::Text(10, 100, Color(255, 0, 0, 255), Render::Fonts::ICON, ("Enemy is defusing"));
		else
			Render::Text(10, 100, Color(0, 255, 0, 255), Render::Fonts::ICON, (""));
}



// Draws a dropped CS:GO Item
void CEsp::DrawDrop(IClientEntity* pEntity, ClientClass* cClass)
{
	Vector Box;
	CBaseCombatWeapon* Weapon = (CBaseCombatWeapon*)pEntity;
	IClientEntity* plr = Interfaces::EntList->GetClientEntityFromHandle((HANDLE)Weapon->GetOwnerHandle());
	if (!plr && Render::WorldToScreen(Weapon->GetOrigin(), Box))
	{
		if (Menu::Window.VisualsTab.OptionsBox.GetState())
		{
			Render::Outline(Box.x - 2, Box.y - 2, 4, 4, Color(255, 255, 255, 255));
			Render::Outline(Box.x - 3, Box.y - 3, 6, 6, Color(10, 10, 10, 150));
		}

		if (Menu::Window.VisualsTab.OptionsInfo.GetState())
		{
			std::string ItemName = CleanItemName(cClass->m_pNetworkName);
			RECT TextSize = Render::GetTextSize(Render::Fonts::ESP, ItemName.c_str());
			Render::Text(Box.x - (TextSize.right / 10), Box.y - 30, Color(255, 255, 255, 255), Render::Fonts::ESP, ItemName.c_str());
		}
	}
}

// Draws a chicken
void CEsp::DrawChicken(IClientEntity* pEntity, ClientClass* cClass)
{
	ESPBox Box;

	if (GetBox(pEntity, Box))
	{
		player_info_t pinfo; strcpy_s(pinfo.name, "Chicken");
		if (Menu::Window.VisualsTab.OptionsBox.GetState())
			DrawBox(Box, Color(255, 255, 255, 255));

		if (Menu::Window.VisualsTab.OptionsName.GetState())
			DrawName(pinfo, Box);
	}
}

// Draw the planted bomb and timer
void CEsp::DrawBombPlanted(IClientEntity* pEntity, ClientClass* cClass)
{
	// Null it out incase bomb has been dropped or planted
	BombCarrier = nullptr;

	Vector vOrig; Vector vScreen;
	vOrig = pEntity->GetOrigin();
	CCSBomb* Bomb = (CCSBomb*)pEntity;

	if (Render::WorldToScreen(vOrig, vScreen))
	{
		float flBlow = Bomb->GetC4BlowTime();
		float TimeRemaining = flBlow - (Interfaces::Globals->interval_per_tick * hackManager.pLocal()->GetTickBase());
		char buffer[64];
		sprintf_s(buffer, "[ Planted C4 ]", TimeRemaining);
		Render::Text(vScreen.x, vScreen.y, Color(255, 255, 255, 255), Render::Fonts::ESP, buffer);
	}
}

void CEsp::DrawBombPlanted2(IClientEntity* pEntity, ClientClass* cClass)
{
	// Null it out incase bomb has been dropped or planted
	BombCarrier = nullptr;

	Vector vOrig; Vector vScreen;
	vOrig = pEntity->GetOrigin();
	CCSBomb* Bomb = (CCSBomb*)pEntity;

		float flBlow = Bomb->GetC4BlowTime();
		float TimeRemaining = flBlow - (Interfaces::Globals->interval_per_tick * hackManager.pLocal()->GetTickBase());
		char buffer[64];
		sprintf_s(buffer, "%.1fs", TimeRemaining);
		Render::Text(10, 10, Color(0, 255, 0, 255), Render::Fonts::ICON, buffer);

}



void CEsp::lbyup(IClientEntity* pEntity, CEsp::ESPBox size)
{
	CUserCmd* cmdlist = *(CUserCmd**)((DWORD)Interfaces::pInput + 0xEC);
	CUserCmd* pCmd = cmdlist;
	IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	for (int i = 0; i < Interfaces::Engine->GetMaxClients(); ++i)
	{
		IClientEntity *pEntity = Interfaces::EntList->GetClientEntity(i);

		if (!pEntity || pEntity->IsDormant() || !pEntity->IsAlive())
			continue;


		if (pEntity->GetTeamNum() == pLocal->GetTeamNum() || !pLocal->IsAlive())
			continue;

		if (pCmd->viewangles.y - *pEntity->GetLowerBodyYawTarget() >= -35 && pCmd->viewangles.y - *pEntity->GetLowerBodyYawTarget() <= 35)
		{
			RECT defSize = Render::GetTextSize(Render::Fonts::ESP, "");
			Render::Text(size.x + size.w + 3, size.y + (0.3*(defSize.bottom + 15)),
				Color(255, 255, 255, 255), Render::Fonts::ESP, "FAKE");
		}
		else
		{
			RECT defSize = Render::GetTextSize(Render::Fonts::ESP, "");
			Render::Text(size.x + size.w + 3, size.y + (0.3*(defSize.bottom + 75)),
				Color(255, 255, 255, 255), Render::Fonts::ESP, "REAL");
		}
	}
}




// Draw the bomb if it's dropped, or store the player who's carrying 
void CEsp::DrawBomb(IClientEntity* pEntity, ClientClass* cClass)
{
	// Null it out incase bomb has been dropped or planted
	BombCarrier = nullptr;
	CBaseCombatWeapon *BombWeapon = (CBaseCombatWeapon *)pEntity;
	Vector vOrig; Vector vScreen;
	vOrig = pEntity->GetOrigin();
	bool adopted = true;
	HANDLE parent = BombWeapon->GetOwnerHandle();
	if (parent || (vOrig.x == 0 && vOrig.y == 0 && vOrig.z == 0))
	{
		IClientEntity* pParentEnt = (Interfaces::EntList->GetClientEntityFromHandle(parent));
		if (pParentEnt && pParentEnt->IsAlive())
		{
			BombCarrier = pParentEnt;
			adopted = false;
		}
	}

	if (adopted)
	{
		if (Render::WorldToScreen(vOrig, vScreen))
		{
			Render::Text(vScreen.x, vScreen.y, Color(255, 255, 255, 255), Render::Fonts::ESP, "[ C4 ]");
		}
	}
}

void DrawBoneArray(int* boneNumbers, int amount, IClientEntity* pEntity, Color color)
{
	Vector LastBoneScreen;
	for (int i = 0; i < amount; i++)
	{
		Vector Bone = pEntity->GetBonePos(boneNumbers[i]);
		Vector BoneScreen;

		if (Render::WorldToScreen(Bone, BoneScreen))
		{
			if (i>0)
			{
				Render::Line(LastBoneScreen.x, LastBoneScreen.y, BoneScreen.x, BoneScreen.y, color);
			}
		}
		LastBoneScreen = BoneScreen;
	}
}


void DrawBoneTest(IClientEntity *pEntity)
{
	for (int i = 0; i < 127; i++)
	{
		Vector BoneLoc = pEntity->GetBonePos(i);
		Vector BoneScreen;
		if (Render::WorldToScreen(BoneLoc, BoneScreen))
		{
			char buf[10];
			_itoa_s(i, buf, 10);
			Render::Text(BoneScreen.x, BoneScreen.y, Color(255, 255, 255, 180), Render::Fonts::ESP, buf);
		}
	}
}

void CEsp::DrawSkeleton(IClientEntity* pEntity)
{
	studiohdr_t* pStudioHdr = Interfaces::ModelInfo->GetStudiomodel(pEntity->GetModel());

	if (!pStudioHdr)
		return;

	Vector vParent, vChild, sParent, sChild;

	for (int j = 0; j < pStudioHdr->numbones; j++)
	{
		mstudiobone_t* pBone = pStudioHdr->GetBone(j);

		if (pBone && (pBone->flags & BONE_USED_BY_HITBOX) && (pBone->parent != -1))
		{
			vChild = pEntity->GetBonePos(j);
			vParent = pEntity->GetBonePos(pBone->parent);

			if (Render::WorldToScreen(vParent, sParent) && Render::WorldToScreen(vChild, sChild))
			{
				Render::Line(sParent[0], sParent[1], sChild[0], sChild[1], Color(255, 255, 255, 255));
			}
		}
	}
}