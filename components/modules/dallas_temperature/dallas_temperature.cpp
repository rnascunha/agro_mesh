// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.

// Version 3.7.2 modified on Dec 6, 2011 to support Arduino 1.0
// See Includes...
// Modified by Jordan Hochenbaum

#include "dallas_temperature.h"
#include "hardware_defs.h"

template <class T> const T& max (const T& a, const T& b) {
  return ( a < b ) ? b : a;
}

template <class T> const T& constrain(const T& n, const T& a, const T& b) {
  return ( n < a ) ? a : ( n > b ? b : n );
}

Dallas_Temperature::Dallas_Temperature(OneWire* wire)
  : _AlarmHandler(&defaultAlarmHandler)
{
	this->wire = wire;
	devices = 0;
	parasite = false;
	bitResolution = 9;
	waitForConversion = true;
	checkForConversion = true;

	resetAlarmSearch();
}

// initialise the bus
void Dallas_Temperature::begin(void)
{
  DeviceAddress deviceAddress;

  wire->reset_search();
  devices = 0; // Reset the number of devices when we enumerate wire devices

  while (wire->search(deviceAddress))
  {
    if (validAddress(deviceAddress))
    {
      if (!parasite && readPowerSupply(deviceAddress)) parasite = true;

      ScratchPad scratchPad;

      readScratchPad(deviceAddress, scratchPad);

	  bitResolution = max(bitResolution, getResolution(deviceAddress));

      devices++;
    }
  }
}

// returns the number of devices found on the bus
uint8_t Dallas_Temperature::getDeviceCount(void)
{
  return devices;
}

// returns true if address is valid
bool Dallas_Temperature::validAddress(uint8_t* deviceAddress)
{
  return (wire->crc8(deviceAddress, 7) == deviceAddress[7]);
}

// finds an address at a given index on the bus
// returns true if the device was found
bool Dallas_Temperature::getAddress(uint8_t* deviceAddress, uint8_t index)
{
  uint8_t depth = 0;

  wire->reset_search();

  while (depth <= index && wire->search(deviceAddress))
  {
    if (depth == index && validAddress(deviceAddress)) return true;
    depth++;
  }

  return false;
}

// attempt to determine if the device at the given address is connected to the bus
bool Dallas_Temperature::isConnected(uint8_t* deviceAddress)
{
  ScratchPad scratchPad;
  return isConnected(deviceAddress, scratchPad);
}

// attempt to determine if the device at the given address is connected to the bus
// also allows for updating the read scratchpad
bool Dallas_Temperature::isConnected(uint8_t* deviceAddress, uint8_t* scratchPad)
{
  readScratchPad(deviceAddress, scratchPad);
  return (wire->crc8(scratchPad, 8) == scratchPad[SCRATCHPAD_CRC]);
}

// read device's scratch pad
void Dallas_Temperature::readScratchPad(uint8_t* deviceAddress, uint8_t* scratchPad)
{
  // send the command
  wire->reset();
  wire->select(deviceAddress);
  wire->write(READSCRATCH);

  // TODO => collect all comments &  use simple loop
  // byte 0: temperature LSB
  // byte 1: temperature MSB
  // byte 2: high alarm temp
  // byte 3: low alarm temp
  // byte 4: DS18S20: store for crc
  //         DS18B20 & DS1822: configuration register
  // byte 5: internal use & crc
  // byte 6: DS18S20: COUNT_REMAIN
  //         DS18B20 & DS1822: store for crc
  // byte 7: DS18S20: COUNT_PER_C
  //         DS18B20 & DS1822: store for crc
  // byte 8: SCRATCHPAD_CRC
  //
  // for(int i=0; i<9; i++)
  // {
  //   scratchPad[i] = wire->read();
  // }


  // read the response

  // byte 0: temperature LSB
  scratchPad[TEMP_LSB] = wire->read();

  // byte 1: temperature MSB
  scratchPad[TEMP_MSB] = wire->read();

  // byte 2: high alarm temp
  scratchPad[HIGH_ALARM_TEMP] = wire->read();

  // byte 3: low alarm temp
  scratchPad[LOW_ALARM_TEMP] = wire->read();

  // byte 4:
  // DS18S20: store for crc
  // DS18B20 & DS1822: configuration register
  scratchPad[CONFIGURATION] = wire->read();

  // byte 5:
  // internal use & crc
  scratchPad[INTERNAL_BYTE] = wire->read();

  // byte 6:
  // DS18S20: COUNT_REMAIN
  // DS18B20 & DS1822: store for crc
  scratchPad[COUNT_REMAIN] = wire->read();

  // byte 7:
  // DS18S20: COUNT_PER_C
  // DS18B20 & DS1822: store for crc
  scratchPad[COUNT_PER_C] = wire->read();

  // byte 8:
  // SCTRACHPAD_CRC
  scratchPad[SCRATCHPAD_CRC] = wire->read();

  wire->reset();
}

