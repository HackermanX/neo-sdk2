#include "esp.h"

void esp::on_cm(CUserCmd* cmd) {
	if (cmd->iButtons & IN_ATTACK)
		is_shooting = true;
	else is_shooting = false;
}

void esp::recoil_c() {
	auto weapon = g_LocalPlayer->m_hActiveWeapon();
	if (!weapon)
		return;

	if (weapon->is_grenade() || weapon->is_knife())
		return;

	int w, h;
	i::Engine->GetScreenSize(w, h);

	int x = w / 2;
	int y = h / 2;
	int dy = h / 97;
	int dx = w / 97;

	QAngle punch_angle = g_LocalPlayer->m_aimPunchAngle();
	x -= (dx * (punch_angle.y));
	y += (dy * (punch_angle.x));

	if (is_shooting && opt::esp::crosshair) {
		render::render_circle_filled(x, y, 2, 32, Color(255, 255, 255, 120));
	}
	else return;
}