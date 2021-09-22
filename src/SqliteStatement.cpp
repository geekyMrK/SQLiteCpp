#include "SqliteStatement.hpp"

template <typename PrepareFunction, typename CharacterSet>
void Sqlite::SqliteStatement::internalPrepare(const SqliteConnection &connection, const PrepareFunction prepare, const CharacterSet *const text) {
	if (SQLITE_OK != prepare(connection.getABI(), text, -1, statementHandle_.set(), nullptr)) {
		connection.throwLastError();
	}
}

void Sqlite::SqliteStatement::throwLastError() const {
	throw exception(sqlite3_db_handle(getABI()));
}

void Sqlite::SqliteStatement::prepare(const SqliteConnection &connection, const char *const characterSet) {
	internalPrepare(connection, sqlite3_prepare_v2, characterSet);
}

void Sqlite::SqliteStatement::prepare(const SqliteConnection &connection, const wchar_t *const characterSet) {
	internalPrepare(connection, sqlite3_prepare16_v2, characterSet);
}

bool Sqlite::SqliteStatement::execute() const {
	const int result = sqlite3_step(getABI());
	if (result == SQLITE_ROW)
		return true;
	else if (result == SQLITE_DONE)
		return false;
	else {
		throwLastError();
		//the control will never reach here, as exception will be thrown
		//added return false, just to remove warning: control may reach end of non-void function [-Wreturn-type]
		return false;
	}
}