// writes device's scratch pad
void Dallas_Temperature::writeScratchPad(uint8_t* deviceAddress, const uint8_t* scratchPad)
{
  wire->reset();
  wire->select(deviceAddress);
  wire->write(WRITESCRATCH);
  wire->write(scratchPad[HIGH_ALARM_TEMP]); // high alarm temp
  wire->write(scratchPad[LOW_ALARM_TEMP]); // low alarm temp
  // DS18S20 does not use the configuration register
  if (deviceAddress[0] != DS18S20MODEL) wire->write(scratchPad[CONFIGURATION]); // configuration
  wire->reset();
  // save the newly written values to eeprom
  wire->write(COPYSCRATCH, parasite);
  if (parasite) delay_ms(10); // 10ms delay
  wire->reset();
}

// reads the device's power requirements
bool Dallas_Temperature::readPowerSupply(uint8_t* deviceAddress)
{
  bool ret = false;
  wire->reset();
  wire->select(deviceAddress);
  wire->write(READPOWERSUPPLY);
  if (wire->read_bit() == 0) ret = true;
  wire->reset();
  return ret;
}

// set resolution of all devices to 9, 10, 11, or 12 bits
// if new resolution is out of range, it is constrained.
void Dallas_Temperature::setResolution(uint8_t newResolution)
{
  bitResolution = constrain(newResolution, (uint8_t)9, (uint8_t)12);
  DeviceAddress deviceAddress;
  for (int i=0; i<devices; i++)
  {
    getAddress(deviceAddress, i);
	setResolution(deviceAddress, bitResolution);
  }
}

// set resolution of a device to 9, 10, 11, or 12 bits
// if new resolution is out of range, 9 bits is used.
bool Dallas_Temperature::setResolution(uint8_t* deviceAddress, uint8_t newResolution)
{
  ScratchPad scratchPad;
  if (isConnected(deviceAddress, scratchPad))
  {
    // DS18S20 has a fixed 9-bit resolution
    if (deviceAddress[0] != DS18S20MODEL)
    {
      switch (newResolution)
      {
        case 12:
          scratchPad[CONFIGURATION] = TEMP_12_BIT;
          break;
        case 11:
          scratchPad[CONFIGURATION] = TEMP_11_BIT;
          break;
        case 10:
          scratchPad[CONFIGURATION] = TEMP_10_BIT;
          break;
        case 9:
        default:
          scratchPad[CONFIGURATION] = TEMP_9_BIT;
          break;
      }
      writeScratchPad(deviceAddress, scratchPad);
    }
	return true;  // new value set
  }
  return false;
}

// returns the global resolution
uint8_t Dallas_Temperature::getResolution()
{
	return bitResolution;
}

// returns the current resolution of the device, 9-12
// returns 0 if device not found
uint8_t Dallas_Temperature::getResolution(uint8_t* deviceAddress)
{
  if (deviceAddress[0] == DS18S20MODEL) return 9; // this model has a fixed resolution

  ScratchPad scratchPad;
  if (isConnected(deviceAddress, scratchPad))
  {
	switch (scratchPad[CONFIGURATION])
    {
      case TEMP_12_BIT:
        return 12;

      case TEMP_11_BIT:
        return 11;

      case TEMP_10_BIT:
        return 10;

      case TEMP_9_BIT:
        return 9;

	}
  }
  return 0;
}


// sets the value of the waitForConversion flag
// TRUE : function requestTemperature() etc returns when conversion is ready
// FALSE: function requestTemperature() etc returns immediately (USE WITH CARE!!)
// 		  (1) programmer has to check if the needed delay has passed
//        (2) but the application can do meaningful things in that time
void Dallas_Temperature::setWaitForConversion(bool flag)
{
	waitForConversion = flag;
}

