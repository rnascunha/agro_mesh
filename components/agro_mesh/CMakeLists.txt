set(RESOURCE_PATH	net/resources)
set(AGRO_SRC	agro_mesh.cpp
				storage.cpp
				sensor/sensor_type.cpp
				sensor/helper.cpp
				net/coap_resources.cpp
				net/init_mesh.cpp
				net/tasks.cpp
				net/messages/make.cpp
				net/helper.cpp
				helper/convert.cpp
				modules/executor/executor.cpp)

set(REQUIRE_LIST coap-te
					wear_levelling
                    fatfs
                    ds3231
                    nvs_flash
                    app_update
                    elfloader)

######################################
# Modules
######################################

if(CONFIG_ENABLE_OTA_SUPPORT)
	list(APPEND AGRO_SRC modules/ota/task.cpp 
						${RESOURCE_PATH}/ota.cpp)
endif()

if(CONFIG_ENABLE_APP_SUPPORT)
	message(STATUS "APP SUPPORTED!")
	list(APPEND AGRO_SRC modules/app/app.cpp 
							modules/app/task.cpp 
							modules/app/executor.cpp 
							${RESOURCE_PATH}/app.cpp)
endif()

if(CONFIG_ENABLE_DEVICE_CLOCK)
	list(APPEND AGRO_SRC modules/clock/rtc_time.cpp
							modules/clock/helper.cpp
							${RESOURCE_PATH}/rtc.cpp)
endif()

if(CONFIG_ENABLE_DEVICE_CLOCK AND CONFIG_ENABLE_JOBS_SUPPORT)
	list(APPEND AGRO_SRC modules/job/job.cpp
							modules/job/job_task.cpp
							${RESOURCE_PATH}/job.cpp)
endif()

###################################
# Resources
###################################
if(CONFIG_ENABLE_COAP_RESOURCES)
	if(CONFIG_COAP_RESOURCE_LED)
		list(APPEND AGRO_SRC ${RESOURCE_PATH}/led.cpp)
	endif()
	
	if(CONFIG_COAP_RESOURCE_DISCOVERY)
		list(APPEND AGRO_SRC ${RESOURCE_PATH}/discovery.cpp)
	endif()
	
	if(CONFIG_COAP_RESOURCE_UPTIME OR 
		CONFIG_COAP_RESOURCE_REBOOT OR 
		CONFIG_COAP_RESOURCE_RESET_REASON)
		list(APPEND AGRO_SRC ${RESOURCE_PATH}/system.cpp)
	endif()
	
	if(CONFIG_COAP_RESOURCE_VERSION)
		list(APPEND AGRO_SRC ${RESOURCE_PATH}/version.cpp)
	endif()
	
	if(CONFIG_COAP_RESOURCE_NET_RSSI OR
		CONFIG_COAP_RESOURCE_NET_WAIVE_ROOT OR
		CONFIG_COAP_RESOURCE_NET_PARENT OR
		CONFIG_COAP_RESOURCE_NET_ROUTE OR
		CONFIG_COAP_RESOURCE_NET_CONFIG OR
		CONFIG_COAP_RESOURCE_NET_FULL_CONFIG)
		list(APPEND AGRO_SRC ${RESOURCE_PATH}/net.cpp)
	endif()
endif(CONFIG_ENABLE_COAP_RESOURCES)

idf_component_register(SRCS ${AGRO_SRC}
                       INCLUDE_DIRS include
                       REQUIRES ${REQUIRE_LIST})

