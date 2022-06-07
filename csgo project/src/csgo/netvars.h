#pragma once
#include "other/entity.h"
#include "netvars/netvars.h"
#include "other/math.h"

#define netvar(type, name, table, netvar)                           \
	type& name##() const {                                          \
		static int _##name = netvar_manager::get_offset(table, netvar);     \
		return *(type*)((uintptr_t)this + _##name);                 \
	}

#define custom_netvar(type, name, table, netvar, additional)        \
	type& name##() const {                                          \
		static int _##name = netvar_manager::get_offset(table, netvar);     \
		return *(type*)((uintptr_t)this + _##name + additional);    \
	}

#define offset( Type, szFunctionName, uOffset)														\
	[[nodiscard]] std::add_lvalue_reference_t<Type> szFunctionName()								\
	{																								\
		return *(std::add_pointer_t<Type>)(reinterpret_cast<std::uintptr_t>(this) + uOffset);	    \
	}

class CBaseEntity;

class CBaseCombatWeapon : public IClientEntity
{
public:
	netvar(float_t, m_flNextPrimaryAttack, "DT_BaseCombatWeapon", "m_flNextPrimaryAttack");
	netvar(float_t, m_flNextSecondaryAttack, "DT_BaseCombatWeapon", "m_flNextSecondaryAttack");
	netvar(int32_t, m_iClip1, "DT_BaseCombatWeapon", "m_iClip1");
	netvar(int32_t, m_iClip2, "DT_BaseCombatWeapon", "m_iClip2");
	netvar(float_t, m_flRecoilIndex, "DT_WeaponCSBase", "m_flRecoilIndex");
	netvar(int32_t, m_iViewModelIndex, "DT_BaseCombatWeapon", "m_iViewModelIndex");
	netvar(int32_t, m_iWorldModelIndex, "DT_BaseCombatWeapon", "m_iWorldModelIndex");
	netvar(int32_t, m_iWorldDroppedModelIndex, "DT_BaseCombatWeapon", "m_iWorldDroppedModelIndex");
	netvar(bool, m_bPinPulled, "DT_BaseCSGrenade", "m_bPinPulled");
	netvar(float_t, m_fThrowTime, "DT_BaseCSGrenade", "m_fThrowTime");
	netvar(float_t, m_flPostponeFireReadyTime, "DT_BaseCombatWeapon", "m_flPostponeFireReadyTime");
	netvar(CBaseHandle, m_hWeaponWorldModel, "DT_BaseCombatWeapon", "m_hWeaponWorldModel");
	netvar(short, m_iItemDefinitionIndex, "DT_BaseAttributableItem", "m_iItemDefinitionIndex");

	bool is_gun() {
		switch (i::WeaponSystem->GetWeaponData(this->m_iItemDefinitionIndex())->nWeaponType)
		{
		case WEAPONTYPE_C4:
			return false;
		case WEAPONTYPE_GRENADE:
			return false;
		case WEAPONTYPE_KNIFE:
			return false;
		default:
			return true;
		}
	}

	bool is_sniper() {
		if (i::WeaponSystem->GetWeaponData(this->m_iItemDefinitionIndex())->nWeaponType
			== WEAPONTYPE_SNIPER) return true;
		else return false;
	}

	bool is_reloading() {
		static auto inReload = *(uint32_t*)(memory::PatternScan(XorStr("client.dll"), XorStr("C6 87 ? ? ? ? ? 8B 06 8B CE FF 90")) + 2);
		return *(bool*)((uintptr_t)this + inReload);
	}

	bool has_bullets() {
		return !is_reloading() && m_iClip1() > 0;
	}

	bool is_rifle() {
		if (i::WeaponSystem->GetWeaponData(this->m_iItemDefinitionIndex())->nWeaponType
			== WEAPONTYPE_RIFLE) return true;
		else return false;
	}

	bool is_pistol() {
		if (i::WeaponSystem->GetWeaponData(this->m_iItemDefinitionIndex())->nWeaponType
			== WEAPONTYPE_PISTOL) return true;
		else return false;
	}

	bool is_smg() {
		if (i::WeaponSystem->GetWeaponData(this->m_iItemDefinitionIndex())->nWeaponType
			== WEAPONTYPE_SUBMACHINEGUN) return true;
		else return false;
	}

	bool is_machinegun() {
		if (i::WeaponSystem->GetWeaponData(this->m_iItemDefinitionIndex())->nWeaponType
			== WEAPONTYPE_MACHINEGUN) return true;
		else return false;
	}

