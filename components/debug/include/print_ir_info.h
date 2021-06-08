#ifndef PRINT_IR_INFO_H__
#define PRINT_IR_INFO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "sdkconfig.h"

#include "stddef.h"
#include "driver/rmt.h"
#include "ir.h"
#include "ir_generic.h"

#ifdef CONFIG_PRINT_IR_INFO_LOG_LEVEL

void print_rmt_item32(rmt_item32_t* item, size_t size);
void print_rmt_item32_ticks(rmt_item32_t* item, size_t size);
void print_ir_protocol(ir_protocol_t protocol);
void print_ir_generic_code(ir_generic_t* gen);
void print_ir_generic_code_formated(ir_generic_t* gen);

#else

inline void __attribute__((always_inline)) print_rmt_item32(rmt_item32_t* item, size_t size){}
inline void __attribute__((always_inline)) print_rmt_item32_ticks(rmt_item32_t* item, size_t size){}
inline void __attribute__((always_inline)) print_ir_protocol(ir_protocol_t protocol){}
inline void __attribute__((always_inline)) print_ir_generic_code(ir_generic_t* gen){}
inline void __attribute__((always_inline)) print_ir_generic_code_formated(ir_generic_t* gen);

#endif /* CONFIG_PRINT_IR_INFO_LOG_LEVEL */

#ifdef __cplusplus
}
#endif

#endif /* PRINT_IR_INFO_H__ */
