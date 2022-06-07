#pragma once
#include "interfaces.h"
#include "netvars.h"

namespace cvar {
	inline float sv_maxunlag;
	inline float sv_minupdaterate;
	inline float sv_maxupdaterate;
	inline float sv_client_min_interp_ratio;
	inline float sv_client_max_interp_ratio;
	inline float cl_interp_ratio;
	inline float cl_interp;
	inline float cl_updaterate;

	inline float flLerpRatio;
	inline float updateRate;

	inline void init() {
		sv_maxunlag = i::ConVar->FindVar("sv_maxunlag")->GetFloat();
		sv_minupdaterate = i::ConVar->FindVar("sv_minupdaterate")->GetFloat();
		sv_maxupdaterate = i::ConVar->FindVar("sv_maxupdaterate")->GetFloat();
		sv_client_min_interp_ratio = i::ConVar->FindVar("sv_client_min_interp_ratio")->GetFloat();
		sv_client_max_interp_ratio = i::ConVar->FindVar("sv_client_max_interp_ratio")->GetFloat();
		cl_interp_ratio = i::ConVar->FindVar("cl_interp_ratio")->GetFloat();
		cl_interp = i::ConVar->FindVar("cl_interp")->GetFloat();
		cl_updaterate = i::ConVar->FindVar("cl_updaterate")->GetFloat();

		flLerpRatio = std::clamp(flLerpRatio, sv_client_min_interp_ratio, sv_client_max_interp_ratio);
		if (flLerpRatio == 0.0f)
			flLerpRatio = 1.0f;
		updateRate = std::clamp(cl_updaterate, sv_minupdaterate, sv_maxupdaterate);
	}
}