	bool is_shotgun() {
		if (i::WeaponSystem->GetWeaponData(this->m_iItemDefinitionIndex())->nWeaponType
			== WEAPONTYPE_SHOTGUN) return true;
		else return false;
	}

	bool is_grenade() {
		if (i::WeaponSystem->GetWeaponData(this->m_iItemDefinitionIndex())->nWeaponType
			== WEAPONTYPE_GRENADE) return true;
		else return false;
	}

	bool is_knife() {
		if (i::WeaponSystem->GetWeaponData(this->m_iItemDefinitionIndex())->nWeaponType
			== WEAPONTYPE_KNIFE) return true;
		else return false;
	}

	int weapontype() {
		return i::WeaponSystem->GetWeaponData(this->m_iItemDefinitionIndex())->nWeaponType;
	}
};

class CBaseEntity : public IClientEntity
{
public:
	static __forceinline CBaseEntity* GetEntityByIndex(int index) {
		return static_cast<CBaseEntity*>(i::ClientEntityList->GetClientEntity(index));
	}
	static __forceinline CBaseEntity* get_entity_from_handle(CBaseHandle h) {
		return static_cast<CBaseEntity*>(i::ClientEntityList->GetClientEntityFromHandle(h));
	}

	netvar(int32_t, m_nModelIndex, "DT_BaseEntity", "m_nModelIndex");
	netvar(int32_t, m_iTeamNum, "DT_BaseEntity", "m_iTeamNum");
	netvar(Vector, m_vecOrigin, "DT_BaseEntity", "m_vecOrigin");
	netvar(Vector, m_vecAngles, "DT_BaseEntity", "m_vecAngles");
	netvar(bool, m_bShouldGlow, "DT_DynamicProp", "m_bShouldGlow");
	netvar(CBaseHandle, m_hOwnerEntity, "DT_BaseEntity", "m_hOwnerEntity");
	netvar(bool, m_bSpotted, "DT_BaseEntity", "m_bSpotted");
	netvar(float_t, m_flC4Blow, "DT_PlantedC4", "m_flC4Blow");
	netvar(int, m_lifeState, "DT_BasePlayer", "m_lifeState");
	netvar(int32_t, m_iHealth, "DT_BasePlayer", "m_iHealth");

	inline bool IsAlive() {
		return (this->m_lifeState() == LIFE_ALIVE && this->m_iHealth() > 0) ? true : false;
	}

	inline bool IsPlayer() {
		return memory::CallVFunc<bool>(this, 158);
	}

	const matrix3x4_t& m_rgflCoordinateFrame()
	{
		static auto _m_rgflCoordinateFrame = netvar_manager::get_offset("DT_BaseEntity", "m_CollisionGroup") - 0x30;
		return *(matrix3x4_t*)((uintptr_t)this + _m_rgflCoordinateFrame);
	}
};


class CBasePlayer : public CBaseEntity
{
public:
	static __forceinline CBasePlayer* GetLocalPlayer(const bool set = false)
	{
		static CBasePlayer* local = nullptr;

		if (!local || set)
			local = GetPlayerByIndex(i::Engine->GetLocalPlayer());

		return local;
	}//new local player getting method !??!?!??!
	//https://www.unknowncheats.me/forum/2360808-post9.html

	static __forceinline CBasePlayer* GetPlayerByUserId(int id)
	{
		return static_cast<CBasePlayer*>(GetEntityByIndex(i::Engine->GetPlayerForUserID(id)));
	}
	static __forceinline CBasePlayer* GetPlayerByIndex(int i)
	{
		return static_cast<CBasePlayer*>(GetEntityByIndex(i));
	}

