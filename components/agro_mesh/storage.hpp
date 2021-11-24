#ifndef AGRO_MESH_STORAGE_HPP__
#define AGRO_MESH_STORAGE_HPP__

#define STORAGE_BASE_PATH		"/flash"

bool mount_storage() noexcept;
bool unmount_storage() noexcept;
bool storage_is_mounted() noexcept;

#endif /* AGRO_MESH_STORAGE_HPP__ */
