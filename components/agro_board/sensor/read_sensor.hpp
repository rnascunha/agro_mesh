#ifndef AGRO_MESH_SENSOR_READ_SENSOR_HPP__
#define AGRO_MESH_SENSOR_READ_SENSOR_HPP__

#define INVALID_TEMPERATURE		(-127.0)

unsigned read_gpios() noexcept;
float read_temperature_retry(unsigned retry_times) noexcept;

#endif /* AGRO_MESH_SENSOR_READ_SENSOR_HPP__ */
