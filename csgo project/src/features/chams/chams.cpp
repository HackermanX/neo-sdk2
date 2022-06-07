#include "chams.h"
#include "../../hooks/hooking.h"
#include "../backtracking/backtrack.h"

chams::chams() {
}

chams::~chams() {
}

void override_mat(int mat, bool ignorez, const Color& rgba) {
	if (mat == 0) return;

	IMaterial* material = nullptr;

	switch (mat) {
	case 1:
		material = i::MaterialSystem->FindMaterial(XorStr("debug/debugtranslucentsinglecolor"), TEXTURE_GROUP_MODEL, true, 0);
		break;
	}

	material->ColorModulate(rgba[0] / 255.f, rgba[1] / 255.f, rgba[2] / 255.f);
	material->AlphaModulate(rgba[3] / 255.f);
	material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, ignorez);

	if (material == nullptr)
		material->IncrementReferenceCount();

	i::ModelRender->ForcedMaterialOverride(material);

	bool found;
	auto color = material->FindVar("$envmaptint", &found);

	if (found) color->SetVector(rgba[0], rgba[1], rgba[2]);
}

void chams::on_dme(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* bone) {
	using fn = void(__thiscall*)(IVModelRender*, IMatRenderContext*, const DrawModelState_t*, const ModelRenderInfo_t*, matrix3x4_t*);
	static auto original = hooks::draw_model_execute.get_original<fn>();
	const char* model_name = info.pModel->szName;

	CBasePlayer* player = CBasePlayer::GetPlayerByIndex(info.nEntityIndex);

	if (info.nEntityIndex > 0 && info.nEntityIndex <= 64) {//player looptie loop
		if (player && g_LocalPlayer && player->is_enemy()) {//santity checks
			if (opt::chams::toggle) {
				if (opt::bt::toggle && opt::chams::bt) {
					for (int i = 1; i < backtrack.data[player->GetIndex()].size(); i++) {//loop bt records
						if (i == backtrack.selected_tick) {//nasa

							int green = int(i * 3 * 2.55f);//no questions pls
							int red = 255 - green;

							override_mat(1, false, Color(red, green, 100, 90));

							original(i::ModelRender, ctx, &state, &info, backtrack.data[player->GetIndex()][i].boneMatrix);
						}
					}
				}
				if (opt::chams::xqz) {
					//currently only xqz chams
					override_mat(1, true, Color(opt::chams::hidden));

					original(i::ModelRender, ctx, &state, &info, bone);
				}
				override_mat(1, false, Color(opt::chams::visible));
			}
		}
	}
}

void chams::rem_p() {
	if (i::Engine->IsConnected()) {
		for (int i = 1; i <= i::Globals->nMaxClients; i++) {
			CBasePlayer* ent = (CBasePlayer*)(i::ClientEntityList->GetClientEntity(i));
			if (!ent || !ent->IsAlive())
				continue;
			for (size_t y = 0; y < 5; y++) {
				ent->m_vecPlayerPatchEconIndices()[y] = NULL;
			}
		}
	}
}

chams g_chams;