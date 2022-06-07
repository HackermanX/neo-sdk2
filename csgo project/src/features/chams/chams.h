#pragma once
#include "../../commonIncludes.h"

class chams {
public:
	chams();
	~chams();
	void on_dme(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* bone);
	void rem_p();
private:
};

extern chams g_chams;