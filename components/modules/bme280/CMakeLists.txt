idf_component_register(SRCS bme280.cpp bme280_i2c.cpp bme280_spi.cpp bme280_spi3w.cpp
                       INCLUDE_DIRS include
                       REQUIRES i2c_master helper basic_defines)