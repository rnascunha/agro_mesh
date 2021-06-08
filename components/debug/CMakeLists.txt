idf_component_register(SRCS print_types_info.c 
							./print_system_info.c 
#							./print_partition_info.c 
							./get_type_string.c 
							./print_freertos_info.c 
							./print_chip_info.c 
							./print_wifi_info.c 
#							./print_coap_info.c 
#							./print_ir_info.c 
#							./debug_example.c 
#							./print_time_info.cpp 
#							./get_type_time_string.cpp
                       INCLUDE_DIRS include                       # Edit following two lines to set component requirements (see docs)
                       REQUIRES spi_flash 
#                       			ir 
#                       			coap 
#                       			bootloader_support 
#                       			app_update 
#                       			scheduler
                       			)

