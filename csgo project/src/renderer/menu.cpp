#include "menu.h"
#include "imgui/impl/imgui_impl_dx9.h"
#include "../csgo/other/input.h"
#include "../options.h"

namespace menu {
	void render() {
		if (!_visible) return;

		auto flags = ImGuiWindowFlags_NoSavedSettings | NULL | NULL | ImGuiWindowFlags_NoResize | NULL | NULL | NULL | NULL;
		auto flags_alpha = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_AlphaPreview;
		auto flags_no_alpha = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip;

		ImGui::SetNextWindowSize({ 168.f, 200.f });

		ImGui::Begin("neo-sdk", nullptr, flags | ImGuiWindowFlags_NoTitleBar);
		{
			ImGui::BeginTabBar("features", ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_NoTooltip);

			if (ImGui::BeginTabItem("legit")) {
				ImGui::SetCursorPos({ 8, 30 });

				ImGui::Checkbox("backtracking", &opt::bt::toggle);
				ImGui::SliderInt("time", &opt::bt::time, 0, 200);
				ImGui::SliderInt("fake", &opt::bt::fake, 0, 1000);

				ImGui::EndTabItem();
			}ImGui::SameLine();

			if (ImGui::BeginTabItem("esp")) {
				ImGui::SetCursorPos({ 8, 30 });

				ImGui::Checkbox("radar", &opt::esp::radar);
				ImGui::Checkbox("recoil crosshair", &opt::esp::crosshair);
				ImGui::Checkbox("Backtrack dots", &opt::esp::bt_dots);

				ImGui::EndTabItem();
			}ImGui::SameLine();

			if (ImGui::BeginTabItem("chams")) {
				ImGui::SetCursorPos({ 8, 30 });

				ImGui::Checkbox("toggle", &opt::chams::toggle); ImGui::SameLine();
				ImGui::ColorEdit4("##visible", opt::chams::visible, flags_alpha);
				ImGui::Checkbox("xqz", &opt::chams::xqz); ImGui::SameLine();
				ImGui::ColorEdit4("##xqz", opt::chams::hidden, flags_alpha);
				ImGui::Checkbox("visualize bt", &opt::chams::bt);

				ImGui::EndTabItem();
			}ImGui::SameLine();

			if (ImGui::BeginTabItem("misc")) {
				ImGui::SetCursorPos({ 8, 30 });

				ImGui::Checkbox("bhop", &opt::move::bhop);

				ImGui::EndTabItem();
			}ImGui::EndTabBar();
		}
		ImGui::End();
	}
	void toggle()
	{
		_visible = !_visible;
	}
	bool is_visible()
	{
		return _visible;
	}
}