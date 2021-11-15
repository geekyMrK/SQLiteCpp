/*
 *MIT License

 *Copyright (c) 2021 geekyMrK

 *Permission is hereby granted, free of charge, to any person obtaining a copy
 *of this software and associated documentation files (the "Software"), to deal
 *in the Software without restriction, including without limitation the rights
 *to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *copies of the Software, and to permit persons to whom the Software is
 *furnished to do so, subject to the following conditions:

 *The above copyright notice and this permission notice shall be included in all
 *copies or substantial portions of the Software.

 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *SOFTWARE.
 */

#ifndef IncludeSQLiteCpp_
#define IncludeSQLiteCpp_

#include <string>
#include <sqlite3.h>


namespace Sqlite {
	
  
template <typename Type>
struct nullHandleTraits {
	static Type invalid() noexcept {
		return nullptr;
	}
};

  

template <typename Traits>
class UniqueHandle{

	using Type = decltype(Traits::invalid());
	Type TypeValue_;

	void close() noexcept {
		if (*this) {
			Traits::close(TypeValue_);
		}
	}

  public:
  
	UniqueHandle(const UniqueHandle &) = delete;
	
	UniqueHandle &operator=(const UniqueHandle &) = delete;

	explicit UniqueHandle(const Type TypeValue = Traits::invalid()) : TypeValue_{TypeValue} 
	{
	}
  
	UniqueHandle(UniqueHandle &&other) : TypeValue_{other.release()} 
	{
	}
	
	UniqueHandle operator=(UniqueHandle &&other){
		if (this != &other) {
			reset(other.release());
		}
		return *this;
	}
	
	~UniqueHandle() noexcept {
		close();
	}

	constexpr explicit operator bool() const noexcept {
		return TypeValue_ != Traits::invalid();
	}

	constexpr Type get() const noexcept {
		return TypeValue_;
	}
	
	Type *set() noexcept {
		return &TypeValue_;
	}

	[[nodiscard]] Type release() {
		Type TypeValue = TypeValue_;
		TypeValue_ = Traits::invalid();
		return TypeValue;
	}
	
	[[maybe_unused]] bool reset(Type typeValue = Traits::invalid()) noexcept {
		if (typeValue != TypeValue_)
		{
			close();
			TypeValue_ = typeValue;
		}
		return static_cast<bool>(*this);
	}

	void swap(UniqueHandle<Traits> &other) noexcept {
		Type temp = TypeValue_;
		TypeValue_ = other.TypeValue_;
		other.TypeValue_ = temp;
	}
};

  
template <typename Traits>
void swap(UniqueHandle<Traits> &left, UniqueHandle<Traits> &right) noexcept {
	left.swap(right);
}



struct exception {
	
	const int errorCode_;
	const std::string errorMessage_;

	exception(sqlite3 *connection) : errorCode_{sqlite3_extended_errcode(connection)}, errorMessage_{sqlite3_errmsg(connection)}
	{
	}
};

  

class SqliteConnection {
	
	struct SqliteConnectionTraits : public nullHandleTraits<sqlite3 *> {
		static void close(sqlite3 *value) noexcept {
			sqlite3_close(value);
		}
	};
	UniqueHandle<SqliteConnectionTraits> connectionHandle_;

	template <typename Function, typename CharacterSet>
	void internalOpen(Function openFunction, const CharacterSet *const filename) {
		SqliteConnection tempConnection;
	
		if (SQLITE_OK != openFunction(filename, tempConnection.connectionHandle_.set())) {
			tempConnection.throwLastError();
		}
	
		swap(connectionHandle_, tempConnection.connectionHandle_);
	}

  public:
	SqliteConnection() = default;
	
	template <typename CharacterSet>
	explicit SqliteConnection(const CharacterSet *const filename) {
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
	
	void throwLastError() const  {
		throw exception(getABI());
	}

	void open(const char *const filename) {
		internalOpen(sqlite3_open, filename);
	}

	void open(const wchar_t *const filename) {
		internalOpen(sqlite3_open16, filename);
	}
	
	long long lastRowId() const noexcept {
		return sqlite3_last_insert_rowid(getABI());
	}
};

  

template <typename T>
struct sqliteReader {
	
	constexpr int getInt(const int columnNum = 0) const noexcept {
		return sqlite3_column_int(static_cast<const T *>(this)->getABI(), columnNum);
	}

	constexpr const char *getString(const int columnNum = 0) const noexcept {
		return reinterpret_cast<const char *>(sqlite3_column_text(static_cast<const T *>(this)->getABI(), columnNum));
	}

	constexpr const wchar_t *getWideString(const int columnNum = 0) const noexcept {
		return static_cast<const wchar_t *>(sqlite3_column_text16(static_cast<const T *>(this)->getABI(), columnNum));
	}

	constexpr int getStringLength(const int columnNum) const noexcept {
		return sqlite3_column_bytes(static_cast<const T *>(this)->getABI(), columnNum);
	}

