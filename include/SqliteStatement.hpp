#include "SqliteConnection.hpp"

namespace Sqlite{
  template <typename T>
  struct sqliteReader {
	  
       int getInt(const int columnNum = 0) const noexcept {
		return sqlite3_column_int(static_cast<const T *>(this)->getABI(), columnNum);
	}

	const char *getString(const int columnNum = 0) const noexcept {
		return reinterpret_cast<const char *>(sqlite3_column_text(static_cast<const T *>(this)->getABI(), columnNum));
	}

	const wchar_t *getWideString(const int columnNum = 0) const noexcept {
		return static_cast<const wchar_t *>(sqlite3_column_text16(static_cast<const T *>(this)->getABI(), columnNum));
	}

	int getStringLength(const int columnNum) const ;
	
	int getWideStringLength(const int columnNum) const noexcept ;

  };
	
  class SqliteStatement : public sqliteReader<SqliteStatement> {
	
	struct SqliteStatementTraits {
		static void close(sqlite3_stmt *value) noexcept {
			sqlite3_finalize(value);
		}
	};

	UniqueHandle<SqliteStatementTraits> statementHandle_;

	template <typename PrepareFunction, typename CharacterSet, typename... VALUES>
	void internalPrepare(const SqliteConnection &connection, const PrepareFunction prepare, const CharacterSet *const text, VALUES &&... values) ;
	
	void internalBindAll(const int) const noexcept{
	}
	
	template <typename FIRST, typename... REST_VALUES>
	void internalBindAll(const int index, FIRST &&first, REST_VALUES &&... restValues) const ;

  public:
	SqliteStatement() = default;
	  
	template <typename CharacterSet, typename... VALUES>
	SqliteStatement(const SqliteConnection &connection, const CharacterSet *const text, VALUES &&... values) {
		prepare(connection, text, std::forward<VALUES>(values)...);
	}
	  
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
	  
	void bind(const int index, const int value) const ;
	
	void bind(const int index, const char *const strValue, const int size ) const ;
	
	void bind(const int index, const wchar_t *const strValue, const int size ) const ;
	
	void bind(const int index, const std::string &strValue) const ;
	
	void bind(const int index, const std::wstring &strValue) const ;
	
	void bind(const int index, const std::string &&strValue) const ;
	
	void bind(const int index, const std::wstring &&strValue) const ;

	template <typename... Values>
	void bindAll(Values &&... values) const ;
	
	template <typename ...Values>
	void reset (Values&&... values) ;
	  

  };
	
  template <typename CharacterSet, typename... Values>
  void sqliteExecute(const SqliteConnection &connection, const CharacterSet *const text, Values &&... values) ;


}
	
