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

  public:
	SqliteConnection() = default;

	explicit operator bool() const noexcept {
		return static_cast<bool>(connectionHandle_);
	}
};

}
