set(RESOURCE_PATH	net/resources)
set(RESOURCE_LIST	${RESOURCE_PATH}/ac_load.cpp
					${RESOURCE_PATH}/rtc.cpp
					${RESOURCE_PATH}/system.cpp
					${RESOURCE_PATH}/led.cpp
					${RESOURCE_PATH}/net.cpp
					${RESOURCE_PATH}/version.cpp
					${RESOURCE_PATH}/discovery.cpp
					${RESOURCE_PATH}/packets.cpp
					${RESOURCE_PATH}/ota.cpp
					${RESOURCE_PATH}/job.cpp
					${RESOURCE_PATH}/app.cpp
					)	

idf_component_register(SRCS main.cpp
							init_io.cpp  
							storage.cpp
							job.cpp
							types/rtc_time.cpp
							types/job.cpp
							types/app/app.cpp
							types/app/task.cpp
							sensor/sensor_type.cpp
							sensor/read_sensor.cpp
							sensor/helper.cpp
							net/coap_resources.cpp
							net/init_mesh.cpp
							net/tasks.cpp
							net/messages/make.cpp
							ota/task.cpp
							${RESOURCE_LIST}
							helper/convert.cpp
                       INCLUDE_DIRS
                       REQUIRES)
						
target_compile_options(${COMPONENT_LIB} 
						PRIVATE -std=gnu++17 
						-DCOAP_TE_ESP_IDF_PLATAFORM=1
						-DCOAP_TE_PORT_ESP_MESH=1
						-DCOAP_TE_PORT_POSIX=1
						-DCOAP_TE_PORT_C_STANDARD=1
						-DCOAP_TE_LOG_LEVEL=${CONFIG_COAP_TE_LOG_LEVEL}
						-DCOAP_TE_USE_SELECT=1)
