#include "pwm.h"
#include "esp_err.h"

int PWM::init(uint32_t freq_hz, gpio_num_t gpio, ledc_timer_bit_t duty_resolution, uint32_t duty){
	ledc_timer_config_t timer_conf = {
		.speed_mode = mode,
		.duty_resolution = duty_resolution,
		.timer_num = timer,
		.freq_hz = freq_hz,
		.clk_cfg = LEDC_USE_REF_TICK, 	//Check this config
	};

	if(ledc_timer_config(&timer_conf) != ESP_OK) {
		return PWM_ERR_TIMER;
	}

	ledc_channel_config_t ch_conf = {
		.gpio_num = gpio,
		.speed_mode = mode,
		.channel = channel,
		.intr_type = LEDC_INTR_DISABLE,
		.timer_sel = timer,
		.duty = duty,
		.hpoint = 0
	};

	if (ledc_channel_config(&ch_conf) != ESP_OK) {
		return PWM_ERR_CHANNEL;
	}
	return PWM_ERR_OK;
}

void PWM::stop(unsigned int level){
	ledc_stop(mode, channel, level);
}

void PWM::pause(){
	ledc_timer_pause(mode, timer);
}

void PWM::resume(){
	ledc_timer_resume(mode, timer);
}

void PWM::reset(){
	ledc_timer_rst(mode, timer);
}
