#include "SqliteStatement.hpp"

template <typename T>
int Sqlite::sqliteReader<T>::getStringLength(const int columnNum) const noexcept {
        return sqlite3_column_bytes(static_cast<const T *>(this)->getABI(), columnNum);
}
	
template <typename T>
int Sqlite::sqliteReader<T>::getWideStringLength(const int columnNum) const noexcept
{
	return (sqlite3_column_bytes16(static_cast<const T *>(this)->getABI(), columnNum) / sizeof(wchar_t));
}



void Sqlite::SqliteStatement::throwLastError() const {
	throw exception(sqlite3_db_handle(getABI()));
}

template <typename... Values>
void Sqlite::SqliteStatement::reset(Values &&... values) {
	if (SQLITE_OK != sqlite3_reset(getABI())) {
		throwLastError();
	}
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

void Sqlite::SqliteStatement::bind(const int index, const int value) const {
	if (SQLITE_OK != sqlite3_bind_int(getABI(), index, value)) {
		throwLastError();
	}
}

void Sqlite::SqliteStatement::bind(const int index, const char *const strValue, const int size = -1) const {
	if (SQLITE_OK != sqlite3_bind_text(getABI(), index, strValue, size, SQLITE_STATIC)) {
		throwLastError();
	}
}

void Sqlite::SqliteStatement::bind(const int index, const wchar_t *const strValue, const int size = -1) const {
	if (SQLITE_OK != sqlite3_bind_text16(getABI(), index, strValue, size, SQLITE_STATIC)) {
		throwLastError();
	}
}

void Sqlite::SqliteStatement::bind(const int index, const std::string &strValue) const {
	bind(index, strValue.c_str(), static_cast<int>(strValue.size()));
}

void Sqlite::SqliteStatement::bind(const int index, const std::wstring &strValue) const {
	bind(index, strValue.c_str(), static_cast<int>((strValue.size() * sizeof(wchar_t))));
}

void Sqlite::SqliteStatement::bind(const int index, const std::string &&strValue) const {
	if (SQLITE_OK != sqlite3_bind_text(getABI(), index, strValue.c_str(), static_cast<int>(strValue.size()), SQLITE_TRANSIENT)) {
		throwLastError();
	}
}

void Sqlite::SqliteStatement::bind(const int index, const std::wstring &&strValue) const {
	if (SQLITE_OK != sqlite3_bind_text16(getABI(), index, strValue.c_str(), static_cast<int>((strValue.size() * sizeof(wchar_t))), SQLITE_TRANSIENT)) {
		throwLastError();
	}
}
