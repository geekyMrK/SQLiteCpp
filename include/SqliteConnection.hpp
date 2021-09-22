#include "UniqueHandle.hpp"
#include <sqlite3.h>

namespace Sqlite {

class SqliteConnection {
	
	struct SqliteConnectionTraits : public nullHandleTraits<sqlite3 *> {
		static void close(sqlite3 *value) noexcept {
			sqlite3_close(value);
		}
	};
	UniqueHandle<SqliteConnectionTraits> connectionHandle_;

	template <typename Function, typename CharacterSet>
	void internalOpen(Function openFunction, const CharacterSet *const filename);

  public:
	SqliteConnection() = default;

	template <typename CharacterSet>
	explicit sqliteConnection(const CharacterSet *const filename) {
		open(filename);
	}

	explicit operator bool() const noexcept {
		return static_cast<bool>(connectionHandle_);
	}
	
	static SqliteConnection memory() {
		return SqliteConnection(":memory:");
	}

	static SqliteConnection wideMemory() {
		return SqliteConnection(L":memory:");
	}

	sqlite3 *getABI() const noexcept {
		return connectionHandle_.get();
	}
	
	void throwLastError() const;

	void open(const char *const filename);

	void open(const wchar_t *const filename);
};

}
