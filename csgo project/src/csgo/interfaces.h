#pragma once
#include <Windows.h>
#include "interfaces/ibaseclientdll.h"
#include "interfaces/icliententitylist.h"
#include "interfaces/isurface.h"
#include "interfaces/iclientmodeshared.h"
#include "interfaces/iconvar.h"
#include "interfaces/ieffects.h"
#include "interfaces/inetchannel.h"
#include "interfaces/iengineclient.h"
#include "interfaces/ienginetrace.h"
#include "interfaces/ienginevgui.h"
#include "interfaces/ienginesound.h"
#include "interfaces/igameconsole.h"
#include "interfaces/igameeventmanager.h"
#include "interfaces/ivpanel.h"
#include "interfaces/igameui.h"
#include "interfaces/iglobalvars.h"
#include "interfaces/iservergamedll.h"
#include "interfaces/iinput.h"
#include "interfaces/iinputsystem.h"
#include "interfaces/ilocalize.h"
#include "interfaces/imatchframework.h"
#include "interfaces/igametypes.h"
#include "interfaces/imaterialsystem.h"
#include "interfaces/imdlcache.h"
#include "interfaces/imemalloc.h"
#include "interfaces/inetworkstring.h"
#include "interfaces/iphysicssurfaceprops.h"
#include "interfaces/iprediction.h"
#include "interfaces/isteamclient.h"
#include "interfaces/isteamgamecoordinator.h"
#include "interfaces/istudiorender.h"
#include "interfaces/ivdebugoverlay.h"
#include "interfaces/ivengineeffects.h"
#include "interfaces/iglowmanager.h"
#include "interfaces/iviewrender.h"
#include "interfaces/iviewrenderbeams.h"
#include "interfaces/ivmodelinfo.h"
#include "interfaces/ivmodelrender.h"
#include "interfaces/ivrenderview.h"
#include "interfaces/iweaponsystem.h"
#include "memory.h"
#include <d3d9.h>

class CInterfaceRegister
{
public:
	InstantiateInterfaceFn	pCreateFn;		//0x0
	const char* szName;			//0x4
	CInterfaceRegister* pNext;			//0x8
};

namespace i {
	inline IBaseClientDll* Client;
	inline IClientEntityList* ClientEntityList;
	inline IEffects* Effects;
	inline IGameMovement* GameMovement;
	inline IMoveHelper* MoveHelper;
	inline IPrediction* Prediction;
	inline IGameConsole* GameConsole;
	inline IGameUI* GameUI;
	inline IEngineClient* Engine;
	inline IEngineVGui* EngineVGui;
	inline IEngineTrace* EngineTrace;
	inline IEngineSound* EngineSound;
	inline INetworkContainer* NetworkContainer;
	inline IGameEventManager2* GameEvent;
	inline IVRenderView* RenderView;
	inline IVDebugOverlay* DebugOverlay;
	inline IVEngineEffects* EngineEffects;
	inline IVModelInfo* ModelInfo;
	inline IVModelRender* ModelRender;
	inline IMDLCache* MDLCache;
	inline IStudioRender* StudioRender;
	inline IConVar* ConVar;
	inline IPhysicsSurfaceProps* PhysicsProps;
	inline IMaterialSystem* MaterialSystem;
	inline ISurface* Surface;
	inline IVPanel* Panel;
	inline IInputSystem* InputSystem;
	inline ILocalize* Localize;
	inline IMatchFramework* MatchFramework;
	inline IGameTypes* GameTypes;
	inline IServerGameDLL* Server;
	inline ISteamClient* SteamClient;
	inline ISteamUser* SteamUser;
	inline ISteamGameCoordinator* SteamGameCoordinator;
	inline IClientMode* ClientMode;
	inline IGlobalVarsBase* Globals;
	inline IMemAlloc* MemAlloc;
	inline IDirect3DDevice9* DirectDevice;
	inline IViewRender* ViewRender;
	inline IViewRenderBeams* ViewRenderBeams;
	inline IInput* Input;
	inline IClientState* ClientState;
	inline IWeaponSystem* WeaponSystem;
	inline IGlowObjectManager* GlowManager;

