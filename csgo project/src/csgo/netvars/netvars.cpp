#include "netvars.h"
#include "../other/xor.h"
#include "../interfaces.h"

void netvar_manager::init() {
	database.clear();
	for (auto clientclass = i::Client->GetAllClasses();
		clientclass != nullptr;
		clientclass = clientclass->pNext) {
		if (clientclass->pRecvTable) {
			database.emplace_back(load_table(clientclass->pRecvTable));
		}
	}
}

netvar_manager::netvar_table netvar_manager::load_table(RecvTable_t* recv_table) {
	auto table = netvar_table{};
	table.offset = 0;
	table.name = recv_table->szNetTableName;
	for (auto i = 0; i < recv_table->nProps; ++i) {
		auto prop = &recv_table->pProps[i];
		if (!prop || isdigit(prop->szVarName[0]))
			continue;
		if (strcmp(XorStr("baseclass"), prop->szVarName) == 0)
			continue;
		if (prop->iRecvType == DPT_DATATABLE && prop->pDataTable) {
			table.child_tables.emplace_back(load_table(prop->pDataTable));
			table.child_tables.back().offset = prop->iOffset;
			table.child_tables.back().prop = prop;
		}
		else {
			table.child_props.emplace_back(prop);
		}
	}
	return table;
}

uint32_t netvar_manager::get_offset(const std::string& tableName, const std::string& propName) {
	auto result = 0u;
	for (const auto& table : database) {
		if (table.name == tableName) {
			result = get_offset(table, propName);
			if (result != 0)
				return result;
		}
	}
	return 0;
}

uint32_t netvar_manager::get_offset(const netvar_table& table, const std::string& propName) {
	for (const auto& prop : table.child_props) {
		if (strncmp(prop->szVarName, propName.data(), propName.size()) == 0) {
			return table.offset + prop->iOffset;
		}
	}
	for (const auto& child : table.child_tables) {
		auto prop_offset = get_offset(child, propName);
		if (prop_offset != 0)
			return table.offset + prop_offset;
	}
	for (const auto& child : table.child_tables) {
		if (strncmp(child.prop->szVarName, propName.data(), propName.size()) == 0) {
			return table.offset + child.offset;
		}
	}
	return 0;
}