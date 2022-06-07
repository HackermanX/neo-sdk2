#pragma once
// used: color
#include "datatypes/color.h"
// used: utlvector
#include "datatypes/utlvector.h"
#include "memory.h"

using FnCommandCallbackV1_t = void(__cdecl*)();
using FnChangeCallback_t = void(__cdecl*)(void*, const char*, float);
class CConVar
{
public:
	const char* GetName()
	{
		return memory::CallVFunc<const char*>(this, 5);
	}

	float GetFloat() // idx @ 11
	{
		std::uint32_t uXored = *reinterpret_cast<std::uint32_t*>(&pParent->flValue) ^ reinterpret_cast<std::uint32_t>(this);
		return *reinterpret_cast<float*>(&uXored);
	}

	int GetInt() // idx @ 12
	{
		return static_cast<int>(pParent->iValue ^ reinterpret_cast<int>(this));
	}

	bool GetBool() // idx @ 13
	{
		return !!GetInt();
	}

	const char* GetString() const
	{
		char const* szValue = pParent->szString;
		return szValue ? szValue : "";
	}

	void SetValue(const char* szValue)
	{
		return memory::CallVFunc<void>(this, 14, szValue);
	}

	void SetValue(float flValue)
	{
		return memory::CallVFunc<void>(this, 15, flValue);
	}

	void SetValue(int iValue)
	{
		return memory::CallVFunc<void>(this, 16, iValue);
	}

	void SetValue(Color colValue)
	{
		return memory::CallVFunc<void>(this, 17, colValue);
	}

public:
	std::byte						pad0[0x4];			//0x00
	CConVar* pNext;				//0x04
	bool							bRegistered;		//0x08
	const char* szName;				//0x0C
	const char* szHelpString;		//0x10
	int								nFlags;				//0x14
	FnCommandCallbackV1_t			pCallback;			//0x18
	CConVar* pParent;			//0x1C
	const char* szDefaultValue;		//0x20
	char* szString;			//0x24
	int								iStringLength;		//0x28
	float							flValue;			//0x2C
	int								iValue;				//0x30
	bool							bHasMin;			//0x34
	float							flMinValue;			//0x38
	bool							bHasMax;			//0x3C
	float							flMaxValue;			//0x40
	CUtlVector<FnChangeCallback_t>	fnChangeCallbacks;	//0x44
};