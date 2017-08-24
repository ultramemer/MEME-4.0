/*
Syn's AyyWare Framework 2015
*/

#pragma once

#include "Hacks.h"
extern int ResolverMode;
extern float enemyLBYTimer[65];
extern float enemysLastProxyTimer[65];
extern float enemyLBYDelta[65];
extern float lineLBY;
extern float lineRealAngle;
extern float lineFakeAngle;
extern int ResolverStage[65];
extern float autowalldmgtest[65];

class CEsp : public CHack
{
public:
	void Init();
	void Draw();
	void Move(CUserCmd *pCmd, bool& bSendPacket);
private:

	// Other shit
	IClientEntity *BombCarrier;

	struct ESPBox
	{
		int x, y, w, h;
	};

	// Draw a player


	void DrawPlayer(IClientEntity * pEntity, player_info_t pinfo);

	CEsp::ESPBox GetBOXX(IClientEntity * pEntity);

	void redraw1(IClientEntity * pEntity, CEsp::ESPBox size);

	void DrawMolotov(IClientEntity * pEntity, ClientClass * cClass);

	void DrawSmoke(IClientEntity * pEntity, ClientClass * cClass);

	void DrawDecoy(IClientEntity * pEntity, ClientClass * cClass);

	void DrawHE(IClientEntity * pEntity, ClientClass * cClass);

	void DrawMoney(IClientEntity * pEntity, player_info_t pinfo, CEsp::ESPBox size);

	void Ammo(IClientEntity * pEntity, player_info_t pinfo, CEsp::ESPBox size);

	void Ammo1(IClientEntity * pEntity, player_info_t pinfo, CEsp::ESPBox size);

	void lbyup2(IClientEntity * pEntity, Color color);

	void DrawGrenades(IClientEntity * pEntity);

	void DrawGrenades(IClientEntity * pEntity, CEsp::ESPBox size, Color color);

	void Lines(CEsp::ESPBox size, Color color, IClientEntity * pEntity);

	void BulletTrace(IClientEntity * pEntity, Color color);

	// Get player info
	Color GetPlayerColor(IClientEntity* pEntity);
	bool GetBox(IClientEntity* pEntity, ESPBox &result);

	// Draw shit about player
	void DrawBox(ESPBox size, Color color);
	void Fill(CEsp::ESPBox size, Color color);
	void DrawName(player_info_t pinfo, CEsp::ESPBox size);
	void DrawHealth(IClientEntity* pEntity, ESPBox size);
	void TextHp(IClientEntity * pEntity, player_info_t pinfo, CEsp::ESPBox size);
	void LbyAngles(IClientEntity * pEntity, player_info_t pinfo, CEsp::ESPBox size);
	void DrawDistanse(IClientEntity * pEntity, CEsp::ESPBox size);
	void DrawInfo(IClientEntity* pEntity, ESPBox size);
	void DrawCross(IClientEntity* pEntity);
	void DefuseWarning(IClientEntity * pEntity);
	void DrawSkeleton(IClientEntity* pEntity);
	void DrawArmor(IClientEntity* pEntity, ESPBox size);

	void DrawChicken(IClientEntity* pEntity, ClientClass* cClass);
	void DrawDrop(IClientEntity* pEntity, ClientClass* cClass);
	void DrawBombPlanted(IClientEntity* pEntity, ClientClass* cClass);
	void DrawBombPlanted2(IClientEntity * pEntity, ClientClass * cClass);
	void canhit(IClientEntity * pEntity, CEsp::ESPBox size, player_info_t pinfo);
	void lbyup(IClientEntity * pEntity, CEsp::ESPBox size);
	void resolvertype(IClientEntity * pEntity, CEsp::ESPBox size);
	void DrawBomb(IClientEntity* pEntity, ClientClass* cClass);
};

void DiLight();
