#ifndef AGRO_MESH_MODULE_APP_EVIRONMENT_HPP__
#define AGRO_MESH_MODULE_APP_EVIRONMENT_HPP__

#include "loader.h"
#include "driver/gpio.h"

static const ELFLoaderSymbol_t exports[] = {
    { "puts", (void*) puts },
    { "printf", (void*) printf },
	{ "gpio_set_level", (void*)gpio_set_level}
};

static constexpr const ELFLoaderEnv_t env = {
		exports,
		sizeof(exports) / sizeof(*exports)
};

#endif /* AGRO_MESH_MODULE_APP_EVIRONMENT_HPP__ */
