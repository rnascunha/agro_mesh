#ifndef AGRO_MESH_TYPES_APP_TYPES_HPP__
#define AGRO_MESH_TYPES_APP_TYPES_HPP__

static constexpr const unsigned app_max_name_size = 32;

struct app{
	char		name[app_max_name_size];
	unsigned 	size = 0;
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
app_status add_app(const char* name, unsigned size) noexcept;
app_status delete_app(const char* name) noexcept;
app_status execute_app(const char* name, int args, int& ret) noexcept;

bool init_app_task(const char*) noexcept;

#endif /* AGRO_MESH_TYPES_APP_TYPES_HPP__ */