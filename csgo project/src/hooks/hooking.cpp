#include "hooking.h"
//features
#include "../features/prediction/prediction.h"
#include "../features/backtracking/backtrack.h"
#include "../features/aimboting/legitbot.h"
#include "../csgo/datatypes/clzss.h"
#include "../../protobuf/csgo/netmessages.pb.h"
#include "../features/misc/misc.h"
#include "../features/chams/chams.h"
#include "../renderer/imgui/impl/imgui_impl_dx9.h"
#include "../renderer/menu.h"
#include "../renderer/render.h"
#include "../features/esp/esp.h"

steam_hook::game_overlay_renderer_hook_t steam_hook::hook;
steam_hook::game_overlay_renderer_unhook_t steam_hook::unhook;

void hooks::init() {
	steam_hook::hook = (steam_hook::game_overlay_renderer_hook_t)memory::PatternScan(XorStr("gameoverlayrenderer.dll"), XorStr("55 8B EC 51 8B 45 10 C7"));
	steam_hook::unhook = (steam_hook::game_overlay_renderer_unhook_t)memory::PatternScan(XorStr("gameoverlayrenderer.dll"), XorStr("55 8B ?? 64 ?? ?? ?? ?? ?? 6A ?? 68 ?? ?? ?? ?? 50 64 ?? ?? ?? ?? ?? ?? 81 ?? ?? ?? ?? ?? 56 8B ?? ?? 85"));

	const char* modules[]{ "client.dll", "engine.dll",
	"studiorender.dll", "materialsystem.dll"
	};

	for (auto cur : modules) {
		unk_name.Install(memory::PatternScan(cur, "55 8B EC 56 8B F1 33 C0 57 8B 7D 08"), &sub_10797090);
	};

	create_move.Install(memory::GetVFunc(i::Client, 22), &hkCreateMoveProxy);

	frame_stage_notify.Install(memory::GetVFunc(i::Client, 37), &hkFrameStageNotify);

	paint_traverse.Install(memory::GetVFunc(i::Panel, 41), &hkPaintTraverse);

	lock_cursor.Install(memory::GetVFunc(i::Surface, 67), &hkLockCursor);

	draw_model_execute.Install(memory::GetVFunc(i::ModelRender, 21), &hkDrawModelExecute);

	send_datagram.Install(memory::PatternScan(XorStr("engine.dll"), XorStr("55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 56 57 8B F9 89 7C 24 18")), &hkSendDatagram);

	end_scene.Install(memory::GetVFunc(i::DirectDevice, 42), &hkEndScene);

	add_entity.Install(memory::PatternScan(XorStr("client.dll"), XorStr("55 8B EC 51 8B 45 0C 53 56 8B F1 57")), &hkOnAddEntity);

	remove_entity.Install(memory::PatternScan(XorStr("client.dll"), XorStr("55 8B EC 51 8B 45 0C 53 8B D9 56 57 83 F8 FF 75 07")), &hkOnRemoveEntity);

	get_hint.Install(memory::GetVFunc(i::Localize, 11), &find_w_hooked);
}

void __stdcall CreateMove(int nSequenceNumber, float flInputSampleFrametime, bool bIsActive, bool& bSendPacket) {
	static auto original = hooks::create_move.get_original<decltype(&hooks::hkCreateMoveProxy)>();

	original(i::Client, 0, nSequenceNumber, flInputSampleFrametime, bIsActive);

	CUserCmd* cmd = i::Input->GetUserCmd(nSequenceNumber);
	CVerifiedUserCmd* v_cmd = i::Input->GetVerifiedCmd(nSequenceNumber);

	if (!cmd || !cmd->iCommandNumber)
		return;

	if (opt::bt::fake) fakeping.update_sequence();
	else fakeping.clear_sequence();

	move::init(cmd);
	esp::on_cm(cmd);

	prediction.start(cmd);
	{
		/*flag operators
		* or: |
		* and: &
		* not: ~
		*/
		
		backtrack.on_move(cmd);
	}
	prediction.end(cmd);

	v_cmd->cmd = *cmd;
	v_cmd->crc = cmd->GetChecksum();
}

