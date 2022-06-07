#pragma once
#include "../../commonIncludes.h"
#include "../backtracking/backtrack.h"
#include <functional>
#include <memory>
#include <vector>
#include <ranges>

struct listenr {
	listenr() {
		this->players.reserve(64);
	}
	inline ~listenr() {};

	void add(CBaseEntity* pl) {
		if (pl) {
			const auto it = std::ranges::find(this->players, pl);
			if (pl != g_LocalPlayer && it == this->players.cend() && pl->GetClientNetworkable()->GetIndex() >= 1 && pl->GetClientNetworkable()->GetIndex() <= 64)
				this->players.push_back(pl);
		}
	}

	void remove(CBaseEntity* pl) {
		if (pl) {
			const auto it = std::ranges::find(this->players, pl);
			if (it != this->players.cend())
				this->players.erase(it);
		}
	}

	void reset() {
		this->players.clear();
	}

	void loop(const std::function<void(CBaseEntity*)>& run) const {
		for (const auto pl : this->players) {
			if (pl && pl != g_LocalPlayer)
				run(pl);
		}
	}

private:
	std::vector<CBaseEntity*> players = {};
};

inline listenr listnr;

namespace esp {
	void render();
	void recoil_c();
	void radar(CBaseEntity* ent);
	void bt_dots(CBaseEntity* ent);
	void on_cm(CUserCmd* cmd);

	inline bool is_shooting = false;

	inline RECT GetBBox(CBaseEntity* ent)
	{
		RECT rect{};
		auto collideable = ent->GetCollideable();

		if (!collideable)
			return rect;

		auto min = collideable->OBBMins();
		auto max = collideable->OBBMaxs();

		const matrix3x4_t& trans = ent->m_rgflCoordinateFrame();

		Vector points[] = {
			Vector(min.x, min.y, min.z),
			Vector(min.x, max.y, min.z),
			Vector(max.x, max.y, min.z),
			Vector(max.x, min.y, min.z),
			Vector(max.x, max.y, max.z),
			Vector(min.x, max.y, max.z),
			Vector(min.x, min.y, max.z),
			Vector(max.x, min.y, max.z)
		};

		Vector pointsTransformed[8];
		for (int i = 0; i < 8; i++) {
			math::vector_transform(points[i], trans, pointsTransformed[i]);
		}

		Vector screen_points[8] = {};

		for (int i = 0; i < 8; i++) {
			if (!math::WorldToScreen(pointsTransformed[i], screen_points[i]))
				return rect;
		}

		auto left = screen_points[0].x;
		auto top = screen_points[0].y;
		auto right = screen_points[0].x;
		auto bottom = screen_points[0].y;

		for (int i = 1; i < 8; i++) {
			if (left > screen_points[i].x)
				left = screen_points[i].x;
			if (top < screen_points[i].y)
				top = screen_points[i].y;
			if (right < screen_points[i].x)
				right = screen_points[i].x;
			if (bottom > screen_points[i].y)
				bottom = screen_points[i].y;
		}
		return RECT{ (long)left, (long)top, (long)right, (long)bottom };
	}
}