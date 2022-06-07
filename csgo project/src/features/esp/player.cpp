#include "esp.h"

void esp::render() {
	if (i::Engine->IsInGame()) {
		listnr.loop([&](CBaseEntity* entity) {

			if (!entity)
				return;

			if (!entity->IsAlive() || entity->IsDormant())
				return;

			if (entity == g_LocalPlayer && !i::Input->bCameraInThirdPerson)
				return;

			//your stuff
			radar(entity);
			bt_dots(entity);
		});
	//==========================================================================
		recoil_c();
	}
}

void esp::radar(CBaseEntity* ent) {
	if (opt::esp::radar) {
		if (!ent->m_bSpotted()) {
			ent->m_bSpotted() = true;
		}
	}
}

void esp::bt_dots(CBaseEntity* ent) {
	auto records = &backtrack.data[ent->GetIndex()];

	if (!opt::esp::bt_dots)
		return;

	if (records->size() < 2)
		return;

	Vector previous_screenpos;

	for (int i = 1; i < backtrack.data[ent->GetIndex()].size(); i++) {//loop bt records
		Vector screen_pos;
		auto head = backtrack.data[ent->GetIndex()][i].hitboxPos;

		if (!math::WorldToScreen(head, screen_pos))
			continue;

		if (previous_screenpos.IsValid() && !previous_screenpos.IsZero()) {

			render::render_circle(screen_pos.x, screen_pos.y, 2, 32,
				(i == backtrack.selected_tick && ent->GetIndex() == backtrack.sel_ent_index) ? Color(255, 0, 0, 255) : Color(255, 255, 255, 255));

		} previous_screenpos = screen_pos;
	}
}