__declspec(naked) void __fastcall hooks::hkCreateMoveProxy(IBaseClientDll* thisptr, int edx, int nSequenceNumber, float flInputSampleFrametime, bool bIsActive)
{
	__asm
	{
		push	ebp
		mov		ebp, esp; // store the stack
		push	ebx; // bSendPacket
		push	esp; // restore the stack
		push	dword ptr[bIsActive]; // ebp + 16
		push	dword ptr[flInputSampleFrametime]; // ebp + 12
		push	dword ptr[nSequenceNumber]; // ebp + 8
		call	CreateMove
		pop		ebx
		pop		ebp
		retn	0Ch
	}
}

void __stdcall hooks::hkFrameStageNotify(EClientFrameStage stage) {
	using fn = void(__thiscall*)(IBaseClientDll*, EClientFrameStage);
	static auto original = frame_stage_notify.get_original<fn>();
	//get local player
	g_LocalPlayer = CBasePlayer::GetLocalPlayer(true);

	if (!i::Engine->IsInGame()) {
		fakeping.clear_sequence();
		return original(i::Client, stage);
	}

	static auto set_interpolation_flags = [](CBasePlayer* e, int flag) {
		const auto var_map = (uintptr_t)e + 36;
		const auto sz_var_map = *(int*)(var_map + 20);

		for (auto index = 0; index < sz_var_map; index++)
			*(uintptr_t*)((*(uintptr_t*)var_map) + index * 12) = flag;
	};

	switch (stage) {
	case EClientFrameStage::FRAME_RENDER_START:
		backtrack.setup_records();
		break;
	case EClientFrameStage::FRAME_NET_UPDATE_END:
		if (g_LocalPlayer && g_LocalPlayer->IsAlive()) {
			for (uint32_t i = 1; i <= i::Globals->nMaxClients; i++) {
				CBasePlayer* player = reinterpret_cast<CBasePlayer*>(i::ClientEntityList->GetClientEntity(i));
				if (!player || !player->is_enemy()
					|| player == g_LocalPlayer || player->IsDormant())
					continue;
				set_interpolation_flags(player, 0); // disable interpolation
			}
		}

		g_chams.rem_p();
	}

	return original(i::Client, stage);
}

void __stdcall hooks::hkPaintTraverse(VPANEL panel, bool forceRepaint, bool allowForce) {
	using fn = void(__thiscall*)(IVPanel*, VPANEL, bool, bool);
	static auto panelId = VPANEL{ 0 };
	static auto original = paint_traverse.get_original<fn>();

	if (!panelId) {
		const auto panelName = i::Panel->GetName(panel);
		if (!strcmp(panelName, "FocusOverlayPanel")) {
			panelId = panel;
		}
	}
	else if (panelId == panel)
	{
		static bool bSkip = false;
		bSkip = !bSkip;
		if (bSkip)
			return;

		render::begin_scene();
	}

	original(i::Panel, panel, forceRepaint, allowForce);
}

void __stdcall hooks::hkLockCursor() {
	static auto original = lock_cursor.get_original<void(__thiscall*)(void*)>();
	if (menu::is_visible()) {
		i::Surface->UnLockCursor();
		i::Input->ResetInputState();
		return;
	}
	original(i::Surface);
}

void __stdcall hooks::hkDrawModelExecute(IMatRenderContext* context, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* bone) {
	using fn = void(__thiscall*)(IVModelRender*, IMatRenderContext*, const DrawModelState_t*, const ModelRenderInfo_t*, matrix3x4_t*);
	static auto original = draw_model_execute.get_original<fn>();

	if (i::ModelRender->IsForcedMaterialOverride() && !strstr(info.pModel->szName, "arms") && !strstr(info.pModel->szName, "weapons/v"))
		return original(i::ModelRender, context, &state, &info, bone);

	g_chams.on_dme(context, state, info, bone);

	original(i::ModelRender, context, &state, &info, bone);

	i::ModelRender->ForcedMaterialOverride(nullptr);
}

int __fastcall hooks::hkSendDatagram(INetChannel* thisptr, int edx, bf_write* datagram) {
	static auto original = send_datagram.get_original<decltype(&hkSendDatagram)>();

	if (!opt::bt::fake || !i::Engine->IsInGame() || !g_LocalPlayer || datagram != nullptr)
		return original(thisptr, edx, datagram);

	int instate = thisptr->iInReliableState;
	int insequencenr = thisptr->iInSequenceNr;

	fakeping.add_latency(thisptr);

	const int ret = original(thisptr, edx, datagram);
	thisptr->iInReliableState = instate;
	thisptr->iInSequenceNr = insequencenr;

	return ret;
}

