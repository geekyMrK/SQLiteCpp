#include "SqliteConnection.hpp"

template <typename Function, typename CharacterSet>
void Sqlite::SqliteConnection::internalOpen(Function openFunction, const CharacterSet *const filename) {
	
	SqliteConnection tempConnection;
	
	if (SQLITE_OK != openFunction(filename, tempConnection.connectionHandle_.set())) {
		tempConnection.throwLastError();
	}
	
	connectionHandle_.swap(tempConnection.connectionHandle_);
}

void Sqlite::SqliteConnection::throwLastError() const {
	throw exception(getABI());
}

void Sqlite::SqliteConnection::open(const char *const filename) {
	internalOpen(sqlite3_open, filename);
}

void Sqlite::SqliteConnection::open(const wchar_t *const filename) {
	internalOpen(sqlite3_open16, filename);
}

long long Sqlite::SqliteConnection::lastRowId() const noexcept {
        return sqlite3_last_insert_rowid(getABI());
}	

