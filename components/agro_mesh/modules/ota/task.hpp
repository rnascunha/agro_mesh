#ifndef AGRO_MESH_OTA_TASK_HPP__
#define AGRO_MESH_OTA_TASK_HPP__

#if CONFIG_ENABLE_OTA_SUPPORT == 1

void init_ota_task(const char*, std::size_t) noexcept;

#else /* CONFIG_ENABLE_OTA_SUPPORT */

inline __attribute__((always_inline))
void init_ota_task(const char*, std::size_t) noexcept{}

#endif /* CONFIG_ENABLE_OTA_SUPPORT */

#endif /* AGRO_MESH_OTA_TASK_HPP__ */
