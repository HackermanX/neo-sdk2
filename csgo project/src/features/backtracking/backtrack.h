#pragma once
#include "../../commonIncludes.h"
#include <map>
#include <deque>

struct backtrack_data {
	CBasePlayer* player;
	float simTime;
	mstudiohitboxset_t* hitboxset;
	Vector hitboxPos;
	matrix3x4_t boneMatrix[256];
};

class Backtrack {
public:
	void setup_records();
	void on_move(CUserCmd* pCmd);
	float correct_time = 0.0f;
	float latency = 0.0f;
	float lerp_time = 0.0f;
	int selected_tick = 0;
	int sel_ent_index = 0;
	std::map<int, std::deque<backtrack_data>> data;
};
extern Backtrack backtrack;

class Fakeping {
private:
	struct incoming_sequence_t {
		incoming_sequence_t(int instate, int outstate, int seqnr, float time) {
			m_in_reliable_state = instate;
			m_out_reliable_state = outstate;
			m_sequence_nr = seqnr;
			m_current_time = time;
		}
		int m_in_reliable_state;
		int m_out_reliable_state;
		int m_sequence_nr;
		float m_current_time;
	};
	std::deque< incoming_sequence_t > sequences;
	int m_last_incoming_sequence_number;
public:
	bool flipped_state = false;
	void update_sequence();
	void clear_sequence();
	void add_latency(INetChannel* net_channel);
};

extern Fakeping fakeping;