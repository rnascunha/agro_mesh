#include "ds3231.hpp"

//Registers
#define REG_SEC		0x00
#define REG_MIN		0x01
#define REG_HOUR	0x02
#define REG_DOW		0x03
#define REG_DATE	0x04
#define REG_MON		0x05
#define REG_YEAR	0x06
#define REG_CON		0x0e
#define REG_STATUS	0x0f
#define REG_AGING	0x10
#define REG_TEMPM	0x11
#define REG_TEMPL	0x12

//Alarm 1 registers
#define REG_ALARM1_SEC		0x07
#define REG_ALARM1_MIN		0x08
#define REG_ALARM1_HOUR		0x09
#define REG_ALARM1_DATE		0x0A

//Alarm 2 regiters
#define REG_ALARM2_MIN		0x0B
#define REG_ALARM2_HOUR		0x0C
#define REG_ALARM2_DATE		0x0D

//Bits
#define BIT_AMPM		5	//1: PM | 0: AM
#define BIT_1224		6	//1: 12 | 0: 24
#define BIT_CENTURY		7

//Control register bits
#define BIT_EOSC		7
#define BIT_BBSQW		6
#define BIT_CONV		5
#define BIT_RS2			4
#define BIT_RS1			3
#define BIT_INTCN		2
#define BIT_A2IE		1
#define BIT_A1IE		0

//Status register bits
#define BIT_OSF			7
#define BIT_EN32kHz		3	//1: enable /0: disable
#define BIT_BSY			2
#define BIT_A2F			1	//1: Alarm 2 match
#define BIT_A1F			0	//1: Alarm 1 match

#define DS3231_ADDR		0b01101000	// 0x68 >> 1

DS3231::DS3231(I2C_Master *i2c, uint8_t time_mode /* = FORMAT_24H */)
	: _i2c(i2c), _time_mode(time_mode)
{}

void DS3231::begin(){
	_set_bit_reg(REG_HOUR, BIT_1224, _time_mode);
	enableAlarm1Int(false);
	enableAlarm2Int(false);
	enableInterrupt(false);
}

void DS3231::setDateTime(DateTime *dateTime)
{
	uint8_t date[] = {_encode(dateTime->getSecond()),
					_encode(dateTime->getMinute()),
					_encode(dateTime->getHour()),
					//_encode(dateTime->dayOfWeek()),
					1,
					_encode(dateTime->getDay()),
					_encode(dateTime->getMonth()),
					_encode((uint8_t)(dateTime->getYear()-2000))
					};

	_send(REG_SEC,date,7);
}

void DS3231::getDateTime(DateTime* dateTime)
{
	uint8_t date[7];

	_receive(REG_SEC, date, 7);

	dateTime->setDateTime((uint16_t)_decodeY(date[6])+2000,
							_decode(date[5]),
							_decode(date[4]),
							_decodeH(date[2]),
							_decode(date[1]),
							_decode(date[0]));
}

void DS3231::setTime(DateTime *dateTime)
{
	uint8_t date[] = {_encode(dateTime->getSecond()),
					_encode(dateTime->getMinute()),
					_encode(dateTime->getHour())
					};

	_send(REG_SEC,date,3);
}

void DS3231::getTime(DateTime* dateTime)
{
	uint8_t date[3];

	_receive(REG_SEC,date,3);

	dateTime->setTime(_decodeH(date[2]),
				_decode(date[1]),
				_decode(date[0]));
}

void DS3231::setDate(DateTime *dateTime)
{
	uint8_t date[] = {_encode(dateTime->getDay()),
					_encode(dateTime->getMonth()),
					_encode((uint8_t)(dateTime->getYear()-2000))
					};

	_send(REG_DATE,date,3);
}

void DS3231::getDate(DateTime *dateTime)
{
	uint8_t date[3];

	_receive(REG_DATE,date,3);

	dateTime->setDate((uint16_t)(_decodeY(date[2])+2000),
					_decode(date[1]),
					_decode(date[0])
					);
}

void DS3231::setTimeMode(uint8_t mode)
{
	_set_bit_reg(REG_HOUR,6,mode);
}

uint8_t DS3231::getTimeMode()
{
	return _get_bit_reg(REG_HOUR,6);
}

void DS3231::setDOW(uint8_t dow)
{
	_send(REG_DOW,dow);
}

uint8_t DS3231::getDOW()
{
	return _receive(REG_DOW);
}

void DS3231::enable32kHz(bool enable)
{
	_set_bit_reg(REG_STATUS, BIT_EN32kHz, enable);
}

void DS3231::enableInterrupt(uint8_t enable /*= true*/)
{
	_set_bit_reg(REG_CON, BIT_INTCN, enable);
}

