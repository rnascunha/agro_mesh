set(RESOURCE_PATH	net/resources)
set(RESOURCE_LIST	${RESOURCE_PATH}/ac_load.cpp
					${RESOURCE_PATH}/rtc.cpp
					${RESOURCE_PATH}/sensors.cpp
					${RESOURCE_PATH}/system.cpp
					${RESOURCE_PATH}/led.cpp
					${RESOURCE_PATH}/net.cpp
					${RESOURCE_PATH}/version.cpp
					)	

idf_component_register(SRCS agro_mesh_main.cpp
							init_io.cpp  
							net/coap_resources.cpp
							net/init_mesh.cpp
							net/tasks.cpp
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
						-DCOAP_TE_LOG_LEVEL=5
						-DCOAP_TE_USE_SELECT=1)
