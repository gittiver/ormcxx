#include "ormcxx/ormcxx_sql_config.hpp"
namespace ormcxx {


	std::string sql_type(const std::type_info& info) {
		if (info == typeid(std::string)) {
			return "VARCHAR";
		}
		else if (info == typeid(int)) {
			return "INTEGER";
		}
		else {
			return "";
		}
	}

}