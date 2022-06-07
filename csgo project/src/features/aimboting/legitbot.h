#pragma once
#include "../../commonIncludes.h"

class aimbot {
public:

private:
	int WeaponID = -1;
};
extern aimbot g_aim;

struct LBotWeaponSetting
{
	bool rcs;
	int rcs_v;
};

enum WeaponType
{
	mWEAPON_PISTOL = 0,
	mWEAPON_DEAGLE = 1,
	mWEAPON_REVOLVER = 2,
	mWEAPON_SMG = 3,
	mWEAPON_SHOTGUN = 4,
	mWEAPON_MACHINEGUN = 5,
	mWEAPON_RIFLE = 6,
	mWEAPON_SSG08 = 7,
	mWEAPON_AWP = 8,
	mWEAPON_AUTO = 9,
	mWEAPON_SNIPER = 10
};

namespace legit
{
	inline LBotWeaponSetting set[11] = { 0 };

	inline int GetWeaponType(CBaseCombatWeapon* weapon)
	{
		if (weapon->m_iItemDefinitionIndex() == WEAPON_DEAGLE)
			return WeaponType::mWEAPON_DEAGLE;
		else if (weapon->m_iItemDefinitionIndex() == WEAPON_SSG08)
			return WeaponType::mWEAPON_SSG08;
		else if (weapon->is_smg())
			return WeaponType::mWEAPON_SMG;
		else if (weapon->is_machinegun())
			return WeaponType::mWEAPON_MACHINEGUN;
		else if (weapon->is_rifle())
			return WeaponType::mWEAPON_RIFLE;
		else if (weapon->is_pistol())
			return WeaponType::mWEAPON_PISTOL;
		else if (weapon->is_shotgun())
			return WeaponType::mWEAPON_SHOTGUN;
		else if (weapon->is_sniper())
			return WeaponType::mWEAPON_SNIPER;
		else
			return WeaponType::mWEAPON_SHOTGUN;
	}
}