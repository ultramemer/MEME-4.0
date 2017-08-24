/*
Syn's AyyWare Framework 2015
*/

#include "Menu.h"
#include "Controls.h"
#include "Hooks.h" // for the unload meme
#include "Interfaces.h"
#include "CRC32.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 700

AyyWareWindow Menu::Window;


void Unloadbk()
{
	DoUnload = true;
}


void SaveCallbk()
{
	switch (Menu::Window.SettingsTab.SetNr.GetIndex())
	{
	case 0:
		GUI.SaveWindowState(&Menu::Window, "legit.xml");
		break;
	case 1:
		GUI.SaveWindowState(&Menu::Window, "Legit-2.xml");
		break;
	case 2:
		GUI.SaveWindowState(&Menu::Window, "rage.xml");
		break;
	case 3:
		GUI.SaveWindowState(&Menu::Window, "HVH-1.xml");
		break;
	case 4:
		GUI.SaveWindowState(&Menu::Window, "HVH-2.xml");
		break;
	}

}
void LoadCallbk()
{
	switch (Menu::Window.SettingsTab.SetNr.GetIndex())
	{
	case 0:
		GUI.LoadWindowState(&Menu::Window, "legit.xml");
		break;
	case 1:
		GUI.LoadWindowState(&Menu::Window, "legit-2.xml");
		break;
	case 2:
		GUI.LoadWindowState(&Menu::Window, "rage.xml");
		break;
	case 3:
		GUI.LoadWindowState(&Menu::Window, "HVH-1.xml");
		break;
	case 4:
		GUI.LoadWindowState(&Menu::Window, "HVH-2.xml");
		break;
	}
}



void AyyWareWindow::Setup()
{
	SetPosition(350, 50);
	SetSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	SetTitle("");

	RegisterTab(&RageBotTab);
	//RegisterTab(&LegitBotTab);
	RegisterTab(&VisualsTab);
	RegisterTab(&MiscTab);
	//RegisterTab(&SettingsTab);
	//RegisterTab(&GlovesChanger);
	//RegisterTab(&SkinchangerTab);

	RECT Client = GetClientArea();
	Client.bottom -= 29;

	RageBotTab.Setup();
	//LegitBotTab.Setup();
	VisualsTab.Setup();
	MiscTab.Setup();
	//SettingsTab.Setup();
	//GlovesChanger.Setup();
	//SkinchangerTab.Setup();

}


