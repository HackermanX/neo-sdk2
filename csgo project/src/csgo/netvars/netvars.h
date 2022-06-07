#pragma once
#include <memory>
#include <iostream>
#include <unordered_map>
#include "../datatypes/datatable.h"

namespace netvar_manager
{
	struct netvar_table {
		std::string               name;
		RecvProp_t* prop;
		uint32_t                  offset;
		std::vector<RecvProp_t*>   child_props;
		std::vector<netvar_table> child_tables;
	};

	void init();
	netvar_table load_table(RecvTable_t* recv_table);
	uint32_t get_offset(const std::string& tableName, const std::string& propName);
	uint32_t get_offset(const netvar_table& table, const std::string& propName);

	static std::vector<netvar_table> database;
};