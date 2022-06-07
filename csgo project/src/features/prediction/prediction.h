#pragma once
#include "../../commonIncludes.h"

class CPrediction
{
public:
	CPrediction()
	{
		iPredictionRandomSeed = *reinterpret_cast<int**>(memory::PatternScan(CLIENT_DLL, XorStr("8B 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 83 C4 04")) + 0x2); // @xref: "SelectWeightedSequence"
		pSetPredictionEntity = *reinterpret_cast<CBaseEntity**>(memory::PatternScan(CLIENT_DLL, XorStr("89 35 ? ? ? ? F3 0F 10 48 20")) + 0x2);
	}

	void start(CUserCmd* pCmd);
	void end(CUserCmd* pCmd);
	int GetTickbase(CUserCmd* pCmd);
private:
	int* iPredictionRandomSeed = nullptr;
	CBaseEntity* pSetPredictionEntity = nullptr;
	CMoveData moveData = { };

	float flOldCurrentTime = 0.f;
	float flOldFrameTime = 0.f;
	int iOldTickCount = 0;
};

extern CPrediction prediction;