void DS3231::enableSQWRate(DS3231SQWrate rate, uint8_t enable /*= true*/)
{
	if(enable)
		enableInterrupt(false);

	_set_bit_reg(REG_CON, BIT_BBSQW, enable);

	switch(rate){
		case SQW_RATE_1K:
			_set_bit_reg(REG_CON,BIT_RS2,false);
			_set_bit_reg(REG_CON,BIT_RS1,true);
		break;
		case SQW_RATE_4K:
			_set_bit_reg(REG_CON,BIT_RS2,true);
			_set_bit_reg(REG_CON,BIT_RS1,false);
		break;
		case SQW_RATE_8K:
			_set_bit_reg(REG_CON,BIT_RS2,true);
			_set_bit_reg(REG_CON,BIT_RS1,true);
		break;
		default: /* SQW_RATE_1 */
			_set_bit_reg(REG_CON,BIT_RS2,false);
			_set_bit_reg(REG_CON,BIT_RS1,false);
		break;
	}
}

void DS3231::enableAlarm2Int(bool enable /*= true*/)
{
	_set_bit_reg(REG_CON,BIT_A2IE,enable);
}

void DS3231::enableAlarm1Int(bool enable /*= true*/)
{
	_set_bit_reg(REG_CON,BIT_A1IE,enable);
}

uint8_t DS3231::clearAlarmFlags()
{
	uint8_t alarms = _receive(REG_STATUS) & ((1<<BIT_A2F)|(1<<BIT_A1F));

	_set_bit_reg(REG_STATUS, BIT_A2F, false);
	_set_bit_reg(REG_STATUS, BIT_A1F, false);

	return alarms;
}

void DS3231::configAlarm2(DS3231Alarm2Config type_alarm, DateTime *dateTime /* = NULL */)
{
	switch(type_alarm){
		case PER_MINUTE:
			_set_bit_reg(REG_ALARM2_DATE, 7);
			_set_bit_reg(REG_ALARM2_HOUR, 7);
			_set_bit_reg(REG_ALARM2_MIN, 7);
		break;
		case MINUTES_MATCH:
			_send(REG_ALARM2_MIN, _encode(dateTime->getMinute()));
			_set_bit_reg(REG_ALARM2_DATE, 7);
			_set_bit_reg(REG_ALARM2_HOUR, 7);
			_set_bit_reg(REG_ALARM2_MIN, 7, false);
		break;
		case HOUR_MIN_MATCH:
		{
			uint8_t data[] = {_encode(dateTime->getMinute()),
					_encode(dateTime->getHour())};
			_send(REG_ALARM2_MIN, data, 2);
			_set_bit_reg(REG_ALARM2_DATE, 7);
			_set_bit_reg(REG_ALARM2_HOUR, 7, false);
			_set_bit_reg(REG_ALARM2_MIN, 7, false);
		}
		break;
		case DATE_HOUR_MIN_MATCH:
		{
			uint8_t data[] = {_encode(dateTime->getMinute()),
					_encode(dateTime->getHour()),
					_encode(dateTime->getDay())};
			_send(REG_ALARM2_MIN, data, 3);
			_set_bit_reg(REG_ALARM2_DATE, 7, false);
			_set_bit_reg(REG_ALARM2_HOUR, 7, false);
			_set_bit_reg(REG_ALARM2_MIN, 7, false);
			_set_bit_reg(REG_ALARM2_DATE, 6, false);
		}
		break;
		case DAY_HOUR_MIN_MATCH:
		{
			uint8_t data[] = {_encode(dateTime->getMinute()),
					_encode(dateTime->getHour()),
					_encode(dateTime->dayOfWeek())};
			_send(REG_ALARM2_MIN, data, 3);
			_set_bit_reg(REG_ALARM2_DATE, 7, false);
			_set_bit_reg(REG_ALARM2_HOUR, 7, false);
			_set_bit_reg(REG_ALARM2_MIN, 7, false);
			_set_bit_reg(REG_ALARM2_DATE, 6, true);
		}
		break;
		default:
		break;
	}
}

