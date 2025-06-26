#include "ormcxx_sqlite.hpp"
#include <sqlite3.h>

namespace ormcxx {
	Sqlite3Db::~Sqlite3Db() {
		close();
	}

	void Sqlite3Db::open(const std::string& connInfo) {
		sqlite3_open(connInfo.c_str(), &hDb);
	}

	void Sqlite3Db::close() {
		if (hDb != nullptr) {
			int r = sqlite3_close(hDb);
		}

	}

	Sqlite3Stmt Sqlite3Db::prepare(const std::string& sql_string) {
		Sqlite3Stmt stmt(hDb, sql_string);
		return stmt;
	}

	Sqlite3Stmt::Sqlite3Stmt(sqlite3* db, const std::string& sql_string) : result(this) {
		prepare_rc = sqlite3_prepare_v2(db, sql_string.data(), sql_string.length(), &stmt, nullptr);
	}
	Sqlite3Stmt ::~Sqlite3Stmt() { sqlite3_finalize(stmt); }

	tl::expected<sql_result*, int> Sqlite3Stmt::execute() {

		if (prepare_rc != SQLITE_OK) {
			return tl::make_unexpected(prepare_rc);
		}
		else {

			return &result;
		}
	}
}
