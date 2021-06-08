#ifndef PWM_H__
#define PWM_H__

#include "driver/ledc.h"
#include <stdint.h>
#include "driver/gpio.h"

/**
 * TODO Testar todas as funcoes
 * TODO criar mais metodos de manipulacao (interrpucoes)
 */

#define PWM_ERR_OK		1
#define PWM_ERR_CHANNEL	-1
#define PWM_ERR_TIMER	-2

class PWM{
	public:
		PWM(ledc_mode_t mode, ledc_channel_t channel, ledc_timer_t timer) :
			mode(mode), channel(channel), timer(timer){}
//		~PWM(){
//			stop(0);
//		}

		int init(uint32_t freq_hz, gpio_num_t gpio, ledc_timer_bit_t duty_resolution, uint32_t duty);
		void stop(unsigned int level);

		void pause();
		void resume();
		void reset();
	protected:
		ledc_mode_t	mode;
		ledc_channel_t channel;
		ledc_timer_t timer;

};


#endif /* PWM_H__ */