void CRageBotTab::Setup()
{
	SetTitle("RAGE");

	//ActiveLabel.SetPosition(250, -14);
	//ActiveLabel.SetText("Active");
	//RegisterControl(&ActiveLabel);

	Active.SetFileId("active");
	Active.SetPosition(170, -32);
	RegisterControl(&Active);

#pragma region Aimbot

	AimbotGroup.SetPosition(176, -16);
	AimbotGroup.SetText("Aimbot");
	AimbotGroup.SetSize(270, 295);
	RegisterControl(&AimbotGroup);

	AimbotEnable.SetFileId("aim_enable");
	AimbotGroup.PlaceLabledControl("Enable", this, &AimbotEnable);

	AimbotAutoFire.SetFileId("aim_autofire");
	AimbotGroup.PlaceLabledControl("Auto Fire", this, &AimbotAutoFire);

	AimbotFov.SetFileId("aim_fov");
	AimbotFov.SetBoundaries(0.f, 180.f);
	AimbotFov.SetValue(39.f);
	AimbotGroup.PlaceLabledControl("FOV Range", this, &AimbotFov);

	AimbotSilentAim.SetFileId("aim_silent");
	AimbotGroup.PlaceLabledControl("Silent Aim", this, &AimbotSilentAim);

	AimbotPerfectSilentAim.SetFileId("aim_psilent");
	AimbotGroup.PlaceLabledControl("Perfect Silent", this, &AimbotPerfectSilentAim);

	AimbotAutoPistol.SetFileId("aim_autopistol");
	AimbotGroup.PlaceLabledControl("Auto Pistol", this, &AimbotAutoPistol);

	AimbotAimStep.SetFileId("aim_aimstep");
	AimbotGroup.PlaceLabledControl("Aim Step", this, &AimbotAimStep);

	AimbotKeyPress.SetFileId("aim_usekey");
	AimbotGroup.PlaceLabledControl("On Key Press", this, &AimbotKeyPress);

	AimbotKeyBind.SetFileId("aim_key");
	AimbotGroup.PlaceLabledControl("Key", this, &AimbotKeyBind);

	AimbotStopKey.SetFileId("aim_stop");
	AimbotGroup.PlaceLabledControl("Stop Aim", this, &AimbotStopKey);

	AutoRevoler.SetFileId("aim_AutoRevoler");
	AimbotGroup.PlaceLabledControl("Auto Revolver", this, &AutoRevoler);
#pragma endregion Aimbot Controls Get Setup in here

#pragma region Target
	TargetGroup.SetPosition(176, 278);
	TargetGroup.SetText("Target");
	TargetGroup.SetSize(270, 295);
	RegisterControl(&TargetGroup);

	TargetSelection.SetFileId("tgt_selection");
	TargetSelection.AddItem("Closest To Crosshair");
	TargetSelection.AddItem("Distance");
	TargetSelection.AddItem("Lowest Health");
	TargetGroup.PlaceLabledControl("Selection", this, &TargetSelection);

	TargetFriendlyFire.SetFileId("tgt_friendlyfire");
	TargetGroup.PlaceLabledControl("Friendly Fire", this, &TargetFriendlyFire);

	TargetHitbox.SetFileId("tgt_hitbox");
	TargetHitbox.AddItem("Head");
	TargetHitbox.AddItem("Neck");
	TargetHitbox.AddItem("Chest");
	TargetHitbox.AddItem("Stomach");
	TargetHitbox.AddItem("Shins");
	TargetGroup.PlaceLabledControl("Hitbox", this, &TargetHitbox);

	TargetHitscan.SetFileId("tgt_hitscan");
	TargetHitscan.AddItem("Off"); //0
	TargetHitscan.AddItem("Low"); // 1
	TargetHitscan.AddItem("Medium"); // 2
	TargetHitscan.AddItem("High"); // 3
	TargetHitscan.AddItem("Extreme"); // 4
	TargetGroup.PlaceLabledControl("Hitscan", this, &TargetHitscan);

	TargetMultipoint.SetFileId("tgt_multipoint");
	TargetGroup.PlaceLabledControl("Multipoint", this, &TargetMultipoint);

	TargetPointscale.SetFileId("tgt_pointscale");
	TargetPointscale.SetBoundaries(0.f, 10.f);
	TargetPointscale.SetValue(5.f);
	TargetGroup.PlaceLabledControl("Aim Height", this, &TargetPointscale);

	Resolver.SetFileId("tgt_Resolver");
	TargetGroup.PlaceLabledControl("Resolver test", this, &Resolver);

	Sinister.SetFileId("tgt_Resolver");
	TargetGroup.PlaceLabledControl("Anti LBY", this, &Sinister);

	AccuracyBrute.SetFileId("acc_brute");
	AccuracyBrute.AddItem("Off");
	AccuracyBrute.AddItem("ANTILBY1");
	AccuracyBrute.AddItem("ANTILBY2");
	AccuracyBrute.AddItem("ANTILBY3");
	AccuracyBrute.AddItem("ANTILBY4");
	AccuracyBrute.AddItem("ANTILBY5");
	AccuracyBrute.AddItem("ANTILBY6");
	TargetGroup.PlaceLabledControl("Resolver - lby", this, &AccuracyBrute);

#pragma endregion Targetting controls 

#pragma region Accuracy
	AccuracyGroup.SetPosition(560, 250);
	AccuracyGroup.SetText("Accuracy");
	AccuracyGroup.SetSize(270, 260); //280
	RegisterControl(&AccuracyGroup);

	AccuracyRecoil.SetFileId("acc_norecoil");
	AccuracyGroup.PlaceLabledControl("Anti Recoil", this, &AccuracyRecoil);

	AccuracyAutoWall.SetFileId("acc_awall");
	AccuracyGroup.PlaceLabledControl("Auto Wall", this, &AccuracyAutoWall);

	AccuracyMinimumDamage.SetFileId("acc_mindmg");
	AccuracyMinimumDamage.SetBoundaries(1.f, 99.f);
	AccuracyMinimumDamage.SetValue(1.f);
	AccuracyGroup.PlaceLabledControl("Autowall Damage", this, &AccuracyMinimumDamage);

	AccuracyAutoStop.SetFileId("acc_stop");
	AccuracyGroup.PlaceLabledControl("Auto Stop / Crouch", this, &AccuracyAutoStop);

	AccuracyAutoScope.SetFileId("acc_scope");
	AccuracyGroup.PlaceLabledControl("Auto Scope", this, &AccuracyAutoScope);

	AccuracyHitchance.SetFileId("acc_chance");
	AccuracyHitchance.SetBoundaries(0, 100);
	AccuracyHitchance.SetValue(0);
	AccuracyGroup.PlaceLabledControl("Hit Chance", this, &AccuracyHitchance);
	
	AccuracyResolverYaw.SetFileId("acc_aaa");
	AccuracyResolverYaw.AddItem("Off");
	AccuracyResolverYaw.AddItem("On");
	AccuracyGroup.PlaceLabledControl("Resolver", this, &AccuracyResolverYaw);

	AccuracyPositionAdjustment.SetFileId("acc_posadj");
	AccuracyGroup.PlaceLabledControl("LagFix", this, &AccuracyPositionAdjustment);

	AccuracyPosition.SetFileId("acc_posadj");
	AccuracyGroup.PlaceLabledControl("Position Adj", this, &AccuracyPosition);

	AccuracySmart.SetFileId("acc_smart");
	AccuracySmart.SetBoundaries(0, 20);
	AccuracySmart.SetValue(1);
	AccuracyGroup.PlaceLabledControl("Smart Aim", this, &AccuracySmart);

	AccuracyPrediction.SetFileId("acc_predi");
	AccuracyGroup.PlaceLabledControl("Backtrack", this, &AccuracyPrediction);
#pragma endregion  Accuracy controls get Setup in here

#pragma region AntiAim
	AntiAimGroup.SetPosition(560, -16); //344
	AntiAimGroup.SetText("Anti Aim");
	AntiAimGroup.SetSize(270, 260);
	RegisterControl(&AntiAimGroup);

	AntiAimEnable.SetFileId("aa_enable");
	AntiAimGroup.PlaceLabledControl("Enable", this, &AntiAimEnable);

	AntiAimPitch.SetFileId("aa_x");
	AntiAimPitch.AddItem("OFF");
	AntiAimPitch.AddItem("Down");
	AntiAimPitch.AddItem("SMAC Safe");
	AntiAimPitch.AddItem("Jitter");
	AntiAimPitch.AddItem("Static");
	AntiAimPitch.AddItem("Fake Down");
	AntiAimPitch.AddItem("Lisp Down");
	AntiAimPitch.AddItem("Lisp Up");
	AntiAimPitch.AddItem("Emotion");
	AntiAimPitch.AddItem("Tank AA");
	AntiAimPitch.AddItem("NASA ANTI AIM 1");
	AntiAimPitch.AddItem("NASA ANTI AIM 2");
	AntiAimPitch.AddItem("ZERO");
	AntiAimPitch.AddItem("NASA ANTI AIM 3");
	AntiAimPitch.AddItem("STONE AA");
	AntiAimPitch.AddItem("FakeOverFlow");
	AntiAimPitch.AddItem("FakeEmotionZero");
	AntiAimGroup.PlaceLabledControl("Pitch", this, &AntiAimPitch);

	AntiAimYaw.SetFileId("aa_y");
	AntiAimYaw.AddItem("OFF");
	AntiAimYaw.AddItem("Fake Edge");
	AntiAimYaw.AddItem("Stone Old");
	AntiAimYaw.AddItem("Fake Static");
	AntiAimYaw.AddItem("T Fake");
	AntiAimYaw.AddItem("Fake Jitter");
	AntiAimYaw.AddItem("Jitter");
	AntiAimYaw.AddItem("T Jitter");
	AntiAimYaw.AddItem("Back Jitter");
	AntiAimYaw.AddItem("Backwards");
	AntiAimYaw.AddItem("Fake Lowerbody");
	AntiAimYaw.AddItem("Spin Slow");
	AntiAimYaw.AddItem("LBYJITTER");
	AntiAimYaw.AddItem("LBY1");
	AntiAimYaw.AddItem("LBY2");
	AntiAimYaw.AddItem("Tank AA");
	AntiAimYaw.AddItem("FastSpin2");
	AntiAimYaw.AddItem("Flip");
	AntiAimYaw.AddItem("Fake Jitter Synced");
	AntiAimYaw.AddItem("NASA ANTI AIM 1");
	AntiAimYaw.AddItem("Fake Body");
	AntiAimYaw.AddItem("pLisp");
	AntiAimYaw.AddItem("pFakeSide");
	AntiAimYaw.AddItem("Resolve.Me");
	AntiAimYaw.AddItem("Stone Old 2");
	AntiAimGroup.PlaceLabledControl("Yaw", this, &AntiAimYaw);

	FakeYawAA.SetFileId("aa_y");
	FakeYawAA.AddItem("OFF");
	FakeYawAA.AddItem("Sleepy op asf");
	FakeYawAA.AddItem("Backward");
	FakeYawAA.AddItem("Forward");
	FakeYawAA.AddItem("Sideways Left");
	FakeYawAA.AddItem("Sideways Right");
	FakeYawAA.AddItem("Spin Slow");
	FakeYawAA.AddItem("Spin Fast");
	FakeYawAA.AddItem("Jitter Backward");
	FakeYawAA.AddItem("Jitter Forward");
	FakeYawAA.AddItem("Jitter Sideways");
	FakeYawAA.AddItem("Zero");
	FakeYawAA.AddItem("Lower Body");
	FakeYawAA.AddItem("Fake Lower Body");
	AntiAimGroup.PlaceLabledControl("Fake Yaw", this, &FakeYawAA);

	//zAA.SetFileId("aa_Z");
	//zAA.AddItem("OFF");
	//zAA.AddItem("NASA ANTI AIM 1");
	//zAA.AddItem("NASA ANTI AIM 2");
	//AntiAimGroup.PlaceOtherControl("angle.z", this, &zAA);

	AntiAimEdge.SetFileId("aa_edge");
	AntiAimEdge.AddItem("Off");
	AntiAimEdge.AddItem("Normal");
	AntiAimGroup.PlaceLabledControl("Wall Detect", this, &AntiAimEdge);

	AntiAimOffset.SetFileId("aa_offset");
	AntiAimOffset.SetBoundaries(0, 360);
	AntiAimOffset.SetValue(0);
	AntiAimGroup.PlaceLabledControl("Yaw Offset", this, &AntiAimOffset);

	AntiAimKnife.SetFileId("aa_knife");
	AntiAimGroup.PlaceLabledControl("Anti Aim on Knife", this, &AntiAimKnife);

	AntiAimTarget.SetFileId("aa_target");
	AntiAimGroup.PlaceLabledControl("Anti Aim At Target", this, &AntiAimTarget);

	EdgeAntiAim.SetFileId("aa_EdgeAntiAim");
	AntiAimGroup.PlaceLabledControl("Edge Anti Aim", this, &EdgeAntiAim);

	LByFIX.SetFileId("aa_fixed");
	AntiAimGroup.PlaceLabledControl("LBY FIX RESOLVER", this, &LByFIX);


#pragma endregion  AntiAim controls get setup in here
}

