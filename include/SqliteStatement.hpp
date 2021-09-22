#include "SqliteConnection.hpp"

namespace Sqlite{
  
  class SqliteStatement {
	
	struct SqliteStatementTraits {
		static void close(sqlite3_stmt *value) noexcept {
			sqlite3_finalize(value);
		}
	};

	UniqueHandle<SqliteStatementTraits> statementHandle_;

	template <typename PrepareFunction, typename CharacterSet>
	void internalPrepare(const SqliteConnection &connection, const PrepareFunction prepare, const CharacterSet *const text) ;

  public:
	SqliteStatement() = default;
	
	explicit operator bool() const noexcept {
		return static_cast<bool>(statementHandle_);
	}

	sqlite3_stmt *getABI() const noexcept {
		return statementHandle_.get();
	}

	void throwLastError() const;

	void prepare(const SqliteConnection &connection, const char *const characterSet);
	void prepare(const SqliteConnection &connection, const wchar_t *const characterSet) ;

	bool execute() const ;  

  };
  
}
	
