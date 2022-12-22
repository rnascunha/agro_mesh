#include "storage.hpp"
#include "wear_levelling.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "esp_log.h"
#include "esp_system.h"

#define TAG "STORAGE"

// Handle of the wear levelling library instance
static wl_handle_t s_wl_handle = WL_INVALID_HANDLE;

// Mount path for the partition
const char *base_path = STORAGE_BASE_PATH;
const char* app_list = STORAGE_BASE_PATH "/app";
const char* app_temp = STORAGE_BASE_PATH "/app_temp";

bool mount_storage() noexcept
{
	const esp_vfs_fat_mount_config_t mount_config = {
			.format_if_mount_failed = true,
			.max_files = 4,
			.allocation_unit_size = CONFIG_WL_SECTOR_SIZE,
      .disk_status_check_enable = false
	};
	//esp_err_t err = esp_vfs_fat_spiflash_mount(base_path, "storage", &mount_config, &s_wl_handle);
	esp_err_t err = esp_vfs_fat_spiflash_mount_rw_wl(base_path, "storage", &mount_config, &s_wl_handle);

	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Failed to mount FATFS (%s)", esp_err_to_name(err));
		return false;
	}
	return true;
}

bool unmount_storage() noexcept
{
	if(s_wl_handle != WL_INVALID_HANDLE)
	{
//		esp_err_t err = esp_vfs_fat_spiflash_unmount(base_path, s_wl_handle);
		esp_err_t err = esp_vfs_fat_spiflash_unmount_rw_wl(base_path, s_wl_handle);
		s_wl_handle = WL_INVALID_HANDLE;
		return err == ESP_OK;
	}
	return true;
}

bool storage_is_mounted() noexcept
{
	return s_wl_handle != WL_INVALID_HANDLE;
}
