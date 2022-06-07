#pragma once
#include <string>
#include <sstream>
#include <stdint.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "imgui/imgui.h"
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#include "../commonIncludes.h"
#include "imgui/impl/imgui_impl_dx9.h"
#include "fonts/fonts.hpp"
#include "imgui/imgui_internal.h"
extern ImFont* g_pDefaultFont;
extern ImFont* g_pSecondFont;
extern ImFont* g_pIcons;
class Vector;

namespace render
{
	inline ImDrawList* draw_list_act;
	inline ImDrawList* draw_list_rendering;
	inline ImDrawList* draw_list;
	inline ImDrawData draw_data;

	inline ImU32 GetU32(Color _color)
	{
		return ((_color[3] & 0xff) << 24) + ((_color[2] & 0xff) << 16) + ((_color[1] & 0xff) << 8)
			+ (_color[0] & 0xff);
	}
	void init();
	void get_fonts();
	void clear_dl();
	void begin_scene();
	ImDrawList* render_scene();

	inline void render_image(ImTextureID user_texture_id, const ImVec2& a, const ImVec2& b, const ImVec2& uv_a = ImVec2(0, 0), const ImVec2& uv_b = ImVec2(1, 1), ImU32 col = 0xFFFFFFFF) {
		draw_list->AddImage(user_texture_id, a, b, uv_a, uv_b, col);
	}

	template <class T>
	inline void render_box_by_type(T x1, T y1, T x2, T y2, Color color, float thickness = 1.f, int type = 0) {
		if (type == 0)
			RenderBox(x1, y1, x2, y2, color, thickness);
		else if (type == 1)
			RenderCoalBox(x1, y1, x2, y2, color);
		else if (type == 2)
			RenderBox(x1, y1, x2, y2, color, thickness, 8.f);
	}

	template <class T>
	inline void render_filled_box_by_type(T x1, T y1, T x2, T y2, Color color, float thickness = 1.f, int type = 0) {
		if (type == 0 || type == 1)
			RenderBoxFilled(x1, y1, x2, y2, color, thickness);
		else if (type == 2)
			RenderBoxFilled(x1, y1, x2, y2, color, thickness, 8.f);
	}

	template <class T>
	inline void render_coal_box(T x1, T y1, T x2, T y2, Color color, float th = 1.f) {
		int w = x2 - x1;
		int h = y2 - y1;

		int iw = w / 4;
		int ih = h / 4;
		// top
		render_line(x1, y1, x1 + iw, y1, color, th);					// left
		render_line(x1 + w - iw, y1, x1 + w, y1, color, th);			// right
		render_line(x1, y1, x1, y1 + ih, color, th);					// top left
		render_line(x1 + w - 1, y1, x1 + w - 1, y1 + ih, color, th);	// top right
																	// bottom
		render_line(x1, y1 + h, x1 + iw, y1 + h, color, th);			// left
		render_line(x1 + w - iw, y1 + h, x1 + w, y1 + h, color, th);	// right
		render_line(x1, y1 + h - ih, x1, y1 + h, color, th);			// bottom left
		render_line(x1 + w - 1, y1 + h - ih, x1 + w - 1, y1 + h, color, th);	// bottom right
	}

	template <class T>
	inline void render_box(T x1, T y1, T x2, T y2, Color color, float thickness = 1.f, float rounding = 0.f) {
		draw_list->AddRect(ImVec2(x1, y1), ImVec2(x2, y2), GetU32(color), rounding, 15, thickness);
	}
	inline void render_box(RECT r, Color color, float thickness = 1.f, float rounding = 0.f) {
		render_box(r.left, r.top, r.right, r.bottom, color, thickness, rounding);
	}
	template <class T>
	inline void render_box_filled(T x1, T y1, T x2, T y2, Color color, float thickness = 1.f, float rounding = 0.f) {
		draw_list->AddRectFilled(ImVec2(x1, y1), ImVec2(x2, y2), GetU32(color), rounding, 15);
	}
	template <class T>
	inline void render_line(T x1, T y1, T x2, T y2, Color color, float thickness = 1.f) {
		draw_list->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), GetU32(color), thickness);
	}
	template <class T>
	inline void render_circle(T x, T y, float radius, int points, Color color, float thickness = 1.f) {
		draw_list->AddCircle(ImVec2(x, y), radius, GetU32(color), points, thickness);
	}
	template <class T>
	inline void render_circle_filled(T x, T y, float radius, int points, Color color) {
		draw_list->AddCircleFilled(ImVec2(x, y), radius, GetU32(color), points);
	}
};