
#include "datetime.h"
//#include "support.h"

static const uint8_t _month_length[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
#define SEC_1970_TO_2000 946684800

template <class T>
static inline const T& constrain(const T& n, const T& a, const T& b) {
  return ( n < a ) ? a : ( n > b ? b : n );
}

DateTime::DateTime(TimeMode mode /* = FOMART_24H */, int fuse /* = 0 */)
{
	_timeMode = mode;
	_timeFuse = fuse;
	_period = ND;
	setDateTime(0,0,0,0,0,0);
}

void DateTime::setYear(uint16_t year)
{
	_year = year;
}

void DateTime::setMonth(uint8_t month)
{
	_month = constrain((uint8_t)month,(uint8_t)1,(uint8_t)12);
}

void DateTime::setDay(uint8_t day)
{
	uint8_t max_day = _month_length[_month-1];

	max_day = (_month == 2 && isLeapYear()) ? max_day + 1 : max_day;

	_day = constrain((uint8_t)day,(uint8_t)1,(uint8_t)max_day);
}

void DateTime::setHour(uint8_t hour, DayPeriod period /* = ND */)
{
	if(_timeMode == FORMAT_12H){
		_period = (period == ND) ? AM : period;
		if(_period == PM)
			_hour += 12;
	}
	_hour = constrain((uint8_t)hour,(uint8_t)0,(uint8_t)23);
}

void DateTime::setMinute(uint8_t minute)
{
	_minute = constrain((uint8_t)minute,(uint8_t)0,(uint8_t)59);
}

void DateTime::setSecond(uint8_t second)
{
	_second = constrain((uint8_t)second,(uint8_t)0,(uint8_t)59);
}

void DateTime::setDate(uint16_t year, uint8_t month,uint8_t day)
{
	setYear(year);
	setMonth(month);
	setDay(day);
}
void DateTime::setTime(uint8_t hour, uint8_t minute,uint8_t second, DayPeriod period /* = ND */)
{
	setHour(hour,period);
	setMinute(minute);
	setSecond(second);
}

void DateTime::setDateTime(uint16_t year, uint8_t month,uint16_t day,
		uint8_t hour, uint8_t minute,uint8_t second, DayPeriod period /* = ND */)
{
	setDate(year,month,day);
	setTime(hour,minute,second,period);
}

#define SECONDS_DAY		86400

void DateTime::setUnixTime(uint32_t unixTime)
{
	unsigned long day,mins,secs,year,leap;

	day = unixTime/(24L*60*60);
	secs = unixTime % (24L*60*60);
	_second = secs % 60;
	mins = secs / 60;
	_hour = mins / 60;
	_minute = mins % 60;
	year = (((day * 4) + 2)/1461);
	_year = year + 1970;
	leap = !(_year & 3);
	day -= ((year * 1461) + 1) / 4;
	_day = day;
	day += (day > 58 + leap) ? ((leap) ? 1 : 2) : 0;
	_month = ((day * 12) + 6)/367;
	_day = day + 1 - ((_month * 367) + 5)/12;
}

uint16_t DateTime::getYear(void)
{
	return _year;
}

uint8_t DateTime::getMonth(void)
{
	return _month;
}

uint8_t DateTime::getDay(void)
{
	return _day;
}

void DateTime::getDate(DateTime *date)
{
	date->_year = getYear();
	date->_month = getMonth();
	date->_day = getDay();
}

uint8_t DateTime::getHour(void)
{
	if(_timeMode == FORMAT_12H && _period == PM){
		return _hour - 12;
	}
	return _hour;
}

DayPeriod DateTime::getDayPeriod()
{
	return _hour < 12 ? AM : PM;
}

uint8_t DateTime::getMinute(void)
{
	return _minute;
}

uint8_t DateTime::getSecond(void)
{
	return _second;
}

TimeMode DateTime::getTimeMode(void)
{
	return _timeMode;
}

int8_t DateTime::getTimeFuse(void)
{
	return _timeFuse;
}

void DateTime::getTime(DateTime *time)
{
	time->_hour = getHour();
	time->_minute = getMinute();
	time->_second = getSecond();
	time->_period = getDayPeriod();
	time->_timeMode = getTimeMode();
	time->_timeFuse = getTimeFuse();
}

void DateTime::getDateTime(DateTime *dateTime)
{
	getDate(dateTime);
	getTime(dateTime);
}

/*
 * https://en.wikipedia.org/wiki/Leap_year
 * * Não considera a exceção de anos divisiveis por 100 não é ano bissexto
 * * Não considera a exceção da exceção que anos diviveis por 400 é ano bissexto
 */
bool DateTime::isLeapYear()
{
	return (_year % 4) ? false : true;
}

/*
 * número de segundos desde 1 de janeiro de 1970 UTC
 * Algoritimo só funciona para depois do ano 2000
 */
uint32_t DateTime::getUnixTime()
{
	uint16_t dc;
	dc = _day;
	for (uint8_t i = 0; i<(_month-1); i++)
		dc += _month_length[i];
	if ((_month > 2) && isLeapYear())
		++dc;
	dc = dc + (365 * (_year - 2000)) + (((_year - 2000) + 3) / 4) - 1;

	return ((((((dc * 24UL) + _hour) * 60) + _minute) * 60) + _second) + SEC_1970_TO_2000;
}

/*
 * https://en.wikipedia.org/wiki/Zeller%27s_congruence
 */
DayOfWeek DateTime::dayOfWeek(void)
{
	return (DayOfWeek)(((((_year * 365 + ((_year-1) / 4) - ((_year-1) / 100) + ((_year-1) / 400)) % 7) + 5) % 7) + 1);
}
