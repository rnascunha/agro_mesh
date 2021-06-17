#include "esp_log.h"
#include "../coap_engine.hpp"

#include "gpio.h"

extern const char* RESOURCE_TAG;

extern GPIO_Basic ac_load[];

static void get_ac_load_handler(engine::message const&,
								engine::response& response, void*) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called get ac load handler");


	/**
	 * Option
	 */
	CoAP::Message::content_format format = CoAP::Message::content_format::text_plain;
	CoAP::Message::Option::node content{format};

	/**
	 * Payload
	 */
	char buffer[10];
	snprintf(buffer, 10, "%d %d %d", ac_load[0].read(), ac_load[1].read(), ac_load[2].read());
	response
		.code(CoAP::Message::code::content)
		.add_option(content)
		.payload(buffer)
		.serialize();
}

static void put_ac_load_handler(engine::message const& request,
								engine::response& response,
								void* engine_void) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called put ac load1 handler");

	/**
	 * Querying value (?value=<0|1>)
	 */
	const void* value = nullptr;
	unsigned length;
	if(!CoAP::Message::query_by_key(request, "value", &value, length) || !length)
	{
		/**
		 * query key 'value' not found.
		 */
		response
			.code(CoAP::Message::code::bad_request)
			.payload("value not set")
			.serialize();
		return;
	}

	/**
	 * The third parameter is the engine...
	 */
	engine* eng = static_cast<engine*>(engine_void);

	/**
	 * Searching our node
	 */
	engine::resource_node* node = eng->root_node().search_node(request);
	if(!node)
	{
		/* Couldn't find */
		response
			.code(CoAP::Message::code::internal_server_error)
			.payload("didn't find node")
			.serialize();
	}

	/**
	 * Loads path are '1', '2' or '3'. Getting the index of array ac_load
	 * based on path
	 */
	unsigned index = node->value().path()[0] - '0' - 1;
//	ESP_LOGI(RESOURCE_TAG, "Index[%s]: %u", node->value().path()[0], index);
	/**
	 * Setting value
	 */
	char v = *static_cast<char const*>(value);
	ac_load[index].write(v != '0' ? 1 : 0);

	/**
	 * Payload
	 */
	char buf[2];
	snprintf(buf, 2, "%c", ac_load[index].read() + '0');

	response
		.code(CoAP::Message::code::changed)
		.payload(buf)
		.serialize();
}

engine::resource_node res_ac_load("ac_load", get_ac_load_handler);
engine::resource_node res_ac_load1("1",
								static_cast<engine::resource::callback_t*>(nullptr),
								nullptr,
								put_ac_load_handler);
engine::resource_node res_ac_load2("2",
								static_cast<engine::resource::callback_t*>(nullptr),
								nullptr,
								put_ac_load_handler);
engine::resource_node res_ac_load3("3",
								static_cast<engine::resource::callback_t*>(nullptr),
								nullptr,
								put_ac_load_handler);