/*void CLegitBotTab::Setup()
{
	SetTitle("b");

	ActiveLabel.SetPosition(16, -14);
	ActiveLabel.SetText("Active");
	RegisterControl(&ActiveLabel);

	Active.SetFileId("active");
	Active.SetPosition(66, -14);
	RegisterControl(&Active);

#pragma region Aimbot
	AimbotGroup.SetPosition(16, 16);
	AimbotGroup.SetText("Aimbot");
	AimbotGroup.SetSize(240, 210);
	RegisterControl(&AimbotGroup);

	AimbotEnable.SetFileId("aim_enable");
	AimbotGroup.PlaceLabledControl("Enable", this, &AimbotEnable);

	AimbotAutoFire.SetFileId("aim_autofire");
	AimbotGroup.PlaceLabledControl("Auto Fire", this, &AimbotAutoFire);

	//AimbotFriendlyFire.SetFileId("aim_friendfire");
	//AimbotGroup.PlaceLabledControl("Friendly Fire", this, &AimbotFriendlyFire);

	AimbotKeyPress.SetFileId("aim_usekey");
	AimbotGroup.PlaceLabledControl("On Key Press", this, &AimbotKeyPress);

	AimbotKeyBind.SetFileId("aim_key");
	AimbotGroup.PlaceLabledControl("Key Bind", this, &AimbotKeyBind);
	
	AimbotAutoPistol.SetFileId("aim_apistol");
	AimbotGroup.PlaceLabledControl("Auto Pistol", this, &AimbotAutoPistol);

#pragma endregion Aimbot shit

	#pragma region Triggerbot
	TriggerGroup.SetPosition(272, 16);
	TriggerGroup.SetText("Triggerbot");
	TriggerGroup.SetSize(240, 210);
	RegisterControl(&TriggerGroup);

	TriggerEnable.SetFileId("trig_enable");
	TriggerGroup.PlaceLabledControl("Enable", this, &TriggerEnable);

	TriggerKeyPress.SetFileId("trig_onkey");
	TriggerGroup.PlaceLabledControl("On Key Press", this, &TriggerKeyPress);

	TriggerKeyBind.SetFileId("trig_key");
	TriggerGroup.PlaceLabledControl("Key Bind", this, &TriggerKeyBind);

	TriggerDelay.SetFileId("trig_time");
	TriggerDelay.SetBoundaries(0.f, 1000.f);
	TriggerGroup.PlaceLabledControl("Delay (ms)", this, &TriggerDelay);
#pragma endregion Triggerbot stuff

		WeaponMainGroup.SetPosition(16, 250);
	WeaponMainGroup.SetText("Rifles");
	WeaponMainGroup.SetSize(240, 175);
	RegisterControl(&WeaponMainGroup);

	WeaponMainHitbox.SetFileId("main_hitbox");
	WeaponMainHitbox.AddItem("Head");
	WeaponMainHitbox.AddItem("Neck");
	WeaponMainHitbox.AddItem("Chest");
	WeaponMainHitbox.AddItem("Stomach");
	WeaponMainHitbox.AddItem("Multihitbox");
	WeaponMainGroup.PlaceLabledControl("Hitbox", this, &WeaponMainHitbox);

	WeaponMainSpeed.SetFileId("main_speed");
	WeaponMainSpeed.SetBoundaries(0.f, 100.f);
	WeaponMainSpeed.SetValue(1.f);
	WeaponMainGroup.PlaceLabledControl("Max speed", this, &WeaponMainSpeed);

	WeaponMainFoV.SetFileId("main_fov");
	WeaponMainFoV.SetBoundaries(0.0f, 30.0f);
	WeaponMainFoV.SetValue(3.0f);
	WeaponMainGroup.PlaceLabledControl("FOV", this, &WeaponMainFoV);

	WeaponMainRecoil.SetFileId("main_recoil");
	WeaponMainRecoil.SetBoundaries(0.f, 2.f);
	WeaponMainRecoil.SetValue(1.f);
	WeaponMainGroup.PlaceLabledControl("Recoil", this, &WeaponMainRecoil);

	WeaponMainAimtime.SetFileId("main_aimtime");
	WeaponMainAimtime.SetBoundaries(0, 3);
	WeaponMainAimtime.SetValue(0);
	WeaponMainGroup.PlaceLabledControl("Aim Time", this, &WeaponMainAimtime);

	WeaoponMainStartAimtime.SetFileId("main_aimstart");
	WeaoponMainStartAimtime.SetBoundaries(0, 5);
	WeaoponMainStartAimtime.SetValue(0);
	WeaponMainGroup.PlaceLabledControl("Start Aim Time", this, &WeaoponMainStartAimtime);
	#pragma endregion

	#pragma region Pistols
	WeaponPistGroup.SetPosition(272, 250);
	WeaponPistGroup.SetText("Pistols");
	WeaponPistGroup.SetSize(240, 175);
	RegisterControl(&WeaponPistGroup);

	WeaponPistHitbox.SetFileId("pist_hitbox");
	WeaponPistHitbox.AddItem("Head");
	WeaponPistHitbox.AddItem("Neck");
	WeaponPistHitbox.AddItem("Chest");
	WeaponPistHitbox.AddItem("Stomach");
	WeaponPistHitbox.AddItem("Multihitbox");
	WeaponPistGroup.PlaceLabledControl("Hitbox", this, &WeaponPistHitbox);

	WeaponPistSpeed.SetFileId("pist_speed");
	WeaponPistSpeed.SetBoundaries(0.f, 100.f);
	WeaponPistSpeed.SetValue(1.0f);
	WeaponPistGroup.PlaceLabledControl("Max Speed", this, &WeaponPistSpeed);

	WeaponPistFoV.SetFileId("pist_fov");
	WeaponPistFoV.SetBoundaries(0.0f, 30.0f);
	WeaponPistFoV.SetValue(3.0f);
	WeaponPistGroup.PlaceLabledControl("FOV", this, &WeaponPistFoV);

	WeaponPistRecoil.SetFileId("pist_recoil");
	WeaponPistRecoil.SetBoundaries(0.f, 2.f);
	WeaponPistRecoil.SetValue(1.f);
	WeaponPistGroup.PlaceLabledControl("Recoil", this, &WeaponPistRecoil);

	WeaponPistAimtime.SetFileId("pist_aimtime");
	WeaponPistAimtime.SetBoundaries(0, 3);
	WeaponPistAimtime.SetValue(0);
	WeaponPistGroup.PlaceLabledControl("Aim Time", this, &WeaponPistAimtime);

	WeaoponPistStartAimtime.SetFileId("pist_aimstart");
	WeaoponPistStartAimtime.SetBoundaries(0, 5);
	WeaoponPistStartAimtime.SetValue(0);
	WeaponPistGroup.PlaceLabledControl("Start Aim Time", this, &WeaoponPistStartAimtime);
	#pragma endregion

	#pragma region Snipers
	WeaponSnipGroup.SetPosition(528, 250);
	WeaponSnipGroup.SetText("Snipers");
	WeaponSnipGroup.SetSize(240, 175);
	RegisterControl(&WeaponSnipGroup);

	WeaponSnipHitbox.SetFileId("snip_hitbox");
	WeaponSnipHitbox.AddItem("Head");
	WeaponSnipHitbox.AddItem("Neck");
	WeaponSnipHitbox.AddItem("Chest");
	WeaponSnipHitbox.AddItem("Stomach");
	WeaponSnipHitbox.AddItem("Multihitbox");
	WeaponSnipGroup.PlaceLabledControl("Hitbox", this, &WeaponSnipHitbox);

	WeaponSnipSpeed.SetFileId("snip_speed");
	WeaponSnipSpeed.SetBoundaries(0.f, 100.f);
	WeaponSnipSpeed.SetValue(1.5f);
	WeaponSnipGroup.PlaceLabledControl("Max Speed", this, &WeaponSnipSpeed);

	WeaponSnipFoV.SetFileId("snip_fov");
	WeaponSnipFoV.SetBoundaries(0.0f, 30.0f);
	WeaponSnipFoV.SetValue(2.0f);
	WeaponSnipGroup.PlaceLabledControl("FOV", this, &WeaponSnipFoV);

	WeaponSnipRecoil.SetFileId("snip_recoil");
	WeaponSnipRecoil.SetBoundaries(0.f, 2.f);
	WeaponSnipRecoil.SetValue(1.f);
	WeaponSnipGroup.PlaceLabledControl("Recoil", this, &WeaponSnipRecoil);

	WeaponSnipAimtime.SetFileId("snip_aimtime");
	WeaponSnipAimtime.SetBoundaries(0, 3);
	WeaponSnipAimtime.SetValue(0);
	WeaponSnipGroup.PlaceLabledControl("Aim Time", this, &WeaponSnipAimtime);

	WeaoponSnipStartAimtime.SetFileId("pist_aimstart");
	WeaoponSnipStartAimtime.SetBoundaries(0, 5);
	WeaoponSnipStartAimtime.SetValue(0);
	WeaponSnipGroup.PlaceLabledControl("Start Aim Time", this, &WeaoponSnipStartAimtime);
	#pragma endregion
	*/
	/*#pragma region MPs
	WeaponMpGroup.SetPosition(16, 458);
	WeaponMpGroup.SetText("MPs");
	WeaponMpGroup.SetSize(240, 176);
	RegisterControl(&WeaponMpGroup);

	WeaponMpHitbox.SetFileId("mps_hitbox");
	WeaponMpHitbox.AddItem("Head");
	WeaponMpHitbox.AddItem("Neck");
	WeaponMpHitbox.AddItem("Chest");
	WeaponMpHitbox.AddItem("Stomach");
	WeaponMpHitbox.AddItem("Multihitbox");
	WeaponMpGroup.PlaceLabledControl("Hitbox", this, &WeaponMpHitbox);

	WeaponMpSpeed.SetFileId("mps_speed");
	WeaponMpSpeed.SetBoundaries(0.f, 100.f);
	WeaponMpSpeed.SetValue(1.0f);
	WeaponMpGroup.PlaceLabledControl("Max Speed", this, &WeaponMpSpeed);

	WeaponMpFoV.SetFileId("mps_fov");
	WeaponMpFoV.SetBoundaries(0.0f, 30.0f);
	WeaponMpFoV.SetValue(4.0f);
	WeaponMpGroup.PlaceLabledControl("FOV", this, &WeaponMpFoV);

	WeaponMpRecoil.SetFileId("mps_recoil");
	WeaponMpRecoil.SetBoundaries(0.f, 2.f);
	WeaponMpRecoil.SetValue(1.f);
	WeaponMpGroup.PlaceLabledControl("Recoil", this, &WeaponMpRecoil);

	WeaponMpAimtime.SetFileId("mps_aimtime");
	WeaponMpAimtime.SetBoundaries(0, 3);
	WeaponMpAimtime.SetValue(0);
	WeaponMpGroup.PlaceLabledControl("Aim Time", this, &WeaponMpAimtime);

	WeaoponMpStartAimtime.SetFileId("mps_aimstart");
	WeaoponMpStartAimtime.SetBoundaries(0, 5);
	WeaoponMpStartAimtime.SetValue(0);
	WeaponMpGroup.PlaceLabledControl("Start Aim Time", this, &WeaoponMpStartAimtime);
	#pragma endregion

	#pragma region Shotguns
	WeaponShotgunGroup.SetPosition(272, 458);
	WeaponShotgunGroup.SetText("Shotguns");
	WeaponShotgunGroup.SetSize(240, 176);
	RegisterControl(&WeaponShotgunGroup);

	WeaponShotgunHitbox.SetFileId("shotgun_hitbox");
	WeaponShotgunHitbox.AddItem("Head");
	WeaponShotgunHitbox.AddItem("Neck");
	WeaponShotgunHitbox.AddItem("Chest");
	WeaponShotgunHitbox.AddItem("Stomach");
	WeaponShotgunHitbox.AddItem("Multihitbox");
	WeaponShotgunGroup.PlaceLabledControl("Hitbox", this, &WeaponShotgunHitbox);

	WeaponShotgunSpeed.SetFileId("shotgun_speed");
	WeaponShotgunSpeed.SetBoundaries(0.f, 100.f);
	WeaponShotgunSpeed.SetValue(1.0f);
	WeaponShotgunGroup.PlaceLabledControl("Max Speed", this, &WeaponShotgunSpeed);

	WeaponShotgunFoV.SetFileId("shotgun_fov");
	WeaponShotgunFoV.SetBoundaries(0.0f, 30.0f);
	WeaponShotgunFoV.SetValue(3.0f);
	WeaponShotgunGroup.PlaceLabledControl("FOV", this, &WeaponShotgunFoV);

	WeaponShotgunRecoil.SetFileId("shotgun_recoil");
	WeaponShotgunRecoil.SetBoundaries(0.f, 2.f);
	WeaponShotgunRecoil.SetValue(1.f);
	WeaponShotgunGroup.PlaceLabledControl("Recoil", this, &WeaponShotgunRecoil);

	WeaponShotgunAimtime.SetFileId("shotgun_aimtime");
	WeaponShotgunAimtime.SetBoundaries(0, 3);
	WeaponShotgunAimtime.SetValue(0);
	WeaponShotgunGroup.PlaceLabledControl("Aim Time", this, &WeaponShotgunAimtime);

	WeaoponShotgunStartAimtime.SetFileId("shotgun_aimstart");
	WeaoponShotgunStartAimtime.SetBoundaries(0, 5);
	WeaoponShotgunStartAimtime.SetValue(0);
	WeaponShotgunGroup.PlaceLabledControl("Start Aim Time", this, &WeaoponShotgunStartAimtime);
	#pragma endregion

	#pragma region Machineguns
	WeaponMGGroup.SetPosition(528, 458);
	WeaponMGGroup.SetText("Machineguns");
	WeaponMGGroup.SetSize(240, 176);
	RegisterControl(&WeaponMGGroup);

	WeaponMGHitbox.SetFileId("mg_hitbox");
	WeaponMGHitbox.AddItem("Head");
	WeaponMGHitbox.AddItem("Neck");
	WeaponMGHitbox.AddItem("Chest");
	WeaponMGHitbox.AddItem("Stomach");
	WeaponMGHitbox.AddItem("Multihitbox");
	WeaponMGGroup.PlaceLabledControl("Hitbox", this, &WeaponMGHitbox);

	WeaponMGSpeed.SetFileId("mg_speed");
	WeaponMGSpeed.SetBoundaries(0.f, 100.f);
	WeaponMGSpeed.SetValue(1.0f);
	WeaponMGGroup.PlaceLabledControl("Max Speed", this, &WeaponMGSpeed);
	WeaponMGFoV.SetFileId("mg_fov");
	WeaponMGFoV.SetBoundaries(0.0f, 30.0f);
	WeaponMGFoV.SetValue(4.0f);
	WeaponMGGroup.PlaceLabledControl("FOV", this, &WeaponMGFoV);

	WeaponMGRecoil.SetFileId("mg_recoil");
	WeaponMGRecoil.SetBoundaries(0.f, 2.f);
	WeaponMGRecoil.SetValue(1.f);
	WeaponMGGroup.PlaceLabledControl("Recoil", this, &WeaponMGRecoil);

	WeaponMGAimtime.SetFileId("mg_aimtime");
	WeaponMGAimtime.SetBoundaries(0, 3);
	WeaponMGAimtime.SetValue(0);
	WeaponMGGroup.PlaceLabledControl("Aim Time", this, &WeaponMGAimtime);

	WeaoponMGStartAimtime.SetFileId("mg_aimstart");
	WeaoponMGStartAimtime.SetBoundaries(0, 5);
	WeaoponMGStartAimtime.SetValue(0);
	WeaponMGGroup.PlaceLabledControl("Start Aim Time", this, &WeaoponMGStartAimtime)*/;
