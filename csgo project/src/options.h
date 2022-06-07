#pragma once
#include <map>
#include "csgo/datatypes/color.h"

namespace opt {
	namespace bt {
		inline bool toggle = false;
		inline int time = 0;
		inline int fake = 0;
	}
	namespace move {
		inline bool bhop = false;
	}
	namespace chams {
		inline bool toggle = false;
		inline bool xqz = false;
		inline bool bt = false;
		inline float visible[4] = { 0 / 255.f, 255 / 255.f, 255 / 255.f, 1.f};
		inline float hidden[4] = { 0 / 255.f, 128 / 255.f, 128 / 255.f, 1.f };
	}
	namespace misc {

	}
	namespace esp {
		inline bool crosshair = false;
		inline bool radar = false;
		inline bool bt_dots = false;
	}
}