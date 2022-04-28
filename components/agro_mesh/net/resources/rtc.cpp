#include "esp_log.h"
#include "../coap_engine.hpp"
#include "../../helper/convert.hpp"
#include "../../modules/clock/rtc_time.hpp"

extern Agro::RTC_Time device_clock;

extern const char* RESOURCE_TAG;

static void get_rtc_time_handler(engine::message const& request,
								engine::response& response, void*) noexcept
{
	ESP_LOGI(RESOURCE_TAG, "Called get rtc time handler");

	CoAP::Message::Option::option op;
	CoAP::Message::accept accept_format{CoAP::Message::accept::text_plain};
	if(CoAP::Message::Option::get_option(request, op, CoAP::Message::Option::code::accept))
	{
		accept_format = static_cast<CoAP::Message::accept>(CoAP::Message::Option::parse_unsigned(op));
	}

	/**
	 * Making the payload
	 */
	if(op && accept_format == CoAP::Message::accept::application_octet_stream)
	{
		CoAP::Message::content_format format{CoAP::Message::content_format::application_octet_stream};
		CoAP::Message::Option::node content{format};
		std::uint32_t time = device_clock.get_time();//dt.getUnixTime();

		/**
		 * Sending response (always call serialize)
		 */
		response
				.code(CoAP::Message::code::content)
				.add_option(content)
				.payload(&time, sizeof(std::uint32_t))
				.serialize();
	}
	else
	{
		char buffer[20];
		snprintf(buffer, 20, "%u", device_clock.get_time()/*dt.getUnixTime()*/);
		CoAP::Message::content_format format = CoAP::Message::content_format::text_plain;
		CoAP::Message::Option::node content{format};

		/**
		 * Sending response (always call serialize)
		 */
		response
				.code(CoAP::Message::code::content)
				.add_option(content)
				.payload(buffer)
				.serialize();
	}
}

static void put_rtc_time_handler(engine::message const& request,
								engine::response& response, void*) noexcept
{
	ESP_LOGI(RESOURCE_TAG, "Called put rtc time handler");

	if(!request.payload || request.payload_len == 0)
	{
		response
			.code(CoAP::Message::code::bad_request)
			.payload("data not set")
			.serialize();
		return;
	}

	CoAP::Message::Option::option op;
	CoAP::Message::content_format content_format = CoAP::Message::content_format::text_plain;
	if(CoAP::Message::Option::get_option(request, op, CoAP::Message::Option::code::content_format))
	{
		content_format = static_cast<CoAP::Message::content_format>(CoAP::Message::Option::parse_unsigned(op));
	}

	std::uint32_t unix_time;
	if(op && content_format == CoAP::Message::content_format::application_octet_stream)
	{
		unix_time = *static_cast<const uint32_t*>(request.payload);
	}
	else
	{
		unix_time = strntoll(static_cast<const char*>(request.payload), request.payload_len);
	}

	device_clock.set_time(unix_time);

	response
			.code(CoAP::Message::code::changed)
			.serialize();
}

static void get_fuse_handler(engine::message const& request,
								engine::response& response, void*) noexcept
{
	ESP_LOGI(RESOURCE_TAG, "Called get fuse time handler");

	CoAP::Message::Option::option op;
	CoAP::Message::accept accept_format = CoAP::Message::accept::text_plain;
	if(CoAP::Message::Option::get_option(request, op, CoAP::Message::Option::code::accept))
	{
		accept_format = static_cast<CoAP::Message::accept>(CoAP::Message::Option::parse_unsigned(op));
	}

	/**
	 * Making the payload
	 */
	if(op && accept_format == CoAP::Message::accept::application_octet_stream)
	{
		CoAP::Message::content_format format{CoAP::Message::content_format::application_octet_stream};
		CoAP::Message::Option::node content{format};
		Agro::fuse_type fuse = device_clock.fuse();

		/**
		 * Sending response (always call serialize)
		 */
		response
				.code(CoAP::Message::code::content)
				.add_option(content)
				.payload(&fuse, sizeof(Agro::fuse_type))
				.serialize();
	}
	else
	{
		char buffer[20];
		snprintf(buffer, 20, "%d", device_clock.fuse());
		CoAP::Message::content_format format{CoAP::Message::content_format::text_plain};
		CoAP::Message::Option::node content{format};

		/**
		 * Sending response (always call serialize)
		 */
		response
				.code(CoAP::Message::code::content)
				.add_option(content)
				.payload(buffer)
				.serialize();
	}
}

static void put_fuse_handler(engine::message const& request,
								engine::response& response, void*) noexcept
{
	ESP_LOGI(RESOURCE_TAG, "Called put fuse time handler");

	if(!request.payload || request.payload_len == 0)
	{
		response
			.code(CoAP::Message::code::bad_request)
			.payload("data not set")
			.serialize();
		return;
	}

	CoAP::Message::Option::option op;
	CoAP::Message::content_format content_format{CoAP::Message::content_format::text_plain};
	if(CoAP::Message::Option::get_option(request, op, CoAP::Message::Option::code::content_format))
	{
		content_format = static_cast<CoAP::Message::content_format>(CoAP::Message::Option::parse_unsigned(op));
	}

	Agro::fuse_type fuse;
	if(op && content_format == CoAP::Message::content_format::application_octet_stream)
	{
		fuse = *static_cast<const Agro::fuse_type*>(request.payload);
	}
	else
	{
		fuse = strntoll(static_cast<const char*>(request.payload), request.payload_len);
	}

	device_clock.fuse(fuse);

	response
			.code(CoAP::Message::code::changed)
			.serialize();
}

engine::resource_node res_rtc_time("rtc",
									get_rtc_time_handler,	//get
									nullptr,				//port
									put_rtc_time_handler,	//put
									nullptr);				//delete

engine::resource_node res_fuse_time("fuse",
									get_fuse_handler,
									nullptr,
									put_fuse_handler);
