#ifndef BME280_H__
#define BME280_H__

#include <stdint.h>
#include "sdkconfig.h"

#ifdef CONFIG_BME280_FLOAT_ENABLE
#	define BME280_FLOAT_ENABLE
#else
#	if CONFIG_BME280_64BIT_ENABLE
#		define BME280_64BIT_ENABLE		1
#	endif /* CONFIG_BME280_64BIT_ENABLE */
#endif /* CONFIG_BME280_FLOAT_ENABLE */

#define BME280_ERR_OK				1
#define BME280_ERR_CHIP_ID			-1
#define BME280_ERR_INIT_ADDR		-2
#define BME280_ERR_SOFT_RESET		-3
#define BME280_ERR_SOFT_RESET_WAIT	-4
#define BME280_ERR_CALIB_DATA		-5
#define BME280_ERR_INVALID_OSR		-6
#define BME280_ERR_CONFIG_SAMP		-7
#define BME280_ERR_CONFIG_FIL_STAND	-8
#define BME_ERR_GET_DATA			-9

/*
 * Data choose
 */
#define BME280_PRESS                (1)
#define BME280_TEMP                 (1 << 1)
#define BME280_HUM                  (1 << 2)
#define BME280_ALL                  (0x07)

/**
 * Config
 */
#define BME280_OSR_PRESS_SEL        (1)
#define BME280_OSR_TEMP_SEL         (1 << 1)
#define BME280_OSR_HUM_SEL          (1 << 2)
#define BME280_FILTER_SEL           (1 << 3)
#define BME280_STANDBY_SEL          (1 << 4)
#define BME280_ALL_SETTINGS_SEL     (0x1F)

class BME280{
	public:
		static const uint8_t CHIP_ID = 0x60;

		enum Stand_By_Time_ms{
			STANDBY_TIME_0_5_MS		= 0b000,
			STANDBY_TIME_62_5_MS	= 0b001,
			STANDBY_TIME_125_MS		= 0b010,
			STANDBY_TIME_250_MS		= 0b011,
			STANDBY_TIME_500_MS		= 0b100,
			STANDBY_TIME_1000_MS	= 0b101,
			STANDBY_TIME_10_MS		= 0b110,
			STANDBY_TIME_20_MS		= 0b111,
		};

		enum IIR_Filter_Coeficient{
			FILTER_COEF_OFF			= 0b000,
			FILTER_COEF_2			= 0b001,
			FILTER_COEF_4			= 0b010,
			FILTER_COEF_8			= 0b011,
			FILTER_COEF_16			= 0b100,
		};

		enum Sampling{
			SKIPPING				= 0b000,
			OVERSAMPLING_X1			= 0b001,
			OVERSAMPLING_X2			= 0b010,
			OVERSAMPLING_X4			= 0b011,
			OVERSAMPLING_X8			= 0b100,
			OVERSAMPLING_X16		= 0b101,
		};

		enum Mode{
			SLEEP 					= 0b00,
			FORCED 					= 0b01,
			NORMAL 					= 0b11
		};

		struct calib_data{
			uint16_t dig_T1;
			int16_t dig_T2;
			int16_t dig_T3;
			uint16_t dig_P1;
			int16_t dig_P2;
			int16_t dig_P3;
			int16_t dig_P4;
			int16_t dig_P5;
			int16_t dig_P6;
			int16_t dig_P7;
			int16_t dig_P8;
			int16_t dig_P9;
			uint8_t dig_H1;
			int16_t dig_H2;
			uint8_t dig_H3;
			int16_t dig_H4;
			int16_t dig_H5;
			int8_t dig_H6;
			int32_t t_fine;
		};

#ifdef BME280_FLOAT_ENABLE
		struct data{
			double pressure;			/*! Compensated pressure */
			double temperature;			/*! Compensated temperature */
			double humidity;			/*! Compensated humidity */
		};
#else	/* BME280_FLOAT_ENABLE */
		struct data{
			uint32_t pressure;		    /*! Compensated pressure */
		    int32_t temperature;		/*! Compensated temperature */
		    uint32_t humidity;		    /*! Compensated humidity */
		};
#endif /* BME280_FLOAT_ENABLE */
		struct settings{
			uint8_t desired_set;
			Sampling osr_p;					/*! pressure oversampling */
			Sampling osr_t;					/*! temperature oversampling */
			Sampling osr_h;					/*! humidity oversampling */
			IIR_Filter_Coeficient filter;	/*! filter coefficient */
			Stand_By_Time_ms standby_time; 	/*! standby time */
		};

		virtual ~BME280(){}

		int get_mode(BME280::Mode *mode);
		int set_mode(BME280::Mode mode);

		int config(settings* settings);

		int get_data(uint8_t sensor_comp, data *comp_data);

		virtual int soft_reset();

		uint8_t get_chip_id();
		int check_chip_id();

	protected:
		calib_data calib_data;

		int get_calib_data();

		int set_sleep_mode();

		int write_power_mode(uint8_t sensor_mode);

		int reload_device_settings(const settings *settings);

		int set_filter_standby_settings(const settings *settings);
		int set_osr_settings(const settings *settings);
		int set_osr_humidity_settings(const settings *settings);
		int set_osr_press_temp_settings(const settings *settings);

		virtual int read_command(uint8_t cmd, uint8_t* aws, unsigned int length) = 0;
		virtual int write_command(uint8_t cmd, uint8_t* data, unsigned int length) = 0;
};

#endif /* BME280_H__ */
