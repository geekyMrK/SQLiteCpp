#include "UniqueHandle.hpp"

template <typename Traits>
Sqlite::UniqueHandle<Traits> Sqlite::UniqueHandle<Traits>::operator=(Sqlite::UniqueHandle<Traits> &&other) {
	if (this != &other) {
		reset(other.release());
	}
	return *this;
}

template <typename Traits>
[[maybe_unused]] bool Sqlite::UniqueHandle<Traits>::reset(Type typeValue ) noexcept {
	if (typeValue != TypeValue_) {
		close();
		TypeValue_ = typeValue;
	}
	return static_cast<bool>(*this);
}

