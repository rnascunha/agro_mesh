#ifndef __DS3231_H__
#define __DS3231_H__

/**
 * \file
 * \brief Biblioteca para utilização do módulo RTC DS3231
 *
 * DS3231 é um RTC de alta precisão. Ele se comunica através de uma porta I2C,
 * em modo \i standard ou \i fast. É possível a configuração de 2 alarmes internos,
 * em diferentes modalidades, gerando estes alarmes interrupções externas.
 *
 * Também há pinos externos para verificar a frequência de 32kHz, e possibilidade de ondas
 * quadradas nas frequências de 1Hz, 1kHz, 4kHz e 8kHz.
 *
 * O dispositivo há um sensor de temperatura interno para compensação de diferenças de
 * temperatura.
 *
 * \note Interrupções devem pegar o <i>FALLING EDGE</i>
 *
 * Manual: <a href="https://datasheets.maximintegrated.com/en/ds/DS3231.pdf">DS3231</a>
 *
 * TODO Testar implementação para modo de 12h (AM/PM)
 *
 * \author Rafael Cunha <rnascunha@gmail.com>
 * \date 07/2016
 */

/**
 * \defgroup ds3231 DS3231
 *
 * \brief Definição da classe, tipos e macros referente a utilização de LCDs.
 * @{
 */

/*
 * TODO Criar helper class DateTime, Convert, BitWise
 */

#include "datetime.h"
#include "i2c_master.h"

//Frequency of Square Wave output
typedef enum DS3231SQWrate{
	SQW_RATE_1 = 0,
	SQW_RATE_1K,
	SQW_RATE_4K,
	SQW_RATE_8K
}DS3231SQWrate;

// Output type of NINT/SQW pin
#define OUTPUT_SQW		0
#define OUTPUT_INT		1

//// Hour format
//#define FORMAT_24H	0
//#define FORMAT_12H	1
//
//// Days of the week
//#define SUNDAY		0
//#define MONDAY		1
//#define TUESDAY		2
//#define WEDNESDAY	3
//#define THURSDAY	4
//#define FRIDAY		5
//#define SATURDAY	6

//Alarm 1 config
typedef enum DS3231Alarm1Config {
	PER_SECOND = 0,
	SECONDS_MATCH,
	MIN_SEC_MATCH,
	HOUR_MIN_SEC_MATCH,
	DATE_HOUR_MIN_SEC_MATCH,
	DAY_HOUR_MIN_SEC_MATCH
} DS3231Alarm1Config;

//Alarm 2 config
typedef enum DS3231Alarm2Config{
	PER_MINUTE = 0,
	MINUTES_MATCH,
	HOUR_MIN_MATCH,
	DATE_HOUR_MIN_MATCH,
	DAY_HOUR_MIN_MATCH
}DS3231Alarm2Config;

class DS3231{
	public:
		DS3231(I2C_Master *i2c, uint8_t mode = FORMAT_24H);

		void begin(void);

		void setDateTime(DateTime *dateTime);
		void getDateTime(DateTime *dateTime);

		void setTime(DateTime *dateTime);
		void getTime(DateTime *dateTime);

		void setDate(DateTime *dateTime);
		void getDate(DateTime *dateTime);

		void setTimeMode(uint8_t mode);
		uint8_t getTimeMode();

		void setDOW(uint8_t dow);
		uint8_t getDOW();

		void enable32kHz(bool enable);

		void enableInterrupt(uint8_t enable = true);
		void enableSQWRate(DS3231SQWrate rate, uint8_t enable = true);

		void enableAlarm2Int(bool enable = true);
		void enableAlarm1Int(bool enable = true);

		uint8_t clearAlarmFlags();

		void configAlarm2(DS3231Alarm2Config type_alarm, DateTime *dateTime = NULL);
		void configAlarm1(DS3231Alarm1Config type_alarm, DateTime *dateTime = NULL);

		void startConvTemp();
		float getTemp();

	protected:
		I2C_Master *_i2c;

		uint8_t _time_mode;

		void _send(uint8_t reg, uint8_t* data, size_t length);
		void _send(uint8_t reg, uint8_t data);

		void _receive(uint8_t reg, uint8_t* data, size_t length);
		uint8_t _receive(uint8_t reg);

		void _set_bit_reg(uint8_t reg, uint8_t bit_mask, bool value = true);
		uint8_t _get_bit_reg(uint8_t reg, uint8_t bit_mask);

		static uint8_t _decode(uint8_t value);
		static uint8_t _decodeH(uint8_t value);
		static uint8_t _decodeY(uint8_t value);
		static uint8_t _encode(uint8_t value);
};

/**@}*/

#endif /* __DS3231_H__ */