//}

void CVisualTab::Setup()
{
	SetTitle("VISUALS");

	//ActiveLabel.SetPosition(100, -14);
	//ActiveLabel.SetText("Active");
	//RegisterControl(&ActiveLabel);

	Active.SetFileId("active");
	Active.SetPosition(170, -32);
	RegisterControl(&Active);

#pragma region Options
	OptionsGroup.SetText("ESP");
	OptionsGroup.SetPosition(176, -16);
	OptionsGroup.SetSize(185, 430);
	RegisterControl(&OptionsGroup);

	OptionsBox.SetFileId("opt_box");
	OptionsGroup.PlaceLabledControl("Box", this, &OptionsBox);


	Skeleton.SetFileId("opt_box");
	OptionsGroup.PlaceLabledControl("Skeleton", this, &Skeleton);

	OptionsHealth.SetFileId("opt_hp");
	OptionsGroup.PlaceLabledControl("Health ESP", this, &OptionsHealth);

	DrawMoney.SetFileId("otr_backtomexico");
	OptionsGroup.PlaceLabledControl("Ammo ESP", this, &DrawMoney);

	OptionsWeapon.SetFileId("otr_recoilhair");
	OptionsWeapon.AddItem("off");
	OptionsWeapon.AddItem("Text");
	OptionsGroup.PlaceLabledControl("Weapon ESP", this, &OptionsWeapon);

	DrawMoney.SetFileId("otr_backtomexico");
	OptionsGroup.PlaceLabledControl("Ammo ESP", this, &DrawMoney);

	Distanse.SetFileId("opt_Distaa");
	OptionsGroup.PlaceLabledControl("Distanse ESP", this, &Distanse);

	OptionsInfo.SetFileId("opt_info");
	OptionsGroup.PlaceLabledControl("Flags ESP & world ESP", this, &OptionsInfo);

	OptionsPlant.SetFileId("opt_bone");
	OptionsGroup.PlaceLabledControl("Enable bomb & timer ESP", this, &OptionsPlant);

	selfesp.SetFileId("ftr_enemyonly");
	OptionsGroup.PlaceLabledControl("Self esp", this, &selfesp);

	FiltersEnemiesOnly.SetFileId("ftr_enemyonly");
	OptionsGroup.PlaceLabledControl("Enemies Only", this, &FiltersEnemiesOnly);

#pragma endregion Setting up the Options controls
#pragma region Other
	OtherGroup.SetText("Other");
	OtherGroup.SetPosition(392, -16);
	OtherGroup.SetSize(334, 430);
	RegisterControl(&OtherGroup);

	OtherCrosshair.SetFileId("otr_xhair");
	OtherGroup.PlaceLabledControl("Crosshair", this, &OtherCrosshair);

	OtherNoVisualRecoil.SetFileId("otr_visrecoil");
	OtherGroup.PlaceLabledControl("No Visual Recoil", this, &OtherNoVisualRecoil);

	OtherNoHands.SetFileId("otr_hands");
	OtherNoHands.AddItem("Off");
	OtherNoHands.AddItem("None");
	OtherNoHands.AddItem("Black Hands");
	OtherNoHands.AddItem("Chams Hands");
	OtherNoHands.AddItem("Rainbow Hands");
	OtherGroup.PlaceLabledControl("Hands", this, &OtherNoHands);

	OtherViewmodelFOV.SetFileId("otr_viewfov");
	OtherViewmodelFOV.SetBoundaries(0.f, 180.f);
	OtherViewmodelFOV.SetValue(0.f);
	OtherGroup.PlaceLabledControl("Viewmodel FOV Changer", this, &OtherViewmodelFOV);

	OtherFOV.SetFileId("otr_fov");
	OtherFOV.SetBoundaries(0.f, 180.f);
	OtherFOV.SetValue(90.f);
	OtherGroup.PlaceLabledControl("Field of View Changer", this, &OtherFOV);

	Lines.SetFileId("otr_line");
	OtherGroup.PlaceLabledControl("Snap lines", this, &Lines);

	OtherHitmarker.SetFileId("otr_backtomexico");
	OtherGroup.PlaceLabledControl("Hitmarker", this, &OtherHitmarker);

	lbycheck.SetFileId("otr_backtomexico");
	OtherGroup.PlaceLabledControl("lby check", this, &lbycheck);

	canhit.SetFileId("otr_backtomexico");
	OtherGroup.PlaceLabledControl("canhit check", this, &canhit);

	resolvetype.SetFileId("otr_backtomexico");
	OtherGroup.PlaceLabledControl("Resolver type", this, &resolvetype);

	Nades.SetFileId("otr_backtomexico");
	OtherGroup.PlaceLabledControl("Nade esp", this, &Nades);

	Grenades.SetFileId("otr_backtomexico");
	OtherGroup.PlaceLabledControl("Grenade Prediction", this, &Grenades);

	BulletTrace.SetFileId("otp_bulletrace");
	OptionsGroup.PlaceLabledControl("Line of slight", this, &BulletTrace);

	BulletTraceLength.SetFileId("otp_bulletracerange");
	OptionsGroup.PlaceLabledControl("", this, &BulletTraceLength);
	BulletTraceLength.SetBoundaries(0.f, 1000.f);
	BulletTraceLength.SetValue(200.f);

#pragma endregion Setting up the Other controls
}