	template<typename t> // thanx quo for the abnormal amount of interfaces
	t* create(const char* szModule, std::string_view szInterface) {
		const auto GetRegisterList = [&szModule]() -> CInterfaceRegister*
		{
			FARPROC oCreateInterface = nullptr;

			if (const auto hModule = GetModuleHandle(szModule); hModule != nullptr)
				oCreateInterface = GetProcAddress(hModule, XorStr("CreateInterface"));

			if (oCreateInterface == nullptr)
				throw std::runtime_error(XorStr("failed get createinterface address"));

			const std::uintptr_t uCreateInterfaceRelative = reinterpret_cast<std::uintptr_t>(oCreateInterface) + 0x5;
			const std::uintptr_t uCreateInterface = uCreateInterfaceRelative + 4U + *reinterpret_cast<std::int32_t*>(uCreateInterfaceRelative);
			return **reinterpret_cast<CInterfaceRegister***>(uCreateInterface + 0x6);
		};

		for (auto pRegister = GetRegisterList(); pRegister != nullptr; pRegister = pRegister->pNext)
		{
			// found needed interface
			if ((std::string_view(pRegister->szName).compare(0U, szInterface.length(), szInterface) == 0 &&
				// and it have digits after name
				std::atoi(pRegister->szName + szInterface.length()) > 0) ||
				// or given full interface with hardcoded digits
				szInterface.compare(pRegister->szName) == 0)
			{
				// capture our interface
				auto pInterface = pRegister->pCreateFn();

				return static_cast<t*>(pInterface);
			}
		}
		return nullptr;
	}

