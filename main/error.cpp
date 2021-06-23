#include "error.hpp"

int Error::value() const noexcept
{
	return err_;
}

const char* Error::message() const noexcept
{
	return Error::message(err_);
}

const char* Error::message(int error)
{
	switch(static_cast<errc>(error))
	{
		case errc::insufficient_buffer:	return "insufficient buffer";
		default:
			break;
	}
	return "(unrecognized error)";
}

Error::operator bool() const
{
	return err_ != 0;
}
