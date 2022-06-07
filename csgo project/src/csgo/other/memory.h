#pragma once
#include "../other/xor.h"
#include <windows.h>
#include <cstdint>
#include <algorithm>
#include <sstream>
#include <vector>
#include "../datatypes/datamap.h"

#define FIND_NT_HEADER(x) reinterpret_cast<PIMAGE_NT_HEADERS>( uint32_t(x) + reinterpret_cast<PIMAGE_DOS_HEADER>(x)->e_lfanew )

#define ENGINE_DLL				XorStr("engine.dll")
#define CLIENT_DLL				XorStr("client.dll")
#define LOCALIZE_DLL			XorStr("localize.dll")
#define MATERIALSYSTEM_DLL		XorStr("materialsystem.dll")
#define VGUI_DLL				XorStr("vguimatsurface.dll")
#define VGUI2_DLL				XorStr("vgui2.dll")
#define SHADERPIDX9_DLL			XorStr("shaderapidx9.dll")
#define GAMEOVERLAYRENDERER_DLL XorStr("gameoverlayrenderer.dll")
#define PHYSICS_DLL				XorStr("vphysics.dll")
#define VSTDLIB_DLL				XorStr("vstdlib.dll")
#define TIER0_DLL				XorStr("tier0.dll")
#define INPUTSYSTEM_DLL			XorStr("inputsystem.dll")
#define STUDIORENDER_DLL		XorStr("studiorender.dll")
#define DATACACHE_DLL			XorStr("datacache.dll")
#define STEAM_API_DLL			XorStr("steam_api.dll")
#define MATCHMAKING_DLL			XorStr("matchmaking.dll")
#define SERVER_DLL				XorStr("server.dll")
#define SERVERBROWSER_DLL		XorStr("serverbrowser.dll")

namespace memory {
	inline HANDLE _out = NULL, _old_out = NULL;
	inline HANDLE _err = NULL, _old_err = NULL;
	inline HANDLE _in = NULL, _old_in = NULL;

	__forceinline uint8_t* PatternScan(const char* module_name, const std::string_view signature) {
		std::vector<uint8_t> signature_bytes{ }; {
			signature_bytes.resize(signature.size());

			std::vector<std::string> signature_chunks{ };
			std::string current_chunk{ };
			std::istringstream string_stream{ signature.data() };

			while (std::getline(string_stream, current_chunk, ' '))
				signature_chunks.push_back(current_chunk);

			std::transform(signature_chunks.cbegin(), signature_chunks.cend(), signature_bytes.begin(), [](const std::string& val) -> uint8_t {
				return val.find('?') != std::string::npos ? 0ui8 : static_cast<uint8_t>(std::stoi(val, nullptr, 16)); });
		}
		uint8_t* found_bytes = nullptr; {
			const auto image_start = reinterpret_cast<uint8_t*>(GetModuleHandleA(module_name));
			const auto image_end = image_start + FIND_NT_HEADER(image_start)->OptionalHeader.SizeOfImage;

			const auto result = std::search(image_start, image_end, signature_bytes.cbegin(), signature_bytes.cend(), [](uint8_t left, uint8_t right) -> bool {
				return right == 0ui8 || left == right; });

			found_bytes = (result != image_end) ? result : nullptr;
		} return found_bytes;
	}

	__forceinline unsigned int FindInDataMap(DataMap_t* pMap, const char* name) {
		while (pMap) {
			for (int i = 0; i < pMap->nDataFields; i++) {
				if (pMap->pDataDesc[i].szFieldName == NULL)
					continue;

				if (strcmp(name, pMap->pDataDesc[i].szFieldName) == 0)
					return pMap->pDataDesc[i].iFieldOffset[TD_OFFSET_NORMAL];

				if (pMap->pDataDesc[i].iFieldType == FIELD_EMBEDDED) {
					if (pMap->pDataDesc[i].pTypeDescription) {
						unsigned int offset;

						if ((offset = FindInDataMap(pMap->pDataDesc[i].pTypeDescription, name)) != 0)
							return offset;
					}
				}
			}
			pMap = pMap->pBaseMap;
		}

		return 0;
	}

	/*
	* virtual function implementation
	* returns native function of specified class at given index
	*/
	template <typename T = void*>
	constexpr T GetVFunc(void* thisptr, std::size_t nIndex)
	{
		return (*static_cast<T**>(thisptr))[nIndex];
	}

	/*
	 * virtual function call implementation
	 * calls function of specified class at given index
	 * @note: references should be wrapped with std::ref call!
	 */
	template <typename T, typename ... args_t>
	constexpr T CallVFunc(void* thisptr, std::size_t nIndex, args_t... argList)
	{
		using VirtualFn = T(__thiscall*)(void*, decltype(argList)...);
		return (*static_cast<VirtualFn**>(thisptr))[nIndex](thisptr, argList...);
	}
	//console
	__forceinline void attach_console() {
		_old_out = GetStdHandle(STD_OUTPUT_HANDLE);
		_old_err = GetStdHandle(STD_ERROR_HANDLE);
		_old_in = GetStdHandle(STD_INPUT_HANDLE);

		::AllocConsole() && ::AttachConsole(GetCurrentProcessId());

		_out = GetStdHandle(STD_OUTPUT_HANDLE);
		_err = GetStdHandle(STD_ERROR_HANDLE);
		_in = GetStdHandle(STD_INPUT_HANDLE);

		SetConsoleMode(_out,
			ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);

		SetConsoleMode(_in,
			ENABLE_INSERT_MODE | ENABLE_EXTENDED_FLAGS |
			ENABLE_PROCESSED_INPUT | ENABLE_QUICK_EDIT_MODE);
	}

	__forceinline void detach_console() {
		if (_out && _err && _in) {
			FreeConsole();
			if (_old_out)
				SetStdHandle(STD_OUTPUT_HANDLE, _old_out);
			if (_old_err)
				SetStdHandle(STD_ERROR_HANDLE, _old_err);
			if (_old_in)
				SetStdHandle(STD_INPUT_HANDLE, _old_in);
		}
	}

	__forceinline bool console_print(const char* fmt, ...) {
		if (!_out)
			return false;
		char buf[1024];
		va_list va;
		va_start(va, fmt);
		_vsnprintf_s(buf, 1024, fmt, va);
		va_end(va);
		return !!WriteConsoleA(_out, buf, static_cast<DWORD>(strlen(buf)), nullptr, nullptr);
	}
}