	netvar(bool, m_bHasDefuser, "DT_CSPlayer", "m_bHasDefuser");
	netvar(bool, m_bGunGameImmunity, "DT_CSPlayer", "m_bGunGameImmunity");
	netvar(int32_t, m_iShotsFired, "DT_CSPlayer", "m_iShotsFired");
	netvar(QAngle, m_angEyeAngles, "DT_CSPlayer", "m_angEyeAngles[0]");
	netvar(int, m_ArmorValue, "DT_CSPlayer", "m_ArmorValue");
	netvar(bool, m_bHasHeavyArmor, "DT_CSPlayer", "m_bHasHeavyArmor");
	netvar(bool, m_bHasHelmet, "DT_CSPlayer", "m_bHasHelmet");
	netvar(bool, m_bIsScoped, "DT_CSPlayer", "m_bIsScoped");;
	netvar(float, m_flLowerBodyYawTarget, "DT_CSPlayer", "m_flLowerBodyYawTarget");
	netvar(int32_t, m_iHealth, "DT_BasePlayer", "m_iHealth");
	netvar(int32_t, m_lifeState, "DT_BasePlayer", "m_lifeState");
	netvar(int32_t, m_fFlags, "DT_BasePlayer", "m_fFlags");
	netvar(int32_t, m_nTickBase, "DT_BasePlayer", "m_nTickBase");
	netvar(Vector, m_vecViewOffset, "DT_BasePlayer", "m_vecViewOffset[0]");
	netvar(QAngle, m_viewPunchAngle, "DT_BasePlayer", "m_viewPunchAngle");
	netvar(QAngle, m_aimPunchAngle, "DT_BasePlayer", "m_aimPunchAngle");
	netvar(CBaseHandle, m_hViewModel, "DT_BasePlayer", "m_hViewModel[0]");
	netvar(Vector, m_vecVelocity, "DT_BasePlayer", "m_vecVelocity[0]");
	netvar(float, m_flMaxspeed, "DT_BasePlayer", "m_flMaxspeed");
	netvar(CBaseHandle, m_hObserverTarget, "DT_BasePlayer", "m_hObserverTarget");
	netvar(float, m_flFlashMaxAlpha, "DT_CSPlayer", "m_flFlashMaxAlpha");
	netvar(int32_t, m_nHitboxSet, "DT_BaseAnimating", "m_nHitboxSet");
	netvar(CBaseHandle, m_hActiveWeapon2, "DT_BaseCombatCharacter", "m_hActiveWeapon");
	netvar(int32_t, m_iAccount, "DT_CSPlayer", "m_iAccount");
	netvar(float, m_flFlashDuration, "DT_CSPlayer", "m_flFlashDuration");
	netvar(float, m_flSimulationTime, "DT_BaseEntity", "m_flSimulationTime");
	netvar(float, m_flCycle, "DT_BaseAnimating", "m_flCycle");
	netvar(int, m_nSequence, "DT_BaseViewModel", "m_nSequence");
	netvar(float, m_flNextAttack, "DT_BaseCombatCharacter", "m_flNextAttack");
	netvar(QAngle, m_angAbsAngles, "DT_BaseEntity", "m_angAbsAngles");
	netvar(Vector, m_angAbsOrigin, "DT_BaseEntity", "m_angAbsOrigin");
	netvar(float, m_flDuckSpeed, "DT_BasePlayer", "m_flDuckSpeed");
	netvar(float, m_flDuckAmount, "DT_BasePlayer", "m_flDuckAmount");
	netvar(int, m_nNextThinkTick, "DT_BasePlayer", "m_nNextThinkTick");
	netvar(int32_t, m_nSurvivalTeam, "DT_CSPlayer", "m_nSurvivalTeam");
	netvar(Vector, m_vecPlayerPatchEconIndices, "DT_CSPlayer", "m_vecPlayerPatchEconIndices");

	custom_netvar(CUserCmd*, GetCurrentCommand, "DT_BasePlayer", "m_hConstraintEntity", -0xC);

	offset(CUserCmd, GetLastCommand, 0x3288);
	offset(int, GetButtonDisabled, 0x3330);
	offset(int, GetButtonForced, 0x3334);

