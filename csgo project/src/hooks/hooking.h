#pragma once
#include "../commonIncludes.h"
#include <iostream>
#include "steam_hook.h"
#include <algorithm>

namespace hooks {
	inline steam_hook create_move;
	inline steam_hook frame_stage_notify;
	inline steam_hook paint_traverse;
	inline steam_hook lock_cursor;
	inline steam_hook draw_model_execute;
	inline steam_hook send_datagram;
	inline steam_hook end_scene;
	inline steam_hook add_entity;
	inline steam_hook remove_entity;
	inline steam_hook unk_name;
	inline steam_hook get_hint;

	void init();

	void __fastcall hkCreateMoveProxy(IBaseClientDll* thisptr, int edx, int nSequenceNumber, float flInputSampleFrametime, bool bIsActive);
	void __stdcall hkFrameStageNotify(EClientFrameStage stage);
	void __stdcall hkPaintTraverse(VPANEL panel, bool forceRepaint, bool allowForce);
	void __stdcall hkLockCursor();
	void __stdcall hkDrawModelExecute(IMatRenderContext* context, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* bone);
	int __fastcall hkSendDatagram(INetChannel* thisptr, int edx, bf_write* datagram);
	long __stdcall hkEndScene(IDirect3DDevice9* dev);
	void __fastcall hkOnAddEntity(std::uintptr_t ecx, std::uintptr_t edx, IHandleEntity* entity, CBaseHandle handle);
	void __fastcall hkOnRemoveEntity(std::uintptr_t ecx, std::uintptr_t edx, IHandleEntity* entity, CBaseHandle handle);
	char __fastcall sub_10797090(void* thiscall, void* edx, const char* modulename);
	wchar_t* __fastcall find_w_hooked(ILocalize* thisptr, void*, const char* token);
}