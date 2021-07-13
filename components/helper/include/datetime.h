#ifndef	__DATETIME_H__
#define __DATETIME_H__

#include <stdio.h>
#include <stdint.h>

typedef enum {
	FORMAT_24H = 0,
	FORMAT_12H
} TimeMode;

/*
 * Começando a semana em 1, baseado na iso
 * https://en.wikipedia.org/wiki/ISO_week_date
 */

typedef enum {
	MONDAY = 1,
	TUESDAY,
	WEDNEDAY,
	THURSDAY,
	FRIDAY,
	SATURDAY,
	SUNDAY
} DayOfWeek;

typedef enum {
	AM = 0,				//Antes de meio dia
	PM,					//Depois de meio dia
	ND					//Nao definido, para formado de 24H
}DayPeriod;

class DateTime {
	public:

		DateTime(TimeMode mode = FORMAT_24H, int fuse = 0);

		//SET
		void setYear(uint16_t year);
		void setMonth(uint8_t month);
		void setDay(uint8_t month);
		void setDate(uint16_t year,
				uint8_t month,
				uint8_t day);

		void setHour(uint8_t hour, DayPeriod period = ND);
		void setMinute(uint8_t minute);
		void setSecond(uint8_t second);
		void setTime(uint8_t hour,
				uint8_t minute,
				uint8_t second,
				DayPeriod period = ND);

		void setDateTime(uint16_t year,
				uint8_t mouth,
				uint16_t day,
				uint8_t hour,
				uint8_t minute,
				uint8_t second,
				DayPeriod period = ND);

		void setUnixTime(uint32_t unixTime);

		//GET
		uint16_t getYear(void);
		uint8_t getMonth(void);
		uint8_t getDay(void);
		void getDate(DateTime *date);

		uint8_t getHour(void);
		uint8_t getMinute(void);
		uint8_t getSecond(void);
		DayPeriod getDayPeriod(void);
		TimeMode getTimeMode(void);
		int8_t getTimeFuse(void);
		void getTime(DateTime *time);

		void getDateTime(DateTime *dateTime);

		uint32_t getUnixTime(void);

		bool isLeapYear(void); /*é ano bissexto*/

		DayOfWeek dayOfWeek(void);
		static DayOfWeek dayOfWeek(uint8_t day, uint8_t month, uint16_t year) noexcept;

	protected:
		//Date
		uint8_t _day;		/*0-31*/
		uint8_t _month;		/*1-12*/
		uint16_t _year;		/* XXXX */

		//Time
		uint8_t _hour;		/*0-11 / 0-23*/
		uint8_t _minute;		/*0-59*/
		uint8_t _second;		/*0-59*/
		//uint16_t _miliseconds;	/*0-999*/
		//uint16_t _microseconds; /*0-999*/

		DayPeriod _period;			/* ND,AM,PM */

		TimeMode _timeMode;			/* Modo 12 ou 24 horas */
		int8_t _timeFuse;			/* Fuso */
};

#endif /* __DATETIME_H__ */
