#ifndef AGRO_MESH_TYPES_APP_TYPES_HPP__
#define AGRO_MESH_TYPES_APP_TYPES_HPP__

#include <cstdint>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#if defined(CONFIG_FATFS_LFN_HEAP) || defined(CONFIG_FATFS_LFN_STACK)
static constexpr const unsigned app_max_name_size = CONFIG_FATFS_MAX_LFN;
#else /* CONFIG_FATFS_LFN_NONE */
static constexpr const unsigned app_max_name_size = 12;
#endif

static constexpr const unsigned APP_HASH_SIZE = 32;

struct app
{
	unsigned 		size = 0;						//app size
	std::uint8_t	hash[APP_HASH_SIZE];			//app hash
	char			name[app_max_name_size + 1];	//app name (Null terminated)
};

enum class app_status{
	success = 0,
	not_mounted,
	not_found,
	already_exists,
	file_list_error,
	allocation_error,
	server_response,
	receive_timeout
};

const char* app_error_string(app_status status) noexcept;

app_status get_app(app& app_, const char* name) noexcept;
app_status get_app_list(unsigned char*, unsigned, unsigned& readed, unsigned read_from = 0) noexcept;
app_status add_app(const char* name, unsigned size, std::uint8_t* hash) noexcept;
app_status delete_app(const char* name) noexcept;
app_status execute_app(const char* name, int args, int& ret) noexcept;
app_status execute_app_as_task(const char* app_name,
							int arg,
							uint32_t stack_size,
							int priority,
							TaskHandle_t* handler = nullptr) noexcept;
app_status read_app_file(const char* app_name, unsigned char**) noexcept;

bool init_app_task(const char*, const std::uint8_t*) noexcept;

#endif /* AGRO_MESH_TYPES_APP_TYPES_HPP__ */
