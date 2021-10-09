#include "SQLiteWrapper.hpp"
	
Sqlite::rowIterator& Sqlite::rowIterator::operator++() {
	if (!statement_->execute()) {
		statement_ = nullptr;
	}
	return *this;
}

Sqlite::SqliteRow Sqlite::rowIterator::operator*() const noexcept {
	return SqliteRow(statement_->getABI());
}
