namespace Sqlite {
	
	
template <typename Type>
struct nullHandleTraits {
	static Type invalid() noexcept {
		return nullptr;
	}
};



template <typename Traits>
class UniqueHandle
{
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

	explicit UniqueHandle(const Type TypeValue = Traits::invalid()) : TypeValue_{TypeValue} {
	}
	
	UniqueHandle(UniqueHandle &&other) : TypeValue_{other.release()} {
	}
	
	UniqueHandle operator=(UniqueHandle &&other);
	
	~UniqueHandle() noexcept {
		close();
	}

	explicit operator bool() const noexcept {
		return TypeValue_ != Traits::invalid();
	}

	Type get() const noexcept { return TypeValue_; }

	Type *set() noexcept { return &TypeValue_; }

	[[nodiscard]] Type release() {
		Type TypeValue = TypeValue_;
		TypeValue_ = Traits::invalid();
		return TypeValue;
	}

	[[maybe_unused]] bool reset(Type typeValue = Traits::invalid()) noexcept;

	void swap(UniqueHandle<Traits> &other) noexcept;
};