// gets the value of the waitForConversion flag
bool Dallas_Temperature::getWaitForConversion()
{
	return waitForConversion;
}


// sets the value of the checkForConversion flag
// TRUE : function requestTemperature() etc will 'listen' to an IC to determine whether a conversion is complete
// FALSE: function requestTemperature() etc will wait a set time (worst case scenario) for a conversion to complete
void Dallas_Temperature::setCheckForConversion(bool flag)
{
	checkForConversion = flag;
}

// gets the value of the waitForConversion flag
bool Dallas_Temperature::getCheckForConversion()
{
	return checkForConversion;
}

bool Dallas_Temperature::isConversionAvailable(uint8_t* deviceAddress)
{
	// Check if the clock has been raised indicating the conversion is complete
  	ScratchPad scratchPad;
  	readScratchPad(deviceAddress, scratchPad);
	return scratchPad[0];
}


// sends command for all devices on the bus to perform a temperature conversion
void Dallas_Temperature::requestTemperatures()
{
  wire->reset();
  wire->skip();
  wire->write(STARTCONVO, parasite);

  // ASYNC mode?
  if (!waitForConversion) return;
  blockTillConversionComplete(&bitResolution, 0);

  return;
}

// sends command for one device to perform a temperature by address
// returns FALSE if device is disconnected
// returns TRUE  otherwise
bool Dallas_Temperature::requestTemperaturesByAddress(uint8_t* deviceAddress)
{

  wire->reset();
  wire->select(deviceAddress);
  wire->write(STARTCONVO, parasite);

    // check device
  ScratchPad scratchPad;
  if (!isConnected(deviceAddress, scratchPad)) return false;


  // ASYNC mode?
  if (!waitForConversion) return true;
  uint8_t bitResolution = getResolution(deviceAddress);
  blockTillConversionComplete(&bitResolution, deviceAddress);

  return true;
}


void Dallas_Temperature::blockTillConversionComplete(uint8_t* bitResolution, uint8_t* deviceAddress)
{
	if(deviceAddress != 0 && checkForConversion && !parasite)
	{
	  	// Continue to check if the IC has responded with a temperature
	  	// NB: Could cause issues with multiple devices (one device may respond faster)
//	  	unsigned long start = millis();
//		while(!isConversionAvailable(0) && ((millis() - start) < 750));
		uint8_t timeout = 15;
		while(!isConversionAvailable(0) && timeout-- != 0)
			delay_ms(50);
	}

  	// Wait a fix number of cycles till conversion is complete (based on IC datasheet)
	  switch (*bitResolution)
	  {
	    case 9:
	    	delay_ms(94);
	      break;
	    case 10:
	    	delay_ms(188);
	      break;
	    case 11:
	    	delay_ms(375);
	      break;
	    case 12:
	    default:
	    	delay_ms(750);
	      break;
	  }

}

// sends command for one device to perform a temp conversion by index
bool Dallas_Temperature::requestTemperaturesByIndex(uint8_t deviceIndex)
{
  DeviceAddress deviceAddress;
  getAddress(deviceAddress, deviceIndex);
  return requestTemperaturesByAddress(deviceAddress);
}

// Fetch temperature for device index
float Dallas_Temperature::getTempCByIndex(uint8_t deviceIndex)
{
  DeviceAddress deviceAddress;
  getAddress(deviceAddress, deviceIndex);
  return getTempC((uint8_t*)deviceAddress);
}

// reads scratchpad and returns the temperature in degrees C
float Dallas_Temperature::calculateTemperature(uint8_t* deviceAddress, uint8_t* scratchPad)
{
  int16_t rawTemperature = (((int16_t)scratchPad[TEMP_MSB]) << 8) | scratchPad[TEMP_LSB];

  switch (deviceAddress[0])
  {
    case DS18B20MODEL:
    case DS1822MODEL:
      switch (scratchPad[CONFIGURATION])
      {
        case TEMP_12_BIT:
          return (float)rawTemperature * 0.0625;
          break;
        case TEMP_11_BIT:
          return (float)(rawTemperature >> 1) * 0.125;
          break;
        case TEMP_10_BIT:
          return (float)(rawTemperature >> 2) * 0.25;
          break;
        case TEMP_9_BIT:
          return (float)(rawTemperature >> 3) * 0.5;
          break;
      }
      break;
    case DS18S20MODEL:
      /*

      Resolutions greater than 9 bits can be calculated using the data from
      the temperature, COUNT REMAIN and COUNT PER �C registers in the
      scratchpad. Note that the COUNT PER �C register is hard-wired to 16
      (10h). After reading the scratchpad, the TEMP_READ value is obtained
      by truncating the 0.5�C bit (bit 0) from the temperature data. The
      extended resolution temperature can then be calculated using the
      following equation:

                                       COUNT_PER_C - COUNT_REMAIN
      TEMPERATURE = TEMP_READ - 0.25 + --------------------------
                                               COUNT_PER_C
      */

      // Good spot. Thanks Nic Johns for your contribution
      return (float)(rawTemperature >> 1) - 0.25 +((float)(scratchPad[COUNT_PER_C] - scratchPad[COUNT_REMAIN]) / (float)scratchPad[COUNT_PER_C] );
      break;
  }
  return false;
}

