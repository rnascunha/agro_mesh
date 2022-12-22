#ifndef AGRO_MESH_SENSOR_TYPE_HPP__
#define AGRO_MESH_SENSOR_TYPE_HPP__

#include <stdint.h>
#include <stddef.h>

union sensor_value{
	sensor_value(int);
	sensor_value(unsigned);
	sensor_value(float);
	sensor_value(uint8_t const*);
  constexpr
  sensor_value(const sensor_value&) noexcept = default;

	unsigned	uint_v;
	int		 	int_v;
	float		float_v;
	uint8_t 	array_v[sizeof(float)];

	void set(uint8_t const*) noexcept;

	sensor_value& operator=(int v) noexcept;
	sensor_value& operator=(unsigned v) noexcept;
	sensor_value& operator=(float v) noexcept;
	sensor_value& operator=(sensor_value const&) noexcept;
};

struct sensor_type{
	uint32_t		type:24;
	uint32_t		index:8;
	sensor_value	value;
};

class Sensor_Builder{
	public:
		Sensor_Builder(uint8_t* array, size_t capacity);

		uint8_t const* buffer() const noexcept;
		size_t capacity() const noexcept;
		size_t size() const noexcept;

		bool add(sensor_type const&) noexcept;
		bool add(uint32_t type, sensor_value const&) noexcept;
		bool add(uint32_t type, uint8_t index, sensor_value const&) noexcept;
	private:
		uint8_t* array_;
		size_t	capacity_;
		size_t size_ = 0;

    constexpr
		bool check_addtion() const noexcept;
};

#endif /* AGRO_MESH_SENSOR_TYPE_HPP__ */
