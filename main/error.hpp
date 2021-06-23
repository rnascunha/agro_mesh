#ifndef AGRO_MESH_ERROR_CODE_HPP__
#define AGRO_MESH_ERROR_CODE_HPP__

enum class errc : int{
	//General
	insufficient_buffer		= 10,
};

struct Error {
	Error(){}
	Error(const Error&) = default;

	int value() const noexcept;
	const char* message() const noexcept;
	static const char* message(int error);

	void clear() noexcept{ err_ = 0; }

	operator bool() const;
	inline bool operator==(Error const& rhs)
	{
		return err_ == rhs.err_;
	}

	inline bool operator!=(Error const& rhs)
	{
		return !(*this == rhs);
	}

	inline Error& operator=(Error const& other) noexcept
	{
		err_ = other.err_;
		return *this;
	}

	inline Error& operator=(errc const& error) noexcept
	{
		err_ = static_cast<int>(error);
		return *this;
	}

	inline bool operator==(errc const& error) noexcept
	{
		return err_ == static_cast<int>(error);
	}

	inline bool operator!=(errc const& error) noexcept
	{
		return !(*this == error);
	}
	private:
		int err_ = 0;
};

#endif /* AGRO_MESH_ERROR_CODE_HPP__ */
