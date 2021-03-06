#ifndef AGRO_MESH_PINOUT_HPP__
#define AGRO_MESH_PINOUT_HPP__

#include "hal/gpio_types.h"

#define I2C_SDA				GPIO_NUM_0
#define I2C_SCL				GPIO_NUM_4

#define AC_LOAD1			GPIO_NUM_21		//n-board 21: contator
#define AC_LOAD2			GPIO_NUM_18
#define AC_LOAD3			GPIO_NUM_5

#define DS18B20_DATA		GPIO_NUM_17

#define GPIO_GENERIC1		GPIO_NUM_23		//n-borad K1: contator
#define GPIO_GENERIC2		GPIO_NUM_22		//K2
#define GPIO_GENERIC3		GPIO_NUM_19
#define GPIO_GENERIC4		GPIO_NUM_33		//n-board T1: relé térmico

#define WATER_LEVEL1		GPIO_NUM_26
#define WATER_LEVEL2		GPIO_NUM_14
#define WATER_LEVEL3		GPIO_NUM_12
#define WATER_LEVEL4		GPIO_NUM_27

#define PRESSURE_SENSOR		GPIO_NUM_32		//Presssure/potenciometro 0V-1.1V - ADC
#define	VOLTAGE_SENSOR		GPIO_NUM_34		//Voltage sensor 0V-2.6V - ADC

#define CURRENT_SENSOR1		GPIO_NUM_36		//n-board mounted
#define CURRENT_SENSOR1_ADC	ADC1_CHANNEL_0

#define CURRENT_SENSOR2		GPIO_NUM_39
#define CURRENT_SENSOR2_ADC	ADC1_CHANNEL_3

#endif /* AGRO_MESH_PINOUT_HPP__ */
