#include "app.hpp"

#include "../../storage.hpp"
#include "esp_vfs_fat.h"
#include "esp_log.h"
#include <stdio.h>
#include <cstring>
#include <malloc.h>
#include <unistd.h>
#include "loader.h"
#include "driver/gpio.h"

#define TAG "app"

extern const char* app_list;
extern const char* app_temp;
extern const char *base_path;

static const ELFLoaderSymbol_t exports[] = {
    { "puts", (void*) puts },
    { "printf", (void*) printf },
	{ "gpio_set_level", (void*)gpio_set_level}
};

static const ELFLoaderEnv_t env = {
		exports,
		sizeof(exports) / sizeof(*exports)
};

static char* make_file_path(char* buf, const char* name) noexcept
{
	//Making full file path
	std::strcpy(buf, base_path);
	std::strcat(buf, "/");
	std::strcat(buf, name);

	return buf;
}

const char* app_error_string(app_status status) noexcept
{
	switch(status)
	{
		case app_status::success: return "success";
		case app_status::not_found: return "not found";
		case app_status::not_mounted: return "not mounted";
		case app_status::already_exists: return "already exists";
		case app_status::file_list_error: return "file list error";
		case app_status::allocation_error: return "allocation error";
		case app_status::receive_timeout: return "receive timeout";
		case app_status::server_response: return "server response";
		default:
			break;
	}
	return "undefined";
}

app_status get_app_list(unsigned char* buffer,
		unsigned size,
		unsigned& readed,
		unsigned read_from /* = 0 */) noexcept
{
	if(!storage_is_mounted()) return app_status::not_mounted;

	FILE *f = fopen(app_list, "rb");
	if(!f)
	{
		ESP_LOGE(TAG, "[get_app] Error opeining app list file");
		return app_status::file_list_error;
	}
	if(read_from) fseek(f, read_from, SEEK_SET);

	readed = fread(buffer, 1, size, f);
	ESP_LOGI(TAG, "[get_app_list] readed = %u", readed);
	fclose(f);

	return app_status::success;
}

app_status get_app(app& app_, const char* name) noexcept
{
	if(!storage_is_mounted()) return app_status::not_mounted;

	ESP_LOGI(TAG, "[get_app] Search %s", name);
	FILE *f = fopen(app_list, "rb");
	if(!f)
	{
		ESP_LOGE(TAG, "[get_app] Error opeining app list file");
		return  app_status::file_list_error;
	}

	bool finded = false;
	do{
		unsigned size_read = fread(reinterpret_cast<char*>(&app_), 1, sizeof(app), f);
		if(!size_read) break;
		ESP_LOGI(TAG, "[get_app] readed = %u / name = %s", size_read, app_.name);
		if(strcmp(app_.name, name) == 0)
		{
			finded = true;
			break;
		}
	}while(true);
	fclose(f);

	return finded ? app_status::success : app_status::not_found;
}

app_status add_app(const char* name, unsigned size) noexcept
{
	if(!storage_is_mounted()) return app_status::not_mounted;

	app app_;
	app_status status = get_app(app_, name);
	if(status == app_status::success) return app_status::already_exists;

	FILE *f = fopen(app_list, "a+b");
	if(!f)
	{
		ESP_LOGE(TAG, "[add_app] Error opeining app list file");
		return app_status::file_list_error;
	}

	strncpy(app_.name, name, app_max_name_size);
	app_.size = size;

	fwrite(reinterpret_cast<char*>(&app_), 1, sizeof(app), f);
	fclose(f);

	return app_status::success;
}

app_status delete_app(const char* name) noexcept
{
	if(!storage_is_mounted()) return app_status::not_mounted;

	ESP_LOGI(TAG, "APP to delete %s", name);

	FILE *f = fopen(app_list, "rb");
	if(!f)
	{
		ESP_LOGE(TAG, "[delete_app] Error opeining app list file");
		return app_status::file_list_error;
	}
	FILE *temp = fopen(app_temp, "wb");
	if(!temp)
	{
		ESP_LOGE(TAG, "[delete_app] Error opeining temp file");
		fclose(f);
		return app_status::file_list_error;
	}

	app app_;
	bool finded = false;
	unsigned readed = 0;
	do{
		readed = fread(reinterpret_cast<char*>(&app_), 1, sizeof(app), f);
		if(!readed) break;
		if(!finded && strcmp(app_.name, name) == 0)
		{
			finded = true;
		}
		else
		{
			fwrite(reinterpret_cast<char*>(&app_), 1, sizeof(app), temp);
		}
	}while(true);

	fclose(temp);
	fclose(f);

	unlink(app_list);
	char buffer[app_max_name_size + 10];
	unlink(make_file_path(buffer, name));

	rename(app_temp, app_list);

	return app_status::success;
}

app_status execute_app(const char* name, int args, int& ret) noexcept
{
	if(!storage_is_mounted()) return app_status::not_mounted;

	ESP_LOGI(TAG, "APP to exec %s", name);

	app app_;
	app_status status = get_app(app_, name);
	if(status != app_status::success)
	{
		ESP_LOGE(TAG, "[exec_app] get_app not found = %s", name);
		return status;
	}

	unsigned char* app_buf = (unsigned char*)malloc(sizeof(unsigned char) * app_.size);
	if(!app_buf) return app_status::allocation_error;

	char buffer[app_max_name_size + 10];
	FILE* f = fopen(make_file_path(buffer, name), "rb");
	if(!f)
	{
		ESP_LOGE(TAG, "[exec_app] Error opening file = %s", name);
		return app_status::file_list_error;
	}
	unsigned readed = fread(app_buf, 1, app_.size, f);
	fclose(f);
	if(readed != app_.size)
	{
		ESP_LOGE(TAG, "[exec_app] Readed less than should [%u-%u]", readed, app_.size);
		return app_status::file_list_error;
	}

	ret = elfLoader(app_buf, &env, "local_main", args);

	free(app_buf);

	return app_status::success;
}
