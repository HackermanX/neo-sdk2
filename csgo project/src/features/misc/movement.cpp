#include "misc.h"

void move::init(CUserCmd* cmd) {
	if (g_LocalPlayer->m_fFlags() & FL_INWATER
		|| g_LocalPlayer->m_nMoveType() == MOVETYPE_LADDER
		|| g_LocalPlayer->m_nMoveType() == MOVETYPE_NOCLIP)
		return;

	if (opt::move::bhop) {
		if (!(g_LocalPlayer->m_fFlags() & FL_ONGROUND))
			cmd->iButtons &= ~IN_JUMP;
	}
}