void CMiscTab::Setup()
{
	SetTitle("MISC");

/*#pragma region Knife
	KnifeGroup.SetPosition(16, 16);
	KnifeGroup.SetSize(360, 126);
	KnifeGroup.SetText("Knife Changer");
	RegisterControl(&KnifeGroup);

	KnifeEnable.SetFileId("knife_enable");
	KnifeGroup.PlaceLabledControl("Enable", this, &KnifeEnable);

	KnifeModel.SetFileId("knife_model");
	KnifeModel.AddItem("KARAMBIT");
	KnifeModel.AddItem("BAYONET ");
	KnifeModel.AddItem("M9 BAYONET ");
	KnifeModel.AddItem("FLIP ");
	KnifeGroup.PlaceLabledControl("Knife", this, &KnifeModel);

	KnifeSkin.SetFileId("knife_skin");
	KnifeSkin.AddItem("Doppler Sapphire");
	KnifeSkin.AddItem("Doppler Ruby");
	KnifeSkin.AddItem("Tiger");
	KnifeSkin.AddItem("Lore");
	KnifeSkin.AddItem("Marble Fade");
	KnifeSkin.AddItem("Crimson Web");
	KnifeSkin.AddItem("Slaughter");
	KnifeSkin.AddItem("Fade");
	KnifeSkin.AddItem("Night");
	KnifeSkin.AddItem("Autotronic");
	KnifeSkin.AddItem("Bright water");
	KnifeSkin.AddItem("Gamma Doppler Emerald");
	KnifeGroup.PlaceLabledControl("Skin", this, &KnifeSkin);

	KnifeApply.SetText("Apply Knife");
	KnifeApply.SetCallback(KnifeApplyCallbk);
	KnifeGroup.PlaceLabledControl("", this, &KnifeApply
#pragma endregion	*/

#pragma region Other
	OtherGroup.SetPosition(540, -16);
	OtherGroup.SetSize(360, 340);
	OtherGroup.SetText("Other");
	RegisterControl(&OtherGroup);

	OtherAutoJump.SetFileId("otr_autojump");
	OtherAutoJump.AddItem("Off");
	OtherAutoJump.AddItem("ON");
	OtherGroup.PlaceLabledControl("Bunnyhop", this, &OtherAutoJump);

	OtherEdgeJump.SetFileId("otr_edgejump");
	OtherGroup.PlaceLabledControl("Edge Jump", this, &OtherEdgeJump);

	OtherAutoStrafe.SetFileId("otr_strafe");
	OtherAutoStrafe.AddItem("Off");
	OtherAutoStrafe.AddItem("Legit");
	OtherAutoStrafe.AddItem("Rage");
	OtherGroup.PlaceLabledControl("Auto Strafer", this, &OtherAutoStrafe);

	OtherSafeMode.SetFileId("otr_safemode");
	OtherSafeMode.SetState(true);
	OtherGroup.PlaceLabledControl("Anti Untrust", this, &OtherSafeMode);

	// not working right now we should fix it !
	/*OtherChatSpam.SetFileId("otr_spam");
	OtherChatSpam.AddItem("not working fix needed");
	OtherChatSpam.AddItem("1");
	OtherChatSpam.AddItem("2");
	OtherChatSpam.AddItem("3");
	OtherChatSpam.AddItem("4");
	OtherGroup.PlaceOtherControl("Chat Spam", this, &OtherChatSpam);*/

	OtherClantag.SetFileId("otr_spam");
	OtherClantag.AddItem("Off");
	OtherClantag.AddItem("fhook.cc (animated)");
	OtherClantag.AddItem("skeet.cc");
	OtherClantag.AddItem("fhook.cc (normal)");
	OtherClantag.AddItem("valve");
	OtherGroup.PlaceLabledControl("Custom Clantag", this, &OtherClantag);

	OtherTeamChat.SetFileId("otr_teamchat");
	OtherGroup.PlaceLabledControl("Team Chat Only", this, &OtherTeamChat);

	OtherChatDelay.SetFileId("otr_chatdelay");
	OtherChatDelay.SetBoundaries(0.1, 3.0);
	OtherChatDelay.SetValue(0.5);
	OtherGroup.PlaceLabledControl("Spam Delay", this, &OtherChatDelay);

	OtherAirStuck.SetFileId("otr_astuck");
	OtherGroup.PlaceLabledControl("Air Stuck", this, &OtherAirStuck);

	//OtherSpectators.SetFileId("otr_speclist");
	//OtherGroup.PlaceOtherControl("Spectators List", this, &OtherSpectators);

	OtherThirdperson.SetFileId("aa_thirdpsr");
	OtherGroup.PlaceLabledControl("Thirdperson", this, &OtherThirdperson);

	OtherCircleStrafe.SetFileId("otr_circlestrafe");
	OtherGroup.PlaceLabledControl("Circle Strafe", this, &OtherCircleStrafe);

	OtherCircleStrafeKey.SetFileId("otr_circlestrafeKey");
	OtherGroup.PlaceLabledControl("Circle Strafe Key", this, &OtherCircleStrafeKey);

	FakeWalk.SetFileId("otr_circlestrafe");
	OtherGroup.PlaceLabledControl("ShiftWalk", this, &FakeWalk);

#pragma endregion other random options

#pragma region FakeLag
	FakeLagGroup.SetPosition(176, -16);
	FakeLagGroup.SetSize(360, 141);
	FakeLagGroup.SetText("Fake Lag");
	RegisterControl(&FakeLagGroup);

	FakeLagEnable.SetFileId("fakelag_enable");
	FakeLagGroup.PlaceLabledControl("Fake Lag", this, &FakeLagEnable);

	FakeLagChoke.SetFileId("fakelag_choke");
	FakeLagChoke.SetBoundaries(0, 16);
	FakeLagChoke.SetValue(0);
	FakeLagGroup.PlaceLabledControl("Choke Factor", this, &FakeLagChoke);

	FakeLagSend.SetFileId("fakelag_send");
	FakeLagSend.SetBoundaries(0, 16);
	FakeLagSend.SetValue(0);
	FakeLagGroup.PlaceLabledControl("Send Factor", this, &FakeLagSend);

	ChokeRandomize.SetFileId("choke_random");
	FakeLagGroup.PlaceLabledControl("Randomize Choke", this, &ChokeRandomize);

	SendRandomize.SetFileId("send_random");
	FakeLagGroup.PlaceLabledControl("Randomize Send", this, &SendRandomize);
#pragma endregion fakelag shit

/*#pragma region Teleport
	TeleportGroup.SetPosition(13, 160);
	TeleportGroup.SetSize(280, 75);
	TeleportGroup.SetText("Teleport");
	RegisterControl(&TeleportGroup);

	TeleportEnable.SetFileId("teleport_enable");
	TeleportGroup.PlaceOtherControl("Enable", this, &TeleportEnable);

	TeleportKey.SetFileId("teleport_key");
	TeleportGroup.PlaceOtherControl("Key", this, &TeleportKey);

#pragma endregion*/




	//Options

/*#pragma region OverideFov
	FOVGroup.SetPosition(16, 365);
	FOVGroup.SetSize(360, 75);
	FOVGroup.SetText("FOV Changer");
	RegisterControl(&FOVGroup);

	FOVEnable.SetFileId("fov_enable");
	FOVGroup.PlaceLabledControl("Enable", this, &FOVEnable);

	FOVSlider.SetFileId("fov_slider");
	FOVSlider.SetBoundaries(0, 200);
	FOVSlider.SetValue(0);
	FOVGroup.PlaceLabledControl("FOV Amount", this, &FOVSlider);

#pragma endregion*/
}

