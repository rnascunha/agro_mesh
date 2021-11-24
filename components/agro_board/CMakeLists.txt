set(RESOURCE_PATH		net/resources)
set(RESOURCE_LIST		${RESOURCE_PATH}/packets.cpp
						${RESOURCE_PATH}/ac_load.cpp)

idf_component_register(SRCS board.cpp
							init_io.cpp
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
                       			onewire 
                       			coap-te)

target_compile_options(${COMPONENT_LIB} 
						PRIVATE -std=gnu++17
						-DCOAP_TE_ESP_IDF_PLATAFORM=1
						-DCOAP_TE_PORT_ESP_MESH=1
						-DCOAP_TE_PORT_POSIX=1
						-DCOAP_TE_PORT_C_STANDARD=1
						-DCOAP_TE_LOG_LEVEL=${CONFIG_COAP_TE_LOG_LEVEL}
						-DCOAP_TE_USE_SELECT=1)