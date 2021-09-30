#include "gpio.h"

#ifndef GPIO_INTERRUPT_FLAGS
#define GPIO_INTERRUPT_FLAGS		(ESP_INTR_FLAG_LEVEL3|ESP_INTR_FLAG_IRAM)
#endif

unsigned int GPIO_Basic::driver_instaled = 0;

GPIO_Basic::GPIO_Basic(gpio_num_t num, gpio_mode_t mode) : level(0){
	this->num = num;
	gpio_set_direction(num,mode);
}

void GPIO_Basic::mode(gpio_mode_t mode){
	gpio_set_direction(num,mode);
}

void GPIO_Basic::write(int level){
	gpio_set_level(num, level);
}

void GPIO_Basic::toggle(){
	level = !level;
	write(level);
}

int GPIO_Basic::read(){
	level = gpio_get_level(num);
	return level;
}

void GPIO_Basic::reset() noexcept
{
	gpio_reset_pin(num);
}

void GPIO_Basic::pull(gpio_pull_mode_t mode){
	gpio_set_pull_mode(num, mode);
}

void GPIO_Basic::strength(gpio_drive_cap_t cap){
	gpio_set_drive_capability(num, cap);
}

void GPIO_Basic::hold(bool hold){
	if(hold){
		gpio_hold_en(num);
	} else {
		gpio_hold_dis(num);
	}
}

void GPIO_Basic::deep_sleep_hold(bool hold){
	if(hold){
		gpio_deep_sleep_hold_en();
	} else {
		gpio_deep_sleep_hold_dis();
	}
}

void GPIO_Basic::register_interrupt(gpio_isr_t handler, void* isr_args){
	if(driver_instaled == 0){
		gpio_install_isr_service(GPIO_INTERRUPT_FLAGS);
		driver_instaled++;
	}

	this->isr_args = isr_args;
	gpio_isr_handler_add(num, handler,this);
}

void GPIO_Basic::unregister_interrupt(){
	if(driver_instaled == 0){
		return;
	}

	gpio_isr_handler_remove(num);
	disable_interrupt();

	if(--driver_instaled){
		gpio_uninstall_isr_service();
	}
}

void GPIO_Basic::enable_interrupt(gpio_int_type_t type){
	gpio_set_intr_type(num, type);
	gpio_intr_enable(num);
}

void GPIO_Basic::disable_interrupt(){
	gpio_intr_disable(num);
}

void* GPIO_Basic::get_isr_args(){
	return isr_args;
}

GPIO_Basic::~GPIO_Basic(){
	unregister_interrupt();
}