/*void CPlayersTab::Setup()
{
	SetTitle("PlayerList");

#pragma region PList

	pListGroup.SetPosition(16, 16);
	pListGroup.SetSize(680, 200);
	pListGroup.SetText("Player List");
	pListGroup.SetColumns(2);
	RegisterControl(&pListGroup);

	pListPlayers.SetPosition(26, 46);
	pListPlayers.SetSize(640, 50);
	pListPlayers.SetHeightInItems(20);
	RegisterControl(&pListPlayers);

#pragma endregion

#pragma region Options
	
	OptionsGroup.SetPosition(16, 257);
	OptionsGroup.SetSize(450, 120);
	OptionsGroup.SetText("Player Options");
	RegisterControl(&OptionsGroup);

	OptionsFriendly.SetFileId("pl_friendly");
	OptionsGroup.PlaceLabledControl("Friendly", this, &OptionsFriendly);

	OptionsAimPrio.SetFileId("pl_priority");
	OptionsGroup.PlaceLabledControl("Priority", this, &OptionsAimPrio);

	OptionsCalloutSpam.SetFileId("pl_callout");
	OptionsGroup.PlaceLabledControl("Callout Spam", this, &OptionsCalloutSpam);

#pragma endregion
}

DWORD GetPlayerListIndex(int EntId)
{
	player_info_t pinfo;
	Interfaces::Engine->GetPlayerInfo(EntId, &pinfo);

	// Bot
	if (pinfo.guid[0] == 'B' && pinfo.guid[1] == 'O')
	{
		char buf[64]; sprintf_s(buf, "BOT_420%sAY", pinfo.name);
		return CRC32(buf, 64);
	}
	else
	{
		return CRC32(pinfo.guid, 32);
	}
}

bool IsFriendly(int EntId)
{
	DWORD plistId = GetPlayerListIndex(EntId);
	if (PlayerList.find(plistId) != PlayerList.end())
	{
		return PlayerList[plistId].Friendly;
	}

	return false;
}

bool IsAimPrio(int EntId)
{
	DWORD plistId = GetPlayerListIndex(EntId);
	if (PlayerList.find(plistId) != PlayerList.end())
	{
		return PlayerList[plistId].AimPrio;
	}

	return false;
}

bool IsCalloutTarget(int EntId)
{
	DWORD plistId = GetPlayerListIndex(EntId);
	if (PlayerList.find(plistId) != PlayerList.end())
	{
		return PlayerList[plistId].Callout;
	}

	return false;
}

void UpdatePlayerList()
{
	IClientEntity* pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame() && pLocal && pLocal->IsAlive())
	{
		Menu::Window.Playerlist.pListPlayers.ClearItems();

		// Loop through all active entitys
		for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
		{
			// Get the entity

			player_info_t pinfo;
			if (i != Interfaces::Engine->GetLocalPlayer() && Interfaces::Engine->GetPlayerInfo(i, &pinfo))
			{
				IClientEntity* pEntity = Interfaces::EntList->GetClientEntity(i);
				int HP = 100; char* Location = "Unknown";
				char *Friendly = " ", *AimPrio = " ";

				DWORD plistId = GetPlayerListIndex(Menu::Window.Playerlist.pListPlayers.GetValue());
				if (PlayerList.find(plistId) != PlayerList.end())
				{
					Friendly = PlayerList[plistId].Friendly ? "Friendly" : "";
					AimPrio = PlayerList[plistId].AimPrio ? "AimPrio" : "";
				}

				if (pEntity && !pEntity->IsDormant())
				{
					HP = pEntity->GetHealth();
					Location = pEntity->GetLastPlaceName();
				}

				char nameBuffer[512];
				sprintf_s(nameBuffer, "%-24s %-10s %-10s [%d HP] [Last Seen At %s]", pinfo.name, IsFriendly(i) ? "Friend" : " ", IsAimPrio(i) ? "AimPrio" : " ", HP, Location);
				Menu::Window.Playerlist.pListPlayers.AddItem(nameBuffer, i);

			}

		}

		DWORD meme = GetPlayerListIndex(Menu::Window.Playerlist.pListPlayers.GetValue());

		// Have we switched to a different player?
		static int PrevSelectedPlayer = 0;
		if (PrevSelectedPlayer != Menu::Window.Playerlist.pListPlayers.GetValue())
		{
			if (PlayerList.find(meme) != PlayerList.end())
			{
				Menu::Window.Playerlist.OptionsFriendly.SetState(PlayerList[meme].Friendly);
				Menu::Window.Playerlist.OptionsAimPrio.SetState(PlayerList[meme].AimPrio);
				Menu::Window.Playerlist.OptionsCalloutSpam.SetState(PlayerList[meme].Callout);

			}
			else
			{
				Menu::Window.Playerlist.OptionsFriendly.SetState(false);
				Menu::Window.Playerlist.OptionsAimPrio.SetState(false);
				Menu::Window.Playerlist.OptionsCalloutSpam.SetState(false);

			}
		}
		PrevSelectedPlayer = Menu::Window.Playerlist.pListPlayers.GetValue();

		PlayerList[meme].Friendly = Menu::Window.Playerlist.OptionsFriendly.GetState();
		PlayerList[meme].AimPrio = Menu::Window.Playerlist.OptionsAimPrio.GetState();
		PlayerList[meme].Callout = Menu::Window.Playerlist.OptionsCalloutSpam.GetState();
	}
}*/

 /*void CSettingsTab::Setup()
{

	SetTitle("CONFIG");
#pragma ButtonGroup
	ButtonGroup.SetPosition(150, 16);
	ButtonGroup.SetSize(360, 460);
	ButtonGroup.SetText("Buttons");
	RegisterControl(&ButtonGroup);

	SaveButton.SetText("Save Configuration");
	SaveButton.SetCallback(SaveCallbk);
	ButtonGroup.PlaceOtherControl("Save", this, &SaveButton);

	LoadButton.SetText("Load Configuration");
	LoadButton.SetCallback(LoadCallbk);
	ButtonGroup.PlaceOtherControl("Load", this, &LoadButton);

	//Options


	SetNr.AddItem("Legit");
	SetNr.AddItem("Legit-2");
	SetNr.AddItem("Rage");
	SetNr.AddItem("HVH-1");
	SetNr.AddItem("HVH-2");
	SetNr.AddItem("Disable All");
	ButtonGroup.PlaceOtherControl("Number", this, &SetNr);

	Unload.SetText("Force Crash(time out)");
	Unload.SetCallback(Unloadbk);
	ButtonGroup.PlaceOtherControl("", this, &Unload);
};*/







void Menu::SetupMenu()
{
	Window.Setup();

	GUI.RegisterWindow(&Window);
	GUI.BindWindow(VK_INSERT, &Window);
}

void Menu::DoUIFrame()
{
	// General Processing

	// If the "all filter is selected tick all the others
	if (Window.VisualsTab.FiltersAll.GetState())
	{
		Window.VisualsTab.FiltersC4.SetState(true);
		Window.VisualsTab.FiltersChickens.SetState(true);
		Window.VisualsTab.FiltersPlayers.SetState(true);
		Window.VisualsTab.FiltersWeapons.SetState(true);
	}

	GUI.Update();
	GUI.Draw();

	
}


