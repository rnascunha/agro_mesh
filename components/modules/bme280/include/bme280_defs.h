#ifndef BME280_DEFS_H__
#define BME280_DEFS_H__

#define SLAVE_ADDR_BASE		 	0b01110110

#define BME280_GET_BITS(reg_data, bitname)       ((reg_data & (bitname##_MSK)) >> \
                                                  (bitname##_POS))
#define BME280_GET_BITS_POS_0(reg_data, bitname) (reg_data & (bitname##_MSK))

#define BME280_SET_BITS(reg_data, bitname, data) \
    ((reg_data & ~(bitname##_MSK)) | \
     ((data << bitname##_POS) & bitname##_MSK))
#define BME280_SET_BITS_POS_0(reg_data, bitname, data) \
    ((reg_data & ~(bitname##_MSK)) | \
     (data & bitname##_MSK))

/**\name Macros for bit masking */
#define BME280_SENSOR_MODE_MSK      (0x03)
#define BME280_SENSOR_MODE_POS      (0x00)

#define BME280_CTRL_HUM_MSK         (0x07)
#define BME280_CTRL_HUM_POS         (0x00)

#define BME280_CTRL_PRESS_MSK       (0x1C)
#define BME280_CTRL_PRESS_POS       (0x02)

#define BME280_CTRL_TEMP_MSK        (0xE0)
#define BME280_CTRL_TEMP_POS        (0x05)

#define BME280_FILTER_MSK           (0x1C)
#define BME280_FILTER_POS           (0x02)

#define BME280_STANDBY_MSK          (0xE0)
#define BME280_STANDBY_POS          (0x05)

class BME280_defs{
	public:
		struct uncomp_data{
			/*! un-compensated pressure */
			uint32_t pressure;

			/*! un-compensated temperature */
			uint32_t temperature;

			/*! un-compensated humidity */
			uint32_t humidity;
		};

		enum Regs{
			CHIP_ID 				= 0xD0,
			RESET                  	= 0xE0,
			TEMP_PRESS_CALIB_DATA  	= 0x88,
			HUMIDITY_CALIB_DATA    	= 0xE1,
			PWR_CTRL               	= 0xF4,
			CTRL_HUM               	= 0xF2,
			CTRL_MEAS              	= 0xF4,
			STATUS					= 0xF3,
			CONFIG                 	= 0xF5,
			DATA                   	= 0xF7,
		};

		enum Reset{
			POWER_ON_RESET			= 0xB6
		};

		enum Status{
			MEASURING				= 0b1000,
			IM_UPDATE				= 0b0001
		};

		enum Ctrl_Measurument{
			OSRS_TEMPERATURE		= 5,
			OSRS_PRESSURE			= 2,
			MODE					= 0
		};

		enum Config{
			STANDBY_TIME	 		= 5,
			IRR_FILTER_CONST		= 2,
			SPI_3_WIRE				= 0
		};

		static const uint8_t TEMP_PRESS_CALIB_DATA_LEN = 26;
		static const uint8_t HUMIDITY_CALIB_DATA_LEN = 7;
		static const uint8_t P_T_H_DATA_LEN = 8;

};

#endif /* BME280_DEFS_H__ */