long __stdcall hooks::hkEndScene(IDirect3DDevice9* dev) {
	static auto original = end_scene.get_original<decltype(&hkEndScene)>();

	IDirect3DStateBlock9* pixel_state = NULL;
	IDirect3DVertexDeclaration9* vert_dec;
	IDirect3DVertexShader9* vert_shader;

	dev->CreateStateBlock(D3DSBT_PIXELSTATE, &pixel_state);
	dev->GetVertexDeclaration(&vert_dec);
	dev->GetVertexShader(&vert_shader);

	dev->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);

	ImGui_ImplDX9_NewFrame();

	//define drawlist
	auto drawlist = render::render_scene();

	//draw your stuff here
	menu::render();

	//the rest of imgui stuff
	ImGui::Render(drawlist);

	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	pixel_state->Apply();
	pixel_state->Release();
	dev->SetVertexDeclaration(vert_dec);
	dev->SetVertexShader(vert_shader);
	return original(dev);
}

void __fastcall hooks::hkOnAddEntity(std::uintptr_t ecx, std::uintptr_t edx, IHandleEntity* entity, CBaseHandle handle) {
	static auto original = add_entity.get_original<void(__thiscall*)(std::uintptr_t, IHandleEntity*, CBaseHandle)>();

	IClientUnknown* unknown = reinterpret_cast<IClientUnknown*>(entity);
	CBaseEntity* base_entity = unknown->GetBaseEntity();

	listnr.add(base_entity);

	original(ecx, entity, handle);
}

void __fastcall hooks::hkOnRemoveEntity(std::uintptr_t ecx, std::uintptr_t edx, IHandleEntity* entity, CBaseHandle handle) {
	static auto original = remove_entity.get_original<void(__thiscall*)(std::uintptr_t, IHandleEntity*, CBaseHandle)>();

	IClientUnknown* unknown = reinterpret_cast<IClientUnknown*>(entity);
	CBaseEntity* base_entity = unknown->GetBaseEntity();

	listnr.remove(base_entity);

	original(ecx, entity, handle);
}

/*char __thiscall sub_7A7090(_DWORD* this, LPCSTR lpModuleName)
{
	_DWORD* v2; // esi
	int v3; // eax
	LPCSTR v4; // edi
	int v5; // ecx
	int v6; // ecx
	int v7; // eax

	v2 = this;
	v3 = 0;
	v4 = lpModuleName;
	v5 = this[160];*/

char __fastcall hooks::sub_10797090(void* thiscall, void* edx, const char* modulename) {//epic hook
	return 1;
}

wchar_t* __fastcall hooks::find_w_hooked(ILocalize* thisptr, void*, const char* token) {
	static auto original = get_hint.get_original<wchar_t* (__thiscall*)(ILocalize*, const char*)>();
	// this func is called a lot and string::find isn't the best thing to call a shitload of times per frame so
	if (!i::Engine->IsDrawingLoadingImage())
		return original(thisptr, token);

	std::string tmp = token;
	if (tmp.find("#SFUI_Hint_") != std::string::npos) {
		auto rand_member = rand() % 17;
		return const_cast<wchar_t*>(misc::rand_hint());//i am math
	}
	else
		return original(thisptr, token);
}

//SHIT I MIGHT USE SOME DAY
//
//bool FASTCALL H::hkIsConnected(IEngineClient* thisptr, int edx)
//{
//	static auto oIsConnected = DTR::IsConnected.GetOriginal<decltype(&hkIsConnected)>();
//
//	// @xref: "IsLoadoutAllowed"
//	// sub above the string
//	// sub in that function
//	// .text : 103A2120 84 C0		test    al, al; Logical Compare
//	static std::uintptr_t uLoadoutAllowedReturn = (MEM::FindPattern(CLIENT_DLL, XorStr("75 04 B0 01 5F")) - 0x2);
//
//	// @credits: gavreel
//	if (reinterpret_cast<std::uintptr_t>(_ReturnAddress()) == uLoadoutAllowedReturn && C::Get<bool>(Vars.bMiscUnlockInventory))
//		return false;
//
//	return oIsConnected(thisptr, edx);
//}