// returns temperature in degrees C or DEVICE_DISCONNECTED if the
// device's scratch pad cannot be read successfully.
// the numeric value of DEVICE_DISCONNECTED is defined in
// DallasTemperature.h. It is a large negative number outside the
// operating range of the device
float Dallas_Temperature::getTempC(uint8_t* deviceAddress)
{
  // TODO: Multiple devices (up to 64) on the same bus may take
  //       some time to negotiate a response
  // What happens in case of collision?

  ScratchPad scratchPad;
  if (isConnected(deviceAddress, scratchPad)) return calculateTemperature(deviceAddress, scratchPad);
  return DEVICE_DISCONNECTED;
}

// returns true if the bus requires parasite power
bool Dallas_Temperature::isParasitePowerMode(void)
{
  return parasite;
}

/*

ALARMS:

TH and TL Register Format

BIT 7 BIT 6 BIT 5 BIT 4 BIT 3 BIT 2 BIT 1 BIT 0
  S    2^6   2^5   2^4   2^3   2^2   2^1   2^0

Only bits 11 through 4 of the temperature register are used
in the TH and TL comparison since TH and TL are 8-bit
registers. If the measured temperature is lower than or equal
to TL or higher than or equal to TH, an alarm condition exists
and an alarm flag is set inside the DS18B20. This flag is
updated after every temperature measurement; therefore, if the
alarm condition goes away, the flag will be turned off after
the next temperature conversion.

*/

// sets the high alarm temperature for a device in degrees celsius
// accepts a float, but the alarm resolution will ignore anything
// after a decimal point.  valid range is -55C - 125C
void Dallas_Temperature::setHighAlarmTemp(uint8_t* deviceAddress, signed char celsius)
{
  // make sure the alarm temperature is within the device's range
  if (celsius > 125) celsius = 125;
  else if (celsius < -55) celsius = -55;

  ScratchPad scratchPad;
  if (isConnected(deviceAddress, scratchPad))
  {
    scratchPad[HIGH_ALARM_TEMP] = (uint8_t)celsius;
    writeScratchPad(deviceAddress, scratchPad);
  }
}

// sets the low alarm temperature for a device in degreed celsius
// accepts a float, but the alarm resolution will ignore anything
// after a decimal point.  valid range is -55C - 125C
void Dallas_Temperature::setLowAlarmTemp(uint8_t* deviceAddress, signed char celsius)
{
  // make sure the alarm temperature is within the device's range
  if (celsius > 125) celsius = 125;
  else if (celsius < -55) celsius = -55;

  ScratchPad scratchPad;
  if (isConnected(deviceAddress, scratchPad))
  {
    scratchPad[LOW_ALARM_TEMP] = (uint8_t)celsius;
    writeScratchPad(deviceAddress, scratchPad);
  }
}

// returns a char with the current high alarm temperature or
// DEVICE_DISCONNECTED for an address
char Dallas_Temperature::getHighAlarmTemp(uint8_t* deviceAddress)
{
  ScratchPad scratchPad;
  if (isConnected(deviceAddress, scratchPad)) return (char)scratchPad[HIGH_ALARM_TEMP];
  return DEVICE_DISCONNECTED;
}

// returns a char with the current low alarm temperature or
// DEVICE_DISCONNECTED for an address
char Dallas_Temperature::getLowAlarmTemp(uint8_t* deviceAddress)
{
  ScratchPad scratchPad;
  if (isConnected(deviceAddress, scratchPad)) return (char)scratchPad[LOW_ALARM_TEMP];
  return DEVICE_DISCONNECTED;
}

