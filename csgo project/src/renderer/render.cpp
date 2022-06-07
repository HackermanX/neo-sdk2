#include "render.h"
#include <mutex>
#include "../features/esp/esp.h"

ImFont* g_pDefaultFont;
ImFont* g_pSecondFont;
ImFont* g_pIcons;

ImDrawListSharedData _data;

std::mutex render_mutex;

void render::init()
{
	ImGui::CreateContext();

	ImGui_ImplDX9_Init(input_system::get_main_window(), i::DirectDevice);

	draw_list = new ImDrawList(ImGui::GetDrawListSharedData());
	draw_list_act = new ImDrawList(ImGui::GetDrawListSharedData());
	draw_list_rendering = new ImDrawList(ImGui::GetDrawListSharedData());

	get_fonts();
}

void render::get_fonts() {
	// menu font
	ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
		Fonts::Droid_compressed_data,
		Fonts::Droid_compressed_size,
		14.f);
	// esp font
	g_pDefaultFont = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
		Fonts::Droid_compressed_data,
		Fonts::Droid_compressed_size,
		18.f);
	// font for watermark; just example
	g_pSecondFont = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
		Fonts::Cousine_compressed_data,
		Fonts::Cousine_compressed_size,
		18.f);
//	//icon font
//	g_pIcons = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
//		IconsForTabs_compressed_data,
//		IconsForTabs_compressed_size,
//		18.f);
}

void render::clear_dl() {
	render_mutex.lock();
	draw_list_act->Clear();
	render_mutex.unlock();
}

void render::begin_scene() {
	draw_list->Clear();
	draw_list->PushClipRectFullScreen();

	if (i::Engine->IsInGame()) {
		esp::render();
	}

	render_mutex.lock();
	*draw_list_act = *draw_list;
	render_mutex.unlock();
}

ImDrawList* render::render_scene() {
	if (render_mutex.try_lock()) {
		*draw_list_rendering = *draw_list_act;
		render_mutex.unlock();
	}
	return draw_list_rendering;
}