	inline bool IsAlive()
	{
		return (this->m_lifeState() == LIFE_ALIVE && this->m_iHealth() > 0) ? true : false;
	}
	DataMap_t* GetPredDescMap()
	{
		typedef DataMap_t* (__thiscall* o_GetPredDescMap)(void*);
		return memory::CallVFunc<o_GetPredDescMap>(this, 17)(this);
	}
	int& m_nImpulse()
	{
		static unsigned int impulse = memory::FindInDataMap(GetPredDescMap(), XorStr("m_nImpulse"));
		return *(int*)((uintptr_t)this + impulse);
	}
	int& m_nButtons()
	{
		static unsigned int buttons = memory::FindInDataMap(GetPredDescMap(), XorStr("m_nButtons"));
		return *(int*)((uintptr_t)this + buttons);
	}
	int& m_afButtonLast()
	{
		static unsigned int buttons = memory::FindInDataMap(GetPredDescMap(), XorStr("m_afButtonLast"));
		return *(int*)((uintptr_t)this + buttons);
	}
	int& m_afButtonPressed()
	{
		static unsigned int buttons = memory::FindInDataMap(GetPredDescMap(), XorStr("m_afButtonPressed"));
		return *(int*)((uintptr_t)this + buttons);
	}
	int& m_afButtonReleased()
	{
		static unsigned int buttons = memory::FindInDataMap(GetPredDescMap(), XorStr("m_afButtonReleased"));
		return *(int*)((uintptr_t)this + buttons);
	}
	bool PhysicsRunThink(int nThinkMethod)
	{
		// @xref: from sub with "CLIENT:  %s(%s) thinking for %.02f ms!!!\n"
		using PhysicsRunThinkFn = bool(__thiscall*)(void*, int);
		static auto oPhysicsRunThink = (PhysicsRunThinkFn)memory::PatternScan(XorStr("client.dll"), XorStr("55 8B EC 83 EC 10 53 56 57 8B F9 8B 87"));
		return oPhysicsRunThink(this, nThinkMethod);
	}
	void PreThink()
	{
		memory::CallVFunc<void>(this, 318);
	}
	void Think()
	{
		memory::CallVFunc<void>(this, 139);
	}
	void PstThink()
	{
		memory::CallVFunc<void>(this, 319);
	}
	void UpdateCollisionBounds()
	{
		memory::CallVFunc<void>(this, 340);
	}
	void StudioFrameAdvance()
	{
		memory::CallVFunc<void>(this, 220);
	}
	bool is_not_target()
	{
		if (!this || this == GetLocalPlayer(true))
			return true;

		if (m_iHealth() <= 0)
			return true;

		if (m_bGunGameImmunity())
			return true;

		if (m_fFlags() & FL_FROZEN)
			return true;

		int entIndex = GetIndex();
		return entIndex > i::Globals->nMaxClients;
	}
	bool in_dangerzone()
	{
		static auto game_type = i::ConVar->FindVar(XorStr("game_type"));
		return game_type->GetInt() == 6;
	}
	bool is_enemy()
	{
		const static auto mp_teammates_are_enemies = i::ConVar->FindVar(XorStr("mp_teammates_are_enemies"));

		if (mp_teammates_are_enemies->GetInt())
			return this != GetLocalPlayer(true);

		if (in_dangerzone())
		{
			return this->m_nSurvivalTeam() != GetLocalPlayer(true)->m_nSurvivalTeam() || GetLocalPlayer(true)->m_nSurvivalTeam() == -1;
		}
		else
		{
			return this->m_iTeamNum() != GetLocalPlayer(true)->m_iTeamNum();
		}
	}
	void invalidate_bcache()
	{
		static DWORD addr = (DWORD)memory::PatternScan((XorStr("client.dll")), XorStr("80 3D ? ? ? ? ? 74 16 A1 ? ? ? ? 48 C7 81"));

		*(int*)((uintptr_t)this + 0xA30) = i::Globals->iFrameCount; //we'll skip occlusion checks now
		*(int*)((uintptr_t)this + 0xA28) = 0;//clear occlusion flags

		unsigned long g_iModelBoneCounter = **(unsigned long**)(addr + 10);
		*(unsigned int*)((DWORD)this + 0x2924) = 0xFF7FFFFF; // m_flLastBoneSetupTime = -FLT_MAX;
		*(unsigned int*)((DWORD)this + 0x2690) = (g_iModelBoneCounter - 1); // m_iMostRecentModelBoneCounter = g_iModelBoneCounter - 1;
	}
	Vector eye_pos()
	{
		return m_vecOrigin() + m_vecViewOffset();
	}
	CBaseCombatWeapon* m_hActiveWeapon()
	{
		return i::ClientEntityList->Get<CBaseCombatWeapon>(this->m_hActiveWeapon2());
	}
	Vector hitbox_pos(int hitbox_id)
	{
		matrix3x4_t boneMatrix[MAXSTUDIOBONES];

		if (SetupBones(boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0.0f)) {
			auto studio_model = i::ModelInfo->GetStudioModel(GetModel());
			if (studio_model) {
				auto hitbox = studio_model->GetHitboxSet(0)->GetHitbox(hitbox_id);
				if (hitbox) {
					auto
						min = Vector{},
						max = Vector{};

					math::vector_transform(hitbox->vecBBMin, boneMatrix[hitbox->iBone], min);
					math::vector_transform(hitbox->vecBBMax, boneMatrix[hitbox->iBone], max);

					return (min + max) / 2.0f;
				}
			}
		}
		return Vector{};
	}
	bool can_see_player(CBasePlayer* player, const Vector& pos)
	{
		Trace_t trace = { };
		Ray_t ray(eye_pos(), pos);
		CTraceFilter filter(this);
		i::EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &trace);

		return trace.pHitEntity == player || trace.IsVisible();
	}
	int m_nMoveType()
	{
		return *(int*)((uintptr_t)this + 0x25C);
	}
};

inline CBasePlayer* g_LocalPlayer;//funny