// resets internal variables used for the alarm search
void Dallas_Temperature::resetAlarmSearch()
{
  alarmSearchJunction = -1;
  alarmSearchExhausted = 0;
  for(uint8_t i = 0; i < 7; i++)
    alarmSearchAddress[i] = 0;
}

// This is a modified version of the OneWire::search method.
//
// Also added the OneWire search fix documented here:
// http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1238032295
//
// Perform an alarm search. If this function returns a '1' then it has
// enumerated the next device and you may retrieve the ROM from the
// OneWire::address variable. If there are no devices, no further
// devices, or something horrible happens in the middle of the
// enumeration then a 0 is returned.  If a new device is found then
// its address is copied to newAddr.  Use
// DallasTemperature::resetAlarmSearch() to start over.
bool Dallas_Temperature::alarmSearch(uint8_t* newAddr)
{
  uint8_t i;
  char lastJunction = -1;
  uint8_t done = 1;

  if (alarmSearchExhausted) return false;
  if (!wire->reset()) return false;

  // send the alarm search command
  wire->write(0xEC, 0);

  for(i = 0; i < 64; i++)
  {
    uint8_t a = wire->read_bit( );
    uint8_t nota = wire->read_bit( );
    uint8_t ibyte = i / 8;
    uint8_t ibit = 1 << (i & 7);

    // I don't think this should happen, this means nothing responded, but maybe if
    // something vanishes during the search it will come up.
    if (a && nota) return false;

    if (!a && !nota)
    {
      if (i == alarmSearchJunction)
      {
        // this is our time to decide differently, we went zero last time, go one.
        a = 1;
        alarmSearchJunction = lastJunction;
      }
      else if (i < alarmSearchJunction)
      {
        // take whatever we took last time, look in address
        if (alarmSearchAddress[ibyte] & ibit) a = 1;
        else
        {
          // Only 0s count as pending junctions, we've already exhasuted the 0 side of 1s
          a = 0;
          done = 0;
          lastJunction = i;
        }
      }
      else
      {
        // we are blazing new tree, take the 0
        a = 0;
        alarmSearchJunction = i;
        done = 0;
      }
      // OneWire search fix
      // See: http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1238032295
    }

    if (a) alarmSearchAddress[ibyte] |= ibit;
    else alarmSearchAddress[ibyte] &= ~ibit;

    wire->write_bit(a);
  }

  if (done) alarmSearchExhausted = 1;
  for (i = 0; i < 8; i++) newAddr[i] = alarmSearchAddress[i];
  return true;
}

// returns true if device address has an alarm condition
// TODO: can this be done with only TEMP_MSB REGISTER (faster)
//       if ((char) scratchPad[TEMP_MSB] <= (char) scratchPad[LOW_ALARM_TEMP]) return true;
//       if ((char) scratchPad[TEMP_MSB] >= (char) scratchPad[HIGH_ALARM_TEMP]) return true;
bool Dallas_Temperature::hasAlarm(uint8_t* deviceAddress)
{
  ScratchPad scratchPad;
  if (isConnected(deviceAddress, scratchPad))
  {
    float temp = calculateTemperature(deviceAddress, scratchPad);

    // check low alarm
    if ((char)temp <= (char)scratchPad[LOW_ALARM_TEMP]) return true;

    // check high alarm
    if ((char)temp >= (char)scratchPad[HIGH_ALARM_TEMP]) return true;
  }

  // no alarm
  return false;
}

// returns true if any device is reporting an alarm condition on the bus
bool Dallas_Temperature::hasAlarm(void)
{
  DeviceAddress deviceAddress;
  resetAlarmSearch();
  return alarmSearch(deviceAddress);
}

// runs the alarm handler for all devices returned by alarmSearch()
void Dallas_Temperature::processAlarms(void)
{
  resetAlarmSearch();
  DeviceAddress alarmAddr;

  while (alarmSearch(alarmAddr))
  {
    if (validAddress(alarmAddr))
      _AlarmHandler(alarmAddr);
  }
}

// sets the alarm handler
void Dallas_Temperature::setAlarmHandler(AlarmHandler *handler)
{
  _AlarmHandler = handler;
}

// The default alarm handler
void Dallas_Temperature::defaultAlarmHandler(uint8_t* deviceAddress)
{
}
