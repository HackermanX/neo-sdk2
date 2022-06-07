#include "prediction.h"

void CPrediction::start(CUserCmd* pCmd)
{
	// @xref: "CPrediction::ProcessMovement"

	if (!g_LocalPlayer->IsAlive() || i::MoveHelper == nullptr)
		return;

	// start command
	g_LocalPlayer->GetCurrentCommand() = pCmd;
	g_LocalPlayer->GetLastCommand() = *pCmd;

	// random_seed isn't generated in ClientMode::CreateMove yet, we must generate it ourselves
	*iPredictionRandomSeed = MD5::PseudoRandom(pCmd->iCommandNumber) & 0x7FFFFFFF;
	// set ourselves as a predictable entity
	pSetPredictionEntity = g_LocalPlayer;

	// backup globals
	flOldCurrentTime = i::Globals->flCurrentTime;
	flOldFrameTime = i::Globals->flFrameTime;
	iOldTickCount = i::Globals->iTickCount;

	// backup tickbase
	const int iOldTickBase = g_LocalPlayer->m_nTickBase();

	// backup prediction states
	const bool bOldIsFirstPrediction = i::Prediction->bIsFirstTimePredicted;
	const bool bOldInPrediction = i::Prediction->bInPrediction;

	// set corrected values
	i::Globals->flCurrentTime = TICKS_TO_TIME(GetTickbase(pCmd));
	i::Globals->flFrameTime = i::Prediction->bEnginePaused ? 0.f : TICK_INTERVAL;
	i::Globals->iTickCount = GetTickbase(pCmd);

	i::Prediction->bIsFirstTimePredicted = false;
	i::Prediction->bInPrediction = true;

	/* skipped weapon select and vehicle predicts */

	if (pCmd->uImpulse)
		g_LocalPlayer->m_nImpulse() = pCmd->uImpulse;

	// synchronize m_afButtonForced & m_afButtonDisabled
	pCmd->iButtons |= g_LocalPlayer->GetButtonForced();
	pCmd->iButtons &= ~(g_LocalPlayer->GetButtonDisabled());

	// update button state
	const int iButtons = pCmd->iButtons;
	int nPlayerButtons = g_LocalPlayer->m_nButtons();
	const int nButtonsChanged = iButtons ^ nPlayerButtons;

	// synchronize m_afButtonLast
	g_LocalPlayer->m_afButtonLast() = nPlayerButtons;

	// synchronize m_nButtons
	g_LocalPlayer->m_nButtons() = iButtons;

	// synchronize m_afButtonPressed
	g_LocalPlayer->m_afButtonPressed() = iButtons & nButtonsChanged;

	// synchronize m_afButtonReleased
	g_LocalPlayer->m_afButtonPressed() = nButtonsChanged & ~iButtons;

	// check if the player is standing on a moving entity and adjusts velocity and basevelocity appropriately
	i::Prediction->CheckMovingGround(g_LocalPlayer, i::Globals->flFrameTime);

	// copy angles from command to player
	i::Prediction->SetLocalViewAngles(pCmd->angViewPoint);

	// run prethink
	if (g_LocalPlayer->PhysicsRunThink(THINK_FIRE_ALL_FUNCTIONS))
		g_LocalPlayer->PreThink();

	// run think
	int iNextThinkTick = g_LocalPlayer->m_nNextThinkTick();
	if (iNextThinkTick > 0 && iNextThinkTick <= GetTickbase(pCmd))
	{
		iNextThinkTick = TICK_NEVER_THINK;
		g_LocalPlayer->Think();
	}

	// set host player
	i::MoveHelper->SetHost(g_LocalPlayer);

	// setup move
	i::Prediction->SetupMove(g_LocalPlayer, pCmd, i::MoveHelper, &moveData);
	i::GameMovement->ProcessMovement(g_LocalPlayer, &moveData);

	// finish move
	i::Prediction->FinishMove(g_LocalPlayer, pCmd, &moveData);
	i::MoveHelper->ProcessImpacts();

	// run post think
	g_LocalPlayer->PstThink();

	// restore tickbase
	g_LocalPlayer->m_nTickBase() = iOldTickBase;

	// restore prediction states
	i::Prediction->bInPrediction = bOldInPrediction;
	i::Prediction->bIsFirstTimePredicted = bOldIsFirstPrediction;
}

void CPrediction::end(CUserCmd* pCmd)
{
	if (!g_LocalPlayer->IsAlive() || i::MoveHelper == nullptr)
		return;

	// reset host player
	i::MoveHelper->SetHost(nullptr);

	// restore globals
	i::Globals->flCurrentTime = flOldCurrentTime;
	i::Globals->flFrameTime = flOldFrameTime;
	i::Globals->iTickCount = iOldTickCount;

	// finish command
	g_LocalPlayer->GetCurrentCommand() = nullptr;

	// reset prediction seed
	*iPredictionRandomSeed = -1;

	// reset prediction entity
	pSetPredictionEntity = nullptr;

	// reset move
	i::GameMovement->Reset();
}

int CPrediction::GetTickbase(CUserCmd* pCmd)
{
	static int iTick = 0;
	static CUserCmd* pLastCmd = nullptr;

	if (pCmd != nullptr)
	{
		// if command was not predicted - increment tickbase
		if (pLastCmd == nullptr || pLastCmd->bHasBeenPredicted)
			iTick = g_LocalPlayer->m_nTickBase();
		else
			iTick++;

		pLastCmd = pCmd;
	}

	return iTick;
}

CPrediction prediction;