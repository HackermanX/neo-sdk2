#include "backtrack.h"

float calc_fov(QAngle viewAngle, QAngle aimAngle) {
	QAngle delta = aimAngle - viewAngle;
	math::fix_angles(delta);
	return sqrtf(powf(delta.x, 2.0f) + powf(delta.y, 2.0f));
}

void Backtrack::setup_records() {
	CBasePlayer* player;

	for (int i = 1; i <= i::Globals->nMaxClients; ++i) {
		player = CBasePlayer::GetPlayerByIndex(i);

		if (player->is_not_target()) {
			if (data.count(i) > 0)
				data.erase(i);
			continue;
		}

		if (!player->is_enemy()) {
			if (data.count(i) > 0)
				data.erase(i);
			continue;
		}

		auto& cur_data = data[i];
		if (!cur_data.empty()) {
			auto& front = cur_data.front();
			if (front.simTime == player->m_flSimulationTime())
				continue;

			while (!cur_data.empty()) {
				auto& back = cur_data.back();
				float delta_time = std::clamp(correct_time, 0.f, cvar::sv_maxunlag) - (i::Globals->flCurrentTime - back.simTime);
				if (std::fabsf(delta_time) <= (opt::bt::time / 1000.f))
					break;

				cur_data.pop_back();
			}
		}

		auto model = player->GetModel();
		if (!model)
			continue;

		auto hdr = i::ModelInfo->GetStudioModel(model);
		if (!hdr)
			continue;

		auto hitbox_set = hdr->GetHitboxSet(player->m_nHitboxSet());
		auto hitbox_head = hitbox_set->GetHitbox(HITBOX_HEAD);
		auto hitbox_center = (hitbox_head->vecBBMin + hitbox_head->vecBBMax) * 0.5f;

		backtrack_data bd;
		bd.hitboxset = hitbox_set;
		bd.simTime = player->m_flSimulationTime();
		bd.player = player;

		*(Vector*)((uintptr_t)player + 0xA0) = player->m_vecOrigin();
		*(int*)((uintptr_t)player + 0xA68) = 0;
		*(int*)((uintptr_t)player + 0xA30) = 0;

		player->invalidate_bcache();
		player->SetupBones(bd.boneMatrix, 256, 0x7FF00, i::Globals->flCurrentTime);

		math::vector_transform(hitbox_center, bd.boneMatrix[hitbox_head->iBone], bd.hitboxPos);
		data[i].push_front(bd);
	}
}

void Backtrack::on_move(CUserCmd* pCmd) {
	if (!i::Engine->IsInGame() || !g_LocalPlayer || !g_LocalPlayer->IsAlive()) {
		data.clear();
		return;
	}

	auto pWeapon = g_LocalPlayer->m_hActiveWeapon();
	if (!pWeapon) {
		data.clear();
		return;
	}

	if (!opt::bt::toggle) {
		data.clear();
		return;
	}

	lerp_time = std::fmaxf(cvar::cl_interp, cvar::cl_interp_ratio / cvar::updateRate);
	latency = i::Engine->GetNetChannelInfo()->GetLatency(FLOW_INCOMING) + i::Engine->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING);
	correct_time = latency + lerp_time;

	Vector l_eye_pos = g_LocalPlayer->eye_pos();
	QAngle angles;
	int tick_count = -1;
	float best_fov = 255.0f;

	for (auto& node : data) {
		auto& cur_data = node.second;
		if (cur_data.empty())
			continue;

		for (int i = 0; i < cur_data.size(); i++) {
			auto bd = cur_data[i];
			float delta_time = std::clamp(correct_time, 0.f, cvar::sv_maxunlag) - (i::Globals->flCurrentTime - bd.simTime);

			if (std::fabsf(delta_time) > 0.2f)
				continue;

			math::vector_angles(bd.hitboxPos - l_eye_pos, angles);
			math::fix_angles(angles);

			float fov = calc_fov(pCmd->angViewPoint, angles);

			if (best_fov > fov) {
				best_fov = fov;
				tick_count = TIME_TO_TICKS(bd.simTime + lerp_time);
				selected_tick = i;
				sel_ent_index = bd.player->GetIndex();
			}
		}
	}
	if (tick_count != -1) {
		pCmd->iTickCount = tick_count;
	}
}
Backtrack backtrack;

void Fakeping::update_sequence() {
	auto net_channel = i::ClientState->pNetChannel;
	if (net_channel == nullptr)
		return;
	// set to real sequence to update, otherwise needs time to get it work again
	if (m_last_incoming_sequence_number == 0)
		m_last_incoming_sequence_number = net_channel->iInSequenceNr;
	// check how much sequences we can spike
	if (net_channel->iInSequenceNr > m_last_incoming_sequence_number) {
		net_channel->iInSequenceNr = net_channel->iInSequenceNr;
		sequences.emplace_front(incoming_sequence_t(net_channel->iInReliableState, net_channel->iOutReliableState, net_channel->iInSequenceNr, i::Globals->flRealTime));
	}
	// is cached too much sequences
	if (sequences.size() > 2048U)
		sequences.pop_back();
}

void Fakeping::clear_sequence() {
	if (!sequences.empty())
	{
		m_last_incoming_sequence_number = 0;
		sequences.clear();
	}
}

void Fakeping::add_latency(INetChannel* net_channel) {
	for (auto& seq : sequences) {
		if (i::Globals->flRealTime - seq.m_current_time >= (opt::bt::fake / 1000.f)) {
			net_channel->iInReliableState = seq.m_in_reliable_state;
			net_channel->iInSequenceNr = seq.m_sequence_nr;
			break;
		}
	}
}

Fakeping fakeping;