	constexpr int getWideStringLength(const int columnNum) const noexcept {
		return (sqlite3_column_bytes16(static_cast<const T *>(this)->getABI(), columnNum) / sizeof(wchar_t));
	}

};


  
class SqliteStatement : public sqliteReader<SqliteStatement> {
	
	struct SqliteStatementTraits : public nullHandleTraits<sqlite3_stmt *> {
		static void close(sqlite3_stmt *value) noexcept {
			sqlite3_finalize(value);
		}
	};

	UniqueHandle<SqliteStatementTraits> statementHandle_;


	template <typename PrepareFunction, typename CharacterSet, typename... VALUES>
	void internalPrepare(const SqliteConnection &connection, const PrepareFunction prepare, const CharacterSet *const text, VALUES &&... values) {
		if (SQLITE_OK != prepare(connection.getABI(), text, -1, statementHandle_.set(), nullptr)) {
			connection.throwLastError();
		}
		bindAll(std::forward<VALUES>(values)...);
	}
	
	void internalBindAll(const int) const noexcept
	{
	}
	
	template <typename FIRST, typename... REST_VALUES>
	void internalBindAll(const int index, FIRST &&first, REST_VALUES &&... restValues) const {
		bind(index, std::forward<FIRST>(first));
		internalBindAll(index + 1, std::forward<REST_VALUES>(restValues)...);
	}

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

	void throwLastError() const {
		throw exception(sqlite3_db_handle(getABI()));
	}

	template <typename... VALUES>
	void prepare(const SqliteConnection &connection, const char *const characterSet, VALUES &&... values) {
		internalPrepare(connection, sqlite3_prepare_v2, characterSet, std::forward<VALUES>(values)...);
	}
	
	template <typename... VALUES>
	void prepare(const SqliteConnection &connection, const wchar_t *const characterSet, VALUES &&... values) {
		internalPrepare(connection, sqlite3_prepare16_v2, characterSet, std::forward<VALUES>(values)...);
	}

	bool execute() const {
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
	
	void bind(const int index, const int value) const {
		if (SQLITE_OK != sqlite3_bind_int(getABI(), index, value)) {
			throwLastError();
		}
	}
	
	void bind(const int index, const char *const strValue, const int size = -1) const {
		if (SQLITE_OK != sqlite3_bind_text(getABI(), index, strValue, size, SQLITE_STATIC)) {
			throwLastError();
		}
	}
	
	void bind(const int index, const wchar_t *const strValue, const int size = -1) const {
		if (SQLITE_OK != sqlite3_bind_text16(getABI(), index, strValue, size, SQLITE_STATIC)) {
			throwLastError();
		}
	}
	
	void bind(const int index, const std::string &strValue) const {
		bind(index, strValue.c_str(), static_cast<int>(strValue.size()));
	}
	
	void bind(const int index, const std::wstring &strValue) const {
		bind(index, strValue.c_str(), static_cast<int>((strValue.size() * sizeof(wchar_t))));
	}
	
	void bind(const int index, const std::string &&strValue) const {
		if (SQLITE_OK != sqlite3_bind_text(getABI(), index, strValue.c_str(), static_cast<int>(strValue.size()), SQLITE_TRANSIENT)) {
			throwLastError();
		}
	}
	
	void bind(const int index, const std::wstring &&strValue) const {
		if (SQLITE_OK != sqlite3_bind_text16(getABI(), index, strValue.c_str(), static_cast<int>((strValue.size() * sizeof(wchar_t))), SQLITE_TRANSIENT)) {
			throwLastError();
		}
	}

	template <typename... Values>
	void bindAll(Values &&... values) const {
		internalBindAll(1, std::forward<Values>(values)...);
	}
	
	template <typename ...Values>
	void reset (Values&&... values) {
		if (SQLITE_OK != sqlite3_reset(getABI ())) {
			throwLastError ();
		}
		bindAll(std:: forward<Values> (values)...);
	}
};


  
template <typename CharacterSet, typename... Values>
void sqliteExecute(const SqliteConnection &connection, const CharacterSet *const text, Values &&... values) {
	SqliteStatement(connection, text, std::forward<Values>(values)...).execute();
}



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
	
	SqliteStatement const *statement_{nullptr};

  public:
  
	rowIterator() noexcept = default;
	
	rowIterator(const SqliteStatement &statement) {
		if (statement.execute()) {
			statement_ = &statement;
		}
	}
	
	rowIterator &operator++() {
		if (!statement_->execute()) {
			statement_ = nullptr;
		}
		return *this;
	}

	constexpr bool operator!=(const rowIterator &other) const noexcept {
		return statement_ != other.statement_;
	}

	SqliteRow operator*() const noexcept {
		return SqliteRow(statement_->getABI());
	}
};

  

inline rowIterator begin(const SqliteStatement &statement) noexcept {
	return rowIterator(statement);
}

constexpr inline rowIterator end(const SqliteStatement &) noexcept {
	return rowIterator();
}


}

#endif
