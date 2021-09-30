#ifndef GPIO_H__
#define GPIO_H__

#include "driver/gpio.h"

/**
 * TODO fazer melhor controle da variavel driver_instaled, que contém quantos dispositivos
 * 	estao se utilizando do driver de interrucao
 */

class GPIO_Basic{
	public:
		GPIO_Basic(gpio_num_t num) : num(num){}
		GPIO_Basic(gpio_num_t num, gpio_mode_t mode);

		void mode(gpio_mode_t mode);

		void write(int level);
		void toggle();
		int read();

		void reset() noexcept;

		void register_interrupt(gpio_isr_t handler, void* isr_args);
		void unregister_interrupt();

		void enable_interrupt(gpio_int_type_t type);
		void disable_interrupt();

		void pull(gpio_pull_mode_t mode);

		void strength(gpio_drive_cap_t cap);
		void hold(bool hold);
		static void deep_sleep_hold(bool hold);

		void* get_isr_args();

		~GPIO_Basic();

	protected:
		int level = 0;
		gpio_num_t num;
		void* isr_args = NULL;

		static unsigned int driver_instaled;
};

typedef struct {
	GPIO_Basic* esse;
	void* args;
}gpio_isr_args_t;

#endif /* GPIO_H__ */
