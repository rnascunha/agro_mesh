#include "sensor_type.hpp"
#include <string.h>

sensor_value::sensor_value(int v)
	: int_v{v}{}
sensor_value::sensor_value(unsigned v)
	: uint_v{v}{}
sensor_value::sensor_value(float v)
	: float_v{v}{}
sensor_value::sensor_value(uint8_t const* v)
{
	set(v);
}

sensor_value& sensor_value::operator=(int v) noexcept
{
	int_v = v;
	return *this;
}

sensor_value& sensor_value::operator=(unsigned v) noexcept
{
	uint_v = v;
	return *this;
}

sensor_value& sensor_value::operator=(float v) noexcept
{
	float_v = v;
	return *this;
}

sensor_value& sensor_value::operator=(sensor_value const& v) noexcept
{
	this->set(v.array_v);
	return *this;
}

void sensor_value::set(uint8_t const* v) noexcept
{
	memcpy(array_v, v, sizeof(float));
}

Sensor_Builder::Sensor_Builder(uint8_t* array, size_t capacity)
	: array_{array}, capacity_{capacity}{}

uint8_t const* Sensor_Builder::buffer() const noexcept
{
	return array_;
}

size_t Sensor_Builder::capacity() const noexcept
{
	return capacity_;
}

size_t Sensor_Builder::size() const noexcept
{
	return size_;
}

bool Sensor_Builder::add(sensor_type const& value) noexcept
{
	if(!check_addtion())
	{
		return false;
	}

	memcpy(array_ + size_, &value, sizeof(sensor_type));
	size_ += sizeof(sensor_type);

	return true;
}

bool Sensor_Builder::add(uint32_t type, sensor_value const& v) noexcept
{
	return add({type, 0, v});
}

bool Sensor_Builder::add(uint32_t type, uint8_t index, sensor_value const& v) noexcept
{
	return add({type, index, v});
}

bool Sensor_Builder::check_addtion() const noexcept
{
	return !((sizeof(sensor_type) + size_) > capacity_);
}