void DS3231::configAlarm1(DS3231Alarm1Config type_alarm, DateTime *dateTime /* = NULL */)
{
	switch(type_alarm){
		case PER_SECOND:
			_set_bit_reg(REG_ALARM1_DATE, 7);
			_set_bit_reg(REG_ALARM1_HOUR, 7);
			_set_bit_reg(REG_ALARM1_MIN, 7);
			_set_bit_reg(REG_ALARM1_SEC, 7);
		break;
		case SECONDS_MATCH:
			_send(REG_ALARM1_SEC, _encode(dateTime->getSecond()));
			_set_bit_reg(REG_ALARM1_DATE, 7);
			_set_bit_reg(REG_ALARM1_HOUR, 7);
			_set_bit_reg(REG_ALARM1_MIN, 7);
			_set_bit_reg(REG_ALARM1_SEC, 7, false);
		break;
		case MIN_SEC_MATCH:
		{
			uint8_t data[] = {_encode(dateTime->getSecond()),
					_encode(dateTime->getMinute())};
			_send(REG_ALARM1_SEC, data, 2);
			_set_bit_reg(REG_ALARM1_DATE, 7);
			_set_bit_reg(REG_ALARM1_HOUR, 7);
			_set_bit_reg(REG_ALARM1_MIN, 7, false);
			_set_bit_reg(REG_ALARM1_SEC, 7, false);
		}
		break;
		case HOUR_MIN_SEC_MATCH:
		{
			uint8_t data[] = {_encode(dateTime->getSecond()),
					_encode(dateTime->getMinute()),
					_encode(dateTime->getHour())};
			_send(REG_ALARM1_SEC, data, 3);
			_set_bit_reg(REG_ALARM1_DATE, 7);
			_set_bit_reg(REG_ALARM1_HOUR, 7, false);
			_set_bit_reg(REG_ALARM1_MIN, 7, false);
			_set_bit_reg(REG_ALARM1_SEC, 7, false);
		}
		break;
		case DATE_HOUR_MIN_SEC_MATCH:
		{
			uint8_t data[] = {_encode(dateTime->getSecond()),
					_encode(dateTime->getMinute()),
					_encode(dateTime->getHour()),
					_encode(dateTime->getDay())};
			_send(REG_ALARM1_SEC,data, 4);
			_set_bit_reg(REG_ALARM1_DATE, 7, false);
			_set_bit_reg(REG_ALARM1_HOUR, 7, false);
			_set_bit_reg(REG_ALARM1_MIN, 7, false);
			_set_bit_reg(REG_ALARM1_SEC, 7, false);
			_set_bit_reg(REG_ALARM1_DATE, 6, false);
		}
		break;
		case DAY_HOUR_MIN_SEC_MATCH:
		{
			uint8_t data[] = {_encode(dateTime->getSecond()),
					_encode(dateTime->getMinute()),
					_encode(dateTime->getHour()),
					_encode(dateTime->dayOfWeek())};
			_send(REG_ALARM1_SEC,data, 4);
			_set_bit_reg(REG_ALARM1_DATE, 7, false);
			_set_bit_reg(REG_ALARM1_HOUR, 7, false);
			_set_bit_reg(REG_ALARM1_MIN, 7, false);
			_set_bit_reg(REG_ALARM1_SEC, 7, false);
			_set_bit_reg(REG_ALARM1_DATE, 6);
		}
		break;
		default:
		break;
	}
}

void DS3231::startConvTemp()
{
	if(_get_bit_reg(REG_CON, BIT_CONV))
		return;

	_set_bit_reg(REG_CON, BIT_CONV);
}

float DS3231::getTemp()
{
	uint8_t data[2];

	_receive(REG_TEMPM, data, 2);

	return (float)data[0] + ((data[1] >> 6) * 0.25f);
}

/* Private */
void DS3231::_send(uint8_t reg, uint8_t* data, size_t length)
{
	_i2c->write(DS3231_ADDR, reg, data, length, true);
}

void DS3231::_send(uint8_t reg,uint8_t data)
{
	_i2c->write(DS3231_ADDR, reg, data, true);
}

void DS3231::_receive(uint8_t reg,uint8_t* data, size_t length)
{
	_i2c->read(DS3231_ADDR, reg, data, length, true);
}

uint8_t DS3231::_receive(uint8_t reg)
{
	uint8_t data;
	_i2c->read(DS3231_ADDR, reg, &data, true);

	return data;
}

void DS3231::_set_bit_reg(uint8_t reg, uint8_t bit, bool value /* = true*/)
{
	uint8_t r = _receive(reg);

	r &= ~(1 << bit);
	r |= (value << bit);
	_send(reg, r);
}

uint8_t DS3231::_get_bit_reg(uint8_t reg, uint8_t bit)
{
	return (_receive(reg) >> bit) & 1;
}

uint8_t	DS3231::_decode(uint8_t value)
{
	uint8_t decoded = value & 127;
	decoded = (decoded & 15) + 10 * ((decoded & (15 << 4)) >> 4);
	return decoded;
}

uint8_t DS3231::_decodeH(uint8_t value)
{
  if (value & 128)
    value = (value & 15) + (12 * ((value & 32) >> 5));
  else
    value = (value & 15) + (10 * ((value & 48) >> 4));
  return value;
}

uint8_t	DS3231::_decodeY(uint8_t value)
{
	return (value & 15) + 10 * ((value & (15 << 4)) >> 4);
}

uint8_t DS3231::_encode(uint8_t value)
{
	return ((value / 10) << 4) + (value % 10);
}
