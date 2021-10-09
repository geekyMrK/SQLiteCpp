#include "SqliteStatement.hpp"

namespace Sqlite {

class SqliteRow : public sqliteReader<SqliteRow> {
	sqlite3_stmt *statement_{nullptr};

  public:
  
	constexpr sqlite3_stmt *getABI() const noexcept {
		return statement_;
	}
	
	SqliteRow(sqlite3_stmt *const statement) noexcept : statement_{statement} {
	}
};


class rowIterator {
	
	const SqliteStatement const *statement_{nullptr};

  public:
	rowIterator() noexcept = default;
	rowIterator(const SqliteStatement &statement) {
		if (statement.execute()) {
			statement_ = &statement;
		}
	}
	
	rowIterator &operator++() ;

	constexpr bool operator!=(const rowIterator &other) const noexcept {
		return statement_ != other.statement_;
	}

	SqliteRow operator*() const noexcept ;
};


inline rowIterator begin(const SqliteStatement &statement) noexcept {
	return rowIterator(statement);
}

constexpr inline rowIterator end(const SqliteStatement &) noexcept {
	return rowIterator();
}


}
