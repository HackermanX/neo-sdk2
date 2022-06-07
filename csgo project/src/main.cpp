#include <windows.h>
#include "hooks/hooking.h"
#include "commonIncludes.h"
#include <functional>
#include "renderer/render.h"
#include "renderer/menu.h"

void initializer(const char* name, std::function<void()> function) {
	memory::console_print("<Initializing %s...>\n", name);
	function();
	memory::console_print("<%s initialized!>\n", name);
}

unsigned long __stdcall on_attach(void* base) {
	memory::attach_console();

	auto start = std::chrono::high_resolution_clock::now();

	initializer("Interfaces", i::init);

	initializer("Netvars", netvar_manager::init);

	initializer("Input", input_system::init);

	initializer("Render", render::init);

	initializer("Cvars", cvar::init);

	initializer("Hooks", hooks::init);

	input_system::register_hotkey(VK_INSERT, [base]() {
		menu::toggle();
		});

	auto end = std::chrono::high_resolution_clock::now();
	auto nano = std::chrono::duration_cast<std::chrono::nanoseconds, long long>(end - start).count();

	memory::console_print("<Cheat initialized succesfully in (%f)sec!>\n", static_cast<float>(nano) / 1000000000.f); // dont even consider asking wtf is this console bs

	/*server lagger*/
	//_beginthreadex(0, 0, lag::LagThread, 0, NULL, 0);

	while (!GetAsyncKeyState(VK_END))
		Sleep(1000);

	FreeLibraryAndExitThread(static_cast<HMODULE>(base), 1);
}

BOOL WINAPI on_detach() {
	memory::detach_console();
	//unhook
	hooks::create_move.Remove();
	hooks::frame_stage_notify.Remove();
	hooks::paint_traverse.Remove();
	hooks::lock_cursor.Remove();
	hooks::draw_model_execute.Remove();
	hooks::end_scene.Remove();
	hooks::send_datagram.Remove();
	hooks::add_entity.Remove();
	hooks::remove_entity.Remove();
	hooks::unk_name.Remove();

	return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved) {
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		CreateThread(nullptr, 0, static_cast<LPTHREAD_START_ROUTINE>(on_attach), hModule, 0, nullptr);
		return TRUE;
	case DLL_PROCESS_DETACH:
		if (lpReserved == nullptr)
			return on_detach();
		return TRUE;
	}
	return TRUE;
}