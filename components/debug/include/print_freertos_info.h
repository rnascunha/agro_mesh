#ifdef __cplusplus
extern "C" {
#endif

#include "sdkconfig.h"

/**
 * Deve-se habilitar as funcoes de trace do freertos para correta
 * utilizacao dessa biblioteca
 */

#ifdef CONFIG_PRINT_FREERTOS_INFO_LOG_LEVEL

void print_freertos_tasks_name();

#else

inline void __attribute__((always_inline)) print_freertos_tasks_name(){}

#endif /* CONFIG_PRINT_FREERTOS_INFO_LOG_LEVEL */

#ifdef __cplusplus
}
#endif
