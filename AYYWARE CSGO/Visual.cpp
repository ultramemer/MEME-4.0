/*
Syn's AyyWare Framework 2015
*/

#include "Visuals.h"
#include "Interfaces.h"
#include "RenderManager.h"

// Any init here
void CVisuals::Init()
{
	// Idk
}

// Don't really need to do anything in here
void CVisuals::Move(CUserCmd *pCmd, bool &bSendPacket) {}

// Main ESP Drawing loop
void CVisuals::Draw()
{
	
}



// Draw a basic crosshair

// Recoil crosshair
void CVisuals::DrawRecoilCrosshair()
{
	if (Menu::Window.RageBotTab.AccuracyRecoil.GetState())
		return;

	IClientEntity *pLocal = hackManager.pLocal();

	// Get the view with the recoil
	Vector ViewAngles;
	Interfaces::Engine->GetViewAngles(ViewAngles);
	ViewAngles += pLocal->localPlayerExclusive()->GetAimPunchAngle() * 2.f;

	// Build a ray going fowards at that angle
	Vector fowardVec;
	AngleVectors(ViewAngles, &fowardVec);
	fowardVec *= 10000;

	// Get ray start / end
	Vector start = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector end = start + fowardVec, endScreen;

	if (Render::WorldToScreen(end, endScreen) && pLocal->IsAlive())
	{
		static int Scale = 2;
		RECT View = Render::GetViewport();
		int MidX = View.right / 2;
		int MidY = View.bottom / 2;
		Render::Clear(MidX - Scale, MidY - (Scale * 2), (Scale * 2), (Scale * 4), Color(220, 220, 220, 160));
		Render::Clear(MidX - (Scale * 2), MidY - Scale, (Scale * 4), (Scale * 2), Color(220, 220, 220, 160));
		Render::Clear(MidX - Scale - 1, MidY - (Scale * 2) - 1, (Scale * 2) - 2, (Scale * 4) - 2, Color(220, 220, 220, 160));
		Render::Clear(MidX - (Scale * 2) - 1, MidY - Scale - 1, (Scale * 4) - 2, (Scale * 2) - 2, Color(220, 220, 220, 160));
	}
}