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

template <typename Traits>
void Sqlite::UniqueHandle<Traits>::swap(UniqueHandle &other) noexcept {
	Type temp = TypeValue_;
	TypeValue_ = other.TypeValue_;
	other.TypeValue_ = temp;
}
