set(RESOURCE_PATH		net/resources)
set(RESOURCE_LIST		${RESOURCE_PATH}/packets.cpp
						${RESOURCE_PATH}/ac_load.cpp)

idf_component_register(SRCS board.cpp
							init_io.cpp
							executor.cpp
							net/task.cpp
							net/init_resources.cpp
							net/make.cpp
							net/send.cpp
							net/net.cpp
							sensor/read_sensor.cpp
							${RESOURCE_LIST}
                       INCLUDE_DIRS include
                       REQUIRES gpio 
                       			i2c_master 
                       			agro_mesh 
                       			dallas_temperature 
                       			onewire)
