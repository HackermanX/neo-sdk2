#pragma once

class IWeaponSystem
{
public:
	CCSWeaponData* GetWeaponData(short nItemDefinitionIndex)
	{
		return memory::CallVFunc<CCSWeaponData*>(this, 2, nItemDefinitionIndex);
	}
};