	__forceinline void init() {
		Client = create<IBaseClientDll>(CLIENT_DLL, XorStr("VClient"));
		ClientEntityList = create<IClientEntityList>(CLIENT_DLL, XorStr("VClientEntityList"));
		Effects = create<IEffects>(CLIENT_DLL, XorStr("IEffects"));
		GameMovement = create<IGameMovement>(CLIENT_DLL, XorStr("GameMovement"));
		Prediction = create<IPrediction>(CLIENT_DLL, XorStr("VClientPrediction"));
		GameConsole = create<IGameConsole>(CLIENT_DLL, XorStr("GameConsole"));
		GameUI = create<IGameUI>(CLIENT_DLL, XorStr("GameUI"));
		Engine = create<IEngineClient>(ENGINE_DLL, XorStr("VEngineClient"));
		EngineVGui = create<IEngineVGui>(ENGINE_DLL, XorStr("VEngineVGui"));
		EngineTrace = create<IEngineTrace>(ENGINE_DLL, XorStr("EngineTraceClient"));
		EngineSound = create<IEngineSound>(ENGINE_DLL, XorStr("IEngineSoundClient"));
		NetworkContainer = create<INetworkContainer>(ENGINE_DLL, XorStr("VEngineClientStringTable"));
		GameEvent = create<IGameEventManager2>(ENGINE_DLL, XorStr("GAMEEVENTSMANAGER002"));
		RenderView = create<IVRenderView>(ENGINE_DLL, XorStr("VEngineRenderView"));
		DebugOverlay = create<IVDebugOverlay>(ENGINE_DLL, XorStr("VDebugOverlay"));
		EngineEffects = create<IVEngineEffects>(ENGINE_DLL, XorStr("VEngineEffects"));
		ModelInfo = create<IVModelInfo>(ENGINE_DLL, XorStr("VModelInfoClient"));
		ModelRender = create<IVModelRender>(ENGINE_DLL, XorStr("VEngineModel"));
		MDLCache = create<IMDLCache>(DATACACHE_DLL, XorStr("MDLCache"));
		StudioRender = create<IStudioRender>(STUDIORENDER_DLL, XorStr("VStudioRender"));
		ConVar = create<IConVar>(VSTDLIB_DLL, XorStr("VEngineCvar"));
		PhysicsProps = create<IPhysicsSurfaceProps>(PHYSICS_DLL, XorStr("VPhysicsSurfaceProps"));
		MaterialSystem = create<IMaterialSystem>(MATERIALSYSTEM_DLL, XorStr("VMaterialSystem"));
		Surface = create<ISurface>(VGUI_DLL, XorStr("VGUI_Surface"));
		Panel = create<IVPanel>(VGUI2_DLL, XorStr("VGUI_Panel"));
		InputSystem = create<IInputSystem>(INPUTSYSTEM_DLL, XorStr("InputSystemVersion"));
		Localize = create<ILocalize>(LOCALIZE_DLL, XorStr("Localize_"));
		MatchFramework = create<IMatchFramework>(MATCHMAKING_DLL, XorStr("MATCHFRAMEWORK_"));
		GameTypes = create<IGameTypes>(MATCHMAKING_DLL, XorStr("VENGINE_GAMETYPES_VERSION"));
		Server = create<IServerGameDLL>(SERVER_DLL, XorStr("ServerGameDLL"));

		SteamClient = Engine->GetSteamAPIContext()->pSteamClient;
		SteamUser = Engine->GetSteamAPIContext()->pSteamUser;

		const HSteamUser hSteamUser = reinterpret_cast<std::add_pointer_t<HSteamUser()>>(GetProcAddress(GetModuleHandleA(STEAM_API_DLL), XorStr("SteamAPI_GetHSteamUser")))();
		const HSteamPipe hSteamPipe = reinterpret_cast<std::add_pointer_t<HSteamPipe()>>(GetProcAddress(GetModuleHandleA(STEAM_API_DLL), XorStr("SteamAPI_GetHSteamPipe")))();

		SteamGameCoordinator = static_cast<ISteamGameCoordinator*>(SteamClient->GetISteamGenericInterface(hSteamUser, hSteamPipe, XorStr("SteamGameCoordinator001")));
		ClientMode = **reinterpret_cast<IClientMode***>(memory::GetVFunc<std::uintptr_t>(Client, 10) + 0x5);
		Globals = **reinterpret_cast<IGlobalVarsBase***>(memory::GetVFunc<std::uintptr_t>(Client, 11) + 0xA);
		MemAlloc = *reinterpret_cast<IMemAlloc**>(GetProcAddress(GetModuleHandleA(TIER0_DLL), XorStr("g_pMemAlloc")));
		DirectDevice = **reinterpret_cast<IDirect3DDevice9***>(memory::PatternScan(SHADERPIDX9_DLL, XorStr("A1 ? ? ? ? 50 8B 08 FF 51 0C")) + 0x1);
		ViewRender = **reinterpret_cast<IViewRender***>(memory::PatternScan(CLIENT_DLL, XorStr("8B 0D ? ? ? ? FF 75 0C 8B 45 08")) + 0x2);
		ViewRenderBeams = *reinterpret_cast<IViewRenderBeams**>(memory::PatternScan(CLIENT_DLL, XorStr("B9 ? ? ? ? A1 ? ? ? ? FF 10 A1 ? ? ? ? B9")) + 0x1);
		Input = *reinterpret_cast<IInput**>(memory::PatternScan(CLIENT_DLL, XorStr("B9 ? ? ? ? F3 0F 11 04 24 FF 50 10")) + 0x1);
		ClientState = **reinterpret_cast<IClientState***>(memory::PatternScan(ENGINE_DLL, XorStr("A1 ? ? ? ? 8B 88 ? ? ? ? 85 C9 75 07")) + 0x1);
		WeaponSystem = *reinterpret_cast<IWeaponSystem**>(memory::PatternScan(CLIENT_DLL, XorStr("8B 35 ? ? ? ? FF 10 0F B7 C0")) + 0x2);
		GlowManager = *reinterpret_cast<IGlowObjectManager**>(memory::PatternScan(CLIENT_DLL, XorStr("0F 11 05 ? ? ? ? 83 C8 01")) + 0x3);
	}
}

//inline POINT g_MousePosition{ 0, 0 };
//inline float g_ScrollAmount = 0.f;
//inline HWND g_CurrentWindow;
//inline Color